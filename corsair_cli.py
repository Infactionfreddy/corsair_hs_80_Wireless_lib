#!/usr/bin/env python3
"""corsair_cli.py

Minimal CLI to read battery percentage, charging state, volume and mute
state from Corsair HS80-like headsets using hidapi.

This is a heuristic implementation: different firmware versions expose
different reports. The script attempts to read standard feature/input
reports and extract plausible values. If no device is found it prints
an informative JSON object.

Usage:
  python3 corsair_cli.py --json

Dependencies: hid (pip package name: hid)
"""
import sys
import argparse
import json
import struct

try:
    import hid
except Exception:
    hid = None

# Windows-specific: pycaw to read system master volume & mute
IS_WINDOWS = sys.platform.startswith('win')
if IS_WINDOWS:
    try:
        from comtypes import CLSCTX_ALL
        from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
    except Exception:
        AudioUtilities = None
        IAudioEndpointVolume = None


def find_corsair_devices():
    if hid is None:
        return []
    devices = hid.enumerate()
    corsair = []
    for d in devices:
        vendor = (d.get('manufacturer') or '').lower()
        product = (d.get('product_string') or '').lower()
        if 'corsair' in vendor or 'corsair' in product or d.get('vendor_id') in (0x1B1C,):
            corsair.append(d)
    return corsair


def try_read_feature(device_info):
    # Try to open and read a feature report or input report from the device.
    # We will attempt several report IDs that some Corsair devices use.
    if hid is None:
        return None
    path = device_info.get('path')
    try:
        h = hid.device()
        # Open by path if available, otherwise by VID/PID
        if path:
            h.open_path(path)
        else:
            h.open(device_info['vendor_id'], device_info['product_id'])
        h.set_nonblocking(True)
    except Exception:
        return None

    # Try reading input report
    try:
        data = h.read(64)
        if data:
            return bytes(data)
    except Exception:
        pass

    # Try feature reports with common IDs
    for rid in (1, 2, 3, 4, 5, 0x10, 0x11, 0x12):
        try:
            # feature reports are read with report id prefixed
            buf = h.get_feature_report(rid, 64)
            if buf and any(buf):
                return bytes(buf)
        except Exception:
            continue

    try:
        h.close()
    except Exception:
        pass
    return None


def parse_report(data: bytes):
    # Heuristically scan bytes for plausible battery, charging, volume, mute
    # Battery is often a single byte 0-100. Volume 0-100. Mute often 0/1.
    res = {
        'battery_percent': None,
        'charging': None,
        'volume_percent': None,
        'muted': None,
        'raw_hex': data.hex() if data else None,
    }
    if not data:
        return res

    # Look for bytes in 0-100 range and pick likely candidates.
    candidates = [b for b in data if 0 <= b <= 100]
    if candidates:
        # battery candidate: the first value near 20-100
        for c in candidates:
            if 10 <= c <= 100:
                res['battery_percent'] = int(c)
                break

    # Charging: look for flag bytes 0x01 or 0x02 near any known positions
    if len(data) >= 2:
        # simple heuristic: if any byte equals 1 and next byte looks like battery,
        # interpret as charging flag
        for i in range(len(data)-1):
            if data[i] in (1, 2) and 0 <= data[i+1] <= 100:
                res['charging'] = bool(data[i] == 1)
                if res['battery_percent'] is None:
                    res['battery_percent'] = int(data[i+1])
                break

    # Volume/mute: search for sequences where a byte 0/1 is followed by 0-100
    for i in range(len(data)-1):
        a, b = data[i], data[i+1]
        if a in (0, 1) and 0 <= b <= 100:
            # ambiguous: could be mute flag then volume
            res['muted'] = bool(a == 1)
            res['volume_percent'] = int(b)
            break

    # Another heuristic: find distinct high-range values for volume (e.g., >0)
    if res['volume_percent'] is None:
        for c in reversed(candidates):
            if 0 <= c <= 100:
                res['volume_percent'] = int(c)
                break

    return res


def main(argv=None):
    parser = argparse.ArgumentParser(description='Corsair HS80 quick CLI')
    parser.add_argument('--json', action='store_true', help='Print JSON output')
    args = parser.parse_args(argv)

    devices = find_corsair_devices()
    out = {
        'found': len(devices) > 0,
        'devices_scanned': len(devices),
        'devices': [],
        'system': {},
    }

    # Windows system audio as fallback/extra info
    if IS_WINDOWS and AudioUtilities is not None:
        try:
            sessions = AudioUtilities.GetSpeakers()
            interface = sessions.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
            volume = struct.unpack('f', interface.GetMasterVolumeLevelScalar().to_bytes(4, 'little', signed=False))[0] if False else None
            # The above unpack attempt may fail depending on wrapper; use safer approach:
        except Exception:
            volume = None
        try:
            devices = AudioUtilities.GetSpeakers()
            interface = devices.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
            endpoint = interface.QueryInterface(IAudioEndpointVolume)
            vol_scalar = endpoint.GetMasterVolumeLevelScalar()
            muted = bool(endpoint.GetMute())
            out['system']['volume_percent'] = int(round(vol_scalar * 100)) if vol_scalar is not None else None
            out['system']['muted'] = muted
        except Exception:
            # best-effort; ignore
            out['system']['volume_percent'] = None
            out['system']['muted'] = None

    for d in devices:
        info = dict(d)
        # Normalize keys that may be bytes on some platforms
        for k, v in list(info.items()):
            try:
                if isinstance(v, bytes):
                    info[k] = v.decode(errors='ignore')
            except Exception:
                pass

        report = try_read_feature(d)
        parsed = parse_report(report)
        info['parsed'] = parsed
        out['devices'].append(info)

    if args.json:
        print(json.dumps(out, indent=2))
    else:
        # human-friendly
        if not out['found']:
            print('No Corsair device found.')
            sys.exit(1)
        for dev in out['devices']:
            p = dev['parsed']
            print(f"Device: {dev.get('product_string') or dev.get('path')}")
            print(f"  Battery: {p['battery_percent'] or 'unknown'}%")
            print(f"  Charging: {p['charging']}")
            print(f"  Volume: {p['volume_percent'] or 'unknown'}%")
            print(f"  Muted: {p['muted']}")


if __name__ == '__main__':
    main()
