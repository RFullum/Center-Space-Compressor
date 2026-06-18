cmake_minimum_required(VERSION 3.22)

# Must be set before project() so compiler/SDK checks pick them up.
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment target")
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS architectures (Universal Binary)")

project(CenterSpace VERSION 2.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Windows C++ runtime ------------------------------------------------------
# Statically link the MSVC runtime (/MT, /MTd for Debug) so the shipped VST3 is
# self-contained and does NOT require end users to have the Visual C++
# Redistributable installed. Set before add_subdirectory(JUCE) so every target
# — JUCE's static libs and our own sources alike — links the runtime the same
# way; a mismatch produces LNK4098 / runtime-heap corruption. Relies on policy
# CMP0091 (MSVC runtime selected via abstraction), default-NEW on CMake 3.15+.
if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# --- JUCE ---------------------------------------------------------------------
# JUCE is vendored as a git submodule at ./JUCE
add_subdirectory(JUCE)

# --- Plugin target ------------------------------------------------------------
# Sources and binary data are wired up in subsequent sub-steps (5.4 / 5.5).
# Per-config build flags (Universal Binary, optimisation, LTO) are in 5.6.
#
# COPY_PLUGIN_AFTER_BUILD auto-installs the freshly-built plugin so it can be
# loaded in a DAW without a manual copy. On macOS/Linux JUCE targets a
# user-writable dir (~/Library/Audio/Plug-Ins, ~/.vst3), so it just works. On
# Windows the only standard VST3 location is the SYSTEM path
# C:\Program Files\Common Files\VST3, which needs administrator rights — an
# unelevated build fails the post-build copy (copyDir.cmake / MSB3073). We
# package the zip straight from the build artefacts, so the auto-install is
# unnecessary on Windows; disable it there to keep non-admin builds green.
if(WIN32)
    set(CENTERSPACE_COPY_AFTER_BUILD FALSE)
else()
    set(CENTERSPACE_COPY_AFTER_BUILD TRUE)
endif()

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
    COPY_PLUGIN_AFTER_BUILD     ${CENTERSPACE_COPY_AFTER_BUILD})

# JUCE 8 defaults JUCE_VST3_CAN_REPLACE_VST2 to 1, but Center Space never had
# a VST2 ancestor. Disable so the param-ID mismatch check stays quiet.
#
# JUCE_DISPLAY_SPLASH_SCREEN / JUCE_REPORT_APP_USAGE are turned off under
# the FullumMusic JUCE Pro license.
target_compile_definitions(CenterSpace PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    # Center Space has no WebBrowserComponent. Disabling the web browser drops
    # the GTK/WebKit dependency that juce_gui_extra otherwise pulls in on Linux
    # (juce_gui_extra.cpp -> <gtk/gtk.h>). Inert on macOS, which uses WebKit.
    JUCE_WEB_BROWSER=0)

# --- Sources ------------------------------------------------------------------
set(CenterSpace_SOURCES
    Source/Plugin/PluginProcessor.cpp
    Source/Plugin/PluginProcessor.h
    Source/Plugin/PluginEditor.cpp
    Source/Plugin/PluginEditor.h
    Source/DSP/Derivations.h
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
    Source/GUI/GUIUtils/ColorPalette.h
    Source/GUI/GUIUtils/GuiResources.h
    Source/GUI/GUIUtils/CenterSpaceHelpers.h
    Source/GUI/Components/ABButton.cpp
    Source/GUI/Components/ABButton.h
    Source/GUI/Metering/Metering.cpp
    Source/GUI/Metering/Metering.h
    Source/GUI/Metering/StereoFieldMeter.cpp
    Source/GUI/Metering/StereoFieldMeter.h
    Source/GUI/Metering/SidechainGainMeter.cpp
    Source/GUI/Metering/SidechainGainMeter.h
    Source/GUI/Metering/GainReductionMeter.cpp
    Source/GUI/Metering/GainReductionMeter.h)

target_sources(CenterSpace PRIVATE ${CenterSpace_SOURCES})

set(FullumModules_SOURCES
    FullumModules/Signal/Signal.h
    FullumModules/Signal/Property.h
    FullumModules/Signal/OwnedProperty.h
    FullumModules/Signal/PropertyReader.h
    FullumModules/Signal/PropertyBridge.h
    FullumModules/Signal/HoldableOnChange.h
    FullumModules/Libraries/sigslot/signal.hpp
    FullumModules/State/PatchManager.cpp
    FullumModules/State/PatchManager.h
    FullumModules/State/ABCompareManager.cpp
    FullumModules/State/ABCompareManager.h
    FullumModules/GUI/ColorPalette.h
    FullumModules/GUI/PatchControls.cpp
    FullumModules/GUI/PatchControls.h
    FullumModules/GUI/PatchSelectionPopup.cpp
    FullumModules/GUI/PatchSelectionPopup.h
    FullumModules/GUI/GuiHelpers.h
    FullumModules/GUI/FullumLookAndFeel.cpp
    FullumModules/GUI/FullumLookAndFeel.h
    FullumModules/GUI/TitleFooter.cpp
    FullumModules/GUI/TitleFooter.h
    FullumModules/GUI/MeterScaling.h
    FullumModules/GUI/Format.h
    FullumModules/GUI/MeterBallistics.h
    FullumModules/GUI/MeterScale.cpp
    FullumModules/GUI/MeterScale.h
    FullumModules/GUI/Selector.cpp
    FullumModules/GUI/Selector.h)

target_sources(CenterSpace PRIVATE ${FullumModules_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${FullumModules_SOURCES})

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
    Source/DSP
    Source/GUI/TweakComponents
    Source/GUI/VibeComponents
    Source/GUI/TitleHeaderFooter
    Source/GUI/Metering
    Source/GUI/GUIUtils
    Source/GUI/Components
    # NOTE: add the FullumModules *root* to the include path, not its subfolders.
    # Putting FullumModules/Signal directly on the path makes its Signal.h shadow
    # the POSIX <signal.h> on case-insensitive filesystems (macOS/Windows),
    # breaking JUCE's juce_core. Headers are referenced folder-rooted, e.g.
    # #include "Signal/Property.h". Libraries/ stays on the path so the migrated
    # headers' #include "sigslot/signal.hpp" resolves.
    FullumModules
    FullumModules/State
    FullumModules/Libraries)

# --- Factory presets ----------------------------------------------------------
# Per-bundle POST_BUILD: copy Resources/FactoryPresets/*.cspatch into each
# plugin bundle's Contents/Resources/Patches/. PatchManager reads from that
# path at runtime — see PatchManager::ResolveFactoryPatchesDirectory().
# The shell script also pushes a copy into the installed bundle under
# ~/Library/Audio/Plug-Ins/<format>/ so JUCE's COPY_PLUGIN_AFTER_BUILD step
# (which may have already run) doesn't leave a stale install missing patches.
set(FACTORY_PRESETS_SRC    "${CMAKE_CURRENT_SOURCE_DIR}/Resources/FactoryPresets")
set(FACTORY_PRESETS_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy_factory_presets.sh")

if(APPLE)
    # macOS: each plugin is a true Bundle target, so $<TARGET_BUNDLE_DIR> is
    # valid. The shell script also syncs into ~/Library/Audio/Plug-Ins/.
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
elseif(UNIX OR WIN32)
    # Linux & Windows: the VST3 uses a bundle-style layout, but it is NOT a CMake
    # Bundle target, so $<TARGET_BUNDLE_DIR> is unavailable. The binary lands in
    # Center Space.vst3/Contents/<arch>-linux/ (or <arch>-win/ on Windows), so
    # $<TARGET_FILE_DIR>/../Resources resolves to Center Space.vst3/Contents/
    # Resources — where PatchManager::ResolveFactoryPatchesDirectory() looks at
    # runtime. The copy commands are generator-agnostic (cmake -E), so the same
    # branch serves both platforms.
    if(TARGET CenterSpace_VST3)
        set(_linux_vst3_patches "$<TARGET_FILE_DIR:CenterSpace_VST3>/../Resources/Patches")
        add_custom_command(TARGET CenterSpace_VST3 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_linux_vst3_patches}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${FACTORY_PRESETS_SRC}" "${_linux_vst3_patches}"
            VERBATIM
            COMMENT "Copying factory presets into CenterSpace_VST3 (Linux)")
    endif()
endif()

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

# --- Code signing (opt-in) ----------------------------------------------------
# Enables Developer ID signing + hardened runtime entitlements on each plugin
# bundle. Opt-in via the CENTERSPACE_SIGN_IDENTITY environment variable so
# contributors without an Apple Developer cert still get clean builds.
#
# Usage:
#   export CENTERSPACE_SIGN_IDENTITY="Developer ID Application: Robert Fullum (A26T94CMW9)"
#   cmake --build --preset=macos-release --target CenterSpace_All
#
# This step ONLY signs the .vst3 / .component / .app bundles. The .pkg
# installer is built and signed separately by scripts/package-and-notarize.sh
# (which also handles notarization + stapling). HARDENED_RUNTIME_ENABLED in
# juce_add_plugin above writes the matching Info.plist flag; the entitlements
# file below carries the disable-library-validation entitlement that AU/VST3
# hosts require when loading us into their address space.
#
# Implementation note: JUCE's Xcode generator emits a built-in CodeSign build
# phase that ad-hoc signs the bundle ("Sign to Run Locally") AFTER any
# add_custom_command(POST_BUILD) we attach. So a custom codesign step gets
# silently overwritten. Instead, we override the Xcode CODE_SIGN_IDENTITY /
# entitlements attributes on the target — Xcode's built-in CodeSign step then
# applies our Developer ID with hardened runtime + entitlements directly.
if(APPLE)
    set(CENTERSPACE_SIGN_IDENTITY_ENV "$ENV{CENTERSPACE_SIGN_IDENTITY}")
    if(CENTERSPACE_SIGN_IDENTITY_ENV)
        set(CENTERSPACE_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/Resources/CenterSpace.entitlements")
        # Parse Team ID out of identity string of form
        # "Developer ID Application: Robert Fullum (A26T94CMW9)".
        string(REGEX MATCH "\\(([A-Z0-9]+)\\)" _team_match "${CENTERSPACE_SIGN_IDENTITY_ENV}")
        set(CENTERSPACE_TEAM_ID "${CMAKE_MATCH_1}")
        message(STATUS "Center Space: Developer ID signing ENABLED")
        message(STATUS "  Identity: ${CENTERSPACE_SIGN_IDENTITY_ENV}")
        message(STATUS "  Team ID:  ${CENTERSPACE_TEAM_ID}")

        foreach(plugin_target IN ITEMS CenterSpace_VST3 CenterSpace_AU CenterSpace_Standalone)
            if(TARGET ${plugin_target})
                set_target_properties(${plugin_target} PROPERTIES
                    XCODE_ATTRIBUTE_CODE_SIGN_STYLE                 "Manual"
                    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY              "${CENTERSPACE_SIGN_IDENTITY_ENV}"
                    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM                "${CENTERSPACE_TEAM_ID}"
                    XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS          "${CENTERSPACE_ENTITLEMENTS}"
                    XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS           "--timestamp --options=runtime"
                    XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME         "YES")
            endif()
        endforeach()
    else()
        message(STATUS "Center Space: Developer ID signing disabled (set CENTERSPACE_SIGN_IDENTITY to enable)")
    endif()
endif()

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
