#!/bin/sh
# ------------------------------------------------------------------------------
# copy_factory_presets.sh
#
# Sync .cspatch files from the source factory-presets folder into a plugin
# bundle's Contents/Resources/Patches/ directory. Run from CMake as a
# POST_BUILD step per plugin target. Mirrors the DLBS approach.
#
# Args:
#   $1  Bundle path (e.g. .../CenterSpace_artefacts/Release/VST3/Center Space.vst3)
#   $2  Source factory-presets directory (Resources/FactoryPresets)
#
# Also pushes a copy into the installed location under
# ~/Library/Audio/Plug-Ins/<format>/ so JUCE's COPY_PLUGIN_AFTER_BUILD step
# (which may have already run by the time we get here) ends up with the
# patches included. No-op if the install location doesn't exist yet.
# ------------------------------------------------------------------------------

set -e

BUNDLE="$1"
SRC="$2"

if [ ! -d "$SRC" ]; then
    echo "Factory presets source not found: $SRC (no presets to copy)"
    exit 0
fi

if [ ! -d "$BUNDLE" ]; then
    # Static-lib or helper targets: not a bundle, nothing to do.
    exit 0
fi

# Copy into the given bundle (build output).
DEST="$BUNDLE/Contents/Resources/Patches"
mkdir -p "$DEST"
rsync -a --delete --include='*.cspatch' --exclude='*' "$SRC/" "$DEST/"
echo "Factory presets → $DEST"

# Also copy into the installed location. JUCE's COPY_PLUGIN_AFTER_BUILD
# is a separate POST_BUILD that may have already run before us, leaving a
# stale install without our patches. Re-syncing here keeps install in sync
# regardless of POST_BUILD ordering.
BUNDLE_NAME=$(basename "$BUNDLE")
INSTALL_BUNDLE=""

case "$BUNDLE_NAME" in
    *.vst3)
        INSTALL_BUNDLE="$HOME/Library/Audio/Plug-Ins/VST3/$BUNDLE_NAME"
        ;;
    *.component)
        INSTALL_BUNDLE="$HOME/Library/Audio/Plug-Ins/Components/$BUNDLE_NAME"
        ;;
    *.app)
        # Standalone — no system install path.
        ;;
esac

if [ -n "$INSTALL_BUNDLE" ] && [ -d "$INSTALL_BUNDLE" ]; then
    INSTALL_DEST="$INSTALL_BUNDLE/Contents/Resources/Patches"
    mkdir -p "$INSTALL_DEST"
    rsync -a --delete --include='*.cspatch' --exclude='*' "$SRC/" "$INSTALL_DEST/"
    echo "Factory presets → $INSTALL_DEST"
fi
