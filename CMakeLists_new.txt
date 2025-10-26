cmake_minimum_required(VERSION 3.15)
project(HS80 VERSION 1.0 LANGUAGES CXX)

# C++17 Standard erforderlich
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# HS80 Library (Statisch)
# ============================================================================
add_library(HS80_Lib STATIC 
    HS80/HS80_Library.cpp
    HS80/HS80_Library.h
)

target_include_directories(HS80_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/HS80)

if(WIN32)
    target_link_libraries(HS80_Lib PUBLIC hid setupapi)
endif()

# ============================================================================
# Original HS80 Executable
# ============================================================================
add_executable(HS80 
    HS80/HS80.cpp
)

target_link_libraries(HS80 PRIVATE HS80_Lib)
target_include_directories(HS80 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/HS80)

# ============================================================================
# HS80 Demo (High-Level API)
# ============================================================================
add_executable(HS80_Demo 
    HS80/HS80_Demo.cpp
)

target_link_libraries(HS80_Demo PRIVATE HS80_Lib)
target_include_directories(HS80_Demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/HS80)

# ============================================================================
# HS80 Analyzer (Analysis & Debugging Tool)
# ============================================================================
add_executable(HS80_Analyzer 
    HS80/HS80_Analyzer.cpp
)

target_link_libraries(HS80_Analyzer PRIVATE HS80_Lib)
target_include_directories(HS80_Analyzer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/HS80)

# Ausgabeverzeichnis
set_target_properties(HS80 HS80_Demo HS80_Analyzer HS80_Lib PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/HS80/Debug"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/HS80/Debug"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/HS80/Debug"
)
