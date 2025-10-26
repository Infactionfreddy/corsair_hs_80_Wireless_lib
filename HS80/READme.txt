
[11] Empfangen [64 bytes]: 03 01 01 A6 00 00 00 00 00 94 03 AA 00 00 00 00 ...
[PROTOKOLL OK] Header: 03 01 01, Event: 0xA6
[UNBEKANNTES EVENT] Code: 0xA6
6 Byte 00 unmute


[12] Empfangen [64 bytes]: 03 01 01 A6 00 01 00 00 00 94 03 AA 00 00 00 00 ...
[PROTOKOLL OK] Header: 03 01 01, Event: 0xA6
[UNBEKANNTES EVENT] Code: 0xA6
6 Byte 01 mute


[13] Empfangen [64 bytes]: 03 01 01 0F 00 80 02 00 00 94 03 AA 00 00 00 00 ...
[PROTOKOLL OK] Header: 03 01 01, Event: 0xF
>>> BATTERIE-UPDATE <<<
Batteriestand: 64.0%


0x02 senden, 	0x03 empfangen  	// Report ID
0x09 Steuern , 	0x01 Report 	 // Vendor Command
0x06 LED , 	0x01 report-daten // Sub-command
		0x0F Akku stand,  0x10 geladen 
