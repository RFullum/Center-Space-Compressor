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
#
# JUCE_DISPLAY_SPLASH_SCREEN / JUCE_REPORT_APP_USAGE are turned off under
# the FullumMusic JUCE Pro license.
target_compile_definitions(CenterSpace PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0)

# Optional shutdown trace — writes timestamped destructor markers to
# ~/Library/Logs/CenterSpace/shutdown.log. Used to diagnose host beachballing
# during quit. Configure with -DCENTERSPACE_TRACE_SHUTDOWN=OFF to enable.
option(CENTERSPACE_TRACE_SHUTDOWN "Enable plugin shutdown trace logging" OFF)
if(CENTERSPACE_TRACE_SHUTDOWN)
    target_compile_definitions(CenterSpace PUBLIC CS_TRACE_SHUTDOWN=1)
    message(STATUS "Center Space: shutdown trace ENABLED -> ~/Library/Logs/CenterSpace/shutdown.log")
endif()

# --- Sources ------------------------------------------------------------------
set(CenterSpace_SOURCES
    Source/Plugin/PluginProcessor.cpp
    Source/Plugin/PluginProcessor.h
    Source/Plugin/PluginEditor.cpp
    Source/Plugin/PluginEditor.h
    Source/Plugin/ShutdownTrace.h
    Source/Plugin/PatchEngine/PatchManager.cpp
    Source/Plugin/PatchEngine/PatchManager.h
    Source/Plugin/PatchEngine/ABCompareManager.cpp
    Source/Plugin/PatchEngine/ABCompareManager.h
    Source/DSP/Derivations.h
    Source/GUI/LookAndFeel/CSLookAndFeel.cpp
    Source/GUI/LookAndFeel/CSLookAndFeel.h
    Source/GUI/TweakComponents/TweakDetection.cpp
    Source/GUI/TweakComponents/TweakDetection.h
    Source/GUI/TweakComponents/TweakDynamics.cpp
    Source/GUI/TweakComponents/TweakDynamics.h
    Source/GUI/TweakComponents/TweakLayout.cpp
    Source/GUI/TweakComponents/TweakLayout.h
    Source/GUI/VibeComponents/VibeDetection.cpp
    Source/GUI/VibeComponents/VibeDetection.h
    Source/GUI/VibeComponents/VibeDynamics.cpp
    Source/GUI/VibeComponents/VibeDynamics.h
    Source/GUI/VibeComponents/VibeLayout.cpp
    Source/GUI/VibeComponents/VibeLayout.h
    Source/GUI/TitleHeaderFooter/TitleHeader.cpp
    Source/GUI/TitleHeaderFooter/TitleHeader.h
    Source/GUI/TitleHeaderFooter/TitleFooter.cpp
    Source/GUI/TitleHeaderFooter/TitleFooter.h
    Source/GUI/GUIUtils/ColorPalette.h
    Source/GUI/GUIUtils/GuiResources.h
    Source/GUI/GUIUtils/GuiHelpers.h
    Source/GUI/Components/Selector.cpp
    Source/GUI/Components/Selector.h
    Source/GUI/Components/PatchControls.cpp
    Source/GUI/Components/PatchControls.h
    Source/GUI/Components/PatchSelectionPopup.cpp
    Source/GUI/Components/PatchSelectionPopup.h
    Source/GUI/Metering/Metering.cpp
    Source/GUI/Metering/Metering.h
    Source/GUI/Metering/StereoFieldMeter.cpp
    Source/GUI/Metering/StereoFieldMeter.h
    Source/GUI/Metering/SidechainGainMeter.cpp
    Source/GUI/Metering/SidechainGainMeter.h
    Source/GUI/Metering/GainReductionMeter.cpp
    Source/GUI/Metering/GainReductionMeter.h
    Source/GUI/Metering/MeterScale.cpp
    Source/GUI/Metering/MeterScale.h
    Source/GUI/Metering/MeterScaling.h
    Source/Signal/signal/Signal.h
    Source/Signal/signal/Property.h
    Source/Signal/signal/OwnedProperty.h
    Source/Signal/signal/PropertyReader.h
    Source/Signal/signal/PropertyBridge.h
    Source/Signal/signal/HoldableOnChange.h
    Source/ThirdParty/sigslot/signal.hpp)

target_sources(CenterSpace PRIVATE ${CenterSpace_SOURCES})

# Mirror on-disk folder structure in IDE navigators (Xcode, Visual Studio).
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${CenterSpace_SOURCES})

# Collapse JUCE module sources under a single IDE group instead of dumping
# them into the default "Source Files" / "Header Files" buckets.
file(GLOB_RECURSE _juce_module_files
    "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/modules/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/modules/*.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/modules/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/modules/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/modules/*.hpp")
source_group("JUCE Library Code" FILES ${_juce_module_files})

# Source subdirectories on the include path so existing #include "Foo.h"
# directives keep working without per-file path prefixes.
target_include_directories(CenterSpace PRIVATE
    Source/Plugin
    Source/Plugin/PatchEngine
    Source/DSP
    Source/GUI/LookAndFeel
    Source/GUI/TweakComponents
    Source/GUI/VibeComponents
    Source/GUI/TitleHeaderFooter
    Source/GUI/Metering
    Source/GUI/GUIUtils
    Source/GUI/Components
    Source/Signal
    Source/ThirdParty)

# --- Factory presets ----------------------------------------------------------
# Per-bundle POST_BUILD: copy Resources/FactoryPresets/*.cspatch into each
# plugin bundle's Contents/Resources/Patches/. PatchManager reads from that
# path at runtime — see PatchManager::ResolveFactoryPatchesDirectory().
# The shell script also pushes a copy into the installed bundle under
# ~/Library/Audio/Plug-Ins/<format>/ so JUCE's COPY_PLUGIN_AFTER_BUILD step
# (which may have already run) doesn't leave a stale install missing patches.
set(FACTORY_PRESETS_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/Resources/FactoryPresets")
set(FACTORY_PRESETS_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy_factory_presets.sh")

foreach(plugin_target IN ITEMS CenterSpace_VST3 CenterSpace_AU CenterSpace_Standalone)
    if(TARGET ${plugin_target})
        add_custom_command(TARGET ${plugin_target} POST_BUILD
            COMMAND /bin/sh "${FACTORY_PRESETS_SCRIPT}"
                    "$<TARGET_BUNDLE_DIR:${plugin_target}>"
                    "${FACTORY_PRESETS_SRC}"
            VERBATIM
            COMMENT "Copying factory presets into ${plugin_target}")
    endif()
endforeach()

# --- JuceHeader.h -------------------------------------------------------------
# Generated header lets sources keep using `#include <JuceHeader.h>`.
# (Phase 3 cleanup will replace these with per-module includes.)
juce_generate_juce_header(CenterSpace)

# --- Module linkage -----------------------------------------------------------
# juce_audio_plugin_client and juce_audio_processors are linked automatically
# by juce_add_plugin. Everything else the source uses goes here.
# Targeted warning suppressions, applied AFTER juce_recommended_warning_flags
# so they sit last on the compile command line and aren't clobbered by JUCE's
# -Wshadow-all (which would re-enable -Wshadow-field-in-constructor).
#   -Wno-shadow-field-in-constructor          — common pattern in this codebase
#       (e.g. CSLookAndFeel(GuiResources &resources) : resources(resources)).
#       All other -Wshadow categories stay on.
#   -Wno-inconsistent-missing-destructor-override
#       — silences the destructor-specific "needs override" warning. Regular
#         method-override checks stay on.
add_library(centerspace_warning_overrides INTERFACE)
target_compile_options(centerspace_warning_overrides INTERFACE
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wno-shadow-field-in-constructor>
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wno-inconsistent-missing-destructor-override>)

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
        juce::juce_recommended_warning_flags
        centerspace_warning_overrides)

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

# --- DSP test harness ---------------------------------------------------------
# Offline console executable that exercises DSP units without a DAW or GUI.
# Build:  cmake --build --preset=macos-debug --target centerspace_tests
# Run:    ctest --preset=macos    (or run the binary directly)
juce_add_console_app(centerspace_tests
    PRODUCT_NAME "CenterSpace Tests")

set(CenterSpace_Tests_SOURCES
    Tests/TestMain.cpp)

target_sources(centerspace_tests PRIVATE ${CenterSpace_Tests_SOURCES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${CenterSpace_Tests_SOURCES})

target_link_libraries(centerspace_tests
    PRIVATE
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

target_include_directories(centerspace_tests PRIVATE
    Source/DSP)

target_compile_definitions(centerspace_tests PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0)

enable_testing()
add_test(NAME centerspace_tests COMMAND centerspace_tests)
