cmake_minimum_required(VERSION 3.22)

# Must be set before project() so compiler/SDK checks pick them up.
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment target")
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS architectures (Universal Binary)")

project(CenterSpace VERSION 2.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- JUCE ---------------------------------------------------------------------
# JUCE is vendored as a git submodule at ./JUCE
add_subdirectory(JUCE)

# --- Plugin target ------------------------------------------------------------
# Sources and binary data are wired up in subsequent sub-steps (5.4 / 5.5).
# Per-config build flags (Universal Binary, optimisation, LTO) are in 5.6.
juce_add_plugin(CenterSpace
    PRODUCT_NAME                "Center Space"
    VERSION                     2.0.0
    COMPANY_NAME                "FullumMusic"
    COMPANY_WEBSITE             "www.fullummusic.com"
    BUNDLE_ID                   "com.FullumMusic.CenterSpace"
    PLUGIN_MANUFACTURER_CODE    FulM
    PLUGIN_CODE                 Crsp
    FORMATS                     VST3 AU Standalone
    DESCRIPTION                 "Sidechain compressor that only compresses the center of the stereo field"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES             Fx Dynamics
    AU_MAIN_TYPE                kAudioUnitType_Effect
    HARDENED_RUNTIME_ENABLED    TRUE
    COPY_PLUGIN_AFTER_BUILD     TRUE)

# JUCE 8 defaults JUCE_VST3_CAN_REPLACE_VST2 to 1, but Center Space never had
# a VST2 ancestor. Disable so the param-ID mismatch check stays quiet.
target_compile_definitions(CenterSpace PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0)

# --- Sources ------------------------------------------------------------------
target_sources(CenterSpace PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Envelope.cpp
    Source/Envelope.h
    Source/OtherLookAndFeel.cpp
    Source/OtherLookAndFeel.h
    Source/TitleHeader.cpp
    Source/TitleHeader.h
    Source/VUMeter.cpp
    Source/VUMeter.h)

# --- Binary data --------------------------------------------------------------
# Compiles assets into a static lib exposing them via `BinaryData::*` symbols.
juce_add_binary_data(CenterSpaceBinaryData
    SOURCES
        Source/Graphics/Fullum_brandcolours_output.png)

target_link_libraries(CenterSpace PRIVATE CenterSpaceBinaryData)

# --- JuceHeader.h -------------------------------------------------------------
# Generated header lets sources keep using `#include <JuceHeader.h>`.
# (Phase 3 cleanup will replace these with per-module includes.)
juce_generate_juce_header(CenterSpace)

# --- Module linkage -----------------------------------------------------------
# juce_audio_plugin_client and juce_audio_processors are linked automatically
# by juce_add_plugin. Everything else the source uses goes here.
target_link_libraries(CenterSpace
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors_headless
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# --- Per-config Xcode build settings ------------------------------------------
# Mirrors the .jucer Xcode configurations:
#   Debug   — host arch only (fast iteration), -O0
#   Release — Universal Binary (arm64 + x86_64), -O3, LTO (via juce_recommended_lto_flags)
# Strip Local Symbols intentionally left off.
foreach(plugin_target IN ITEMS CenterSpace CenterSpace_Standalone CenterSpace_VST3 CenterSpace_AU)
    if(TARGET ${plugin_target})
        set_target_properties(${plugin_target} PROPERTIES
            XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug]          YES
            XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Release]        NO
            XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug]    0
            XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release]  3)
    endif()
endforeach()
