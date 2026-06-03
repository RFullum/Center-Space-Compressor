#!/bin/bash
# ==============================================================================
# Center Space — Package + Notarize
# ==============================================================================
# Builds a signed, notarized, stapled .pkg installer for distribution.
#
# Prereqs (one-time):
#   - Developer ID Application + Developer ID Installer certs in login keychain
#   - notarytool credentials stored under profile "centerspace-notary":
#       xcrun notarytool store-credentials "centerspace-notary" \
#         --apple-id <you@email> --team-id <TEAMID> --password <app-spec-pw>
#   - CENTERSPACE_SIGN_IDENTITY exported in your shell (signs the bundles
#     during the build step). Example:
#       export CENTERSPACE_SIGN_IDENTITY="Developer ID Application: Robert Fullum (A26T94CMW9)"
#
# Usage:
#   ./scripts/package-and-notarize.sh
#
# Output:
#   dist/CenterSpace-<version>.pkg  (signed, notarized, stapled)
# ==============================================================================

set -euo pipefail

# ---- Config ------------------------------------------------------------------

readonly PRODUCT_NAME="Center Space"
readonly BUNDLE_NAME_VST3="Center Space.vst3"
readonly BUNDLE_NAME_AU="Center Space.component"
readonly PKG_IDENTIFIER="com.FullumMusic.CenterSpace.pkg"
readonly NOTARY_PROFILE="centerspace-notary"

# Derived
readonly PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
readonly BUILD_DIR="${PROJECT_ROOT}/build/macos"
readonly ARTEFACTS_DIR="${BUILD_DIR}/CenterSpace_artefacts/Release"
readonly DIST_DIR="${PROJECT_ROOT}/dist"
readonly STAGING_DIR="${DIST_DIR}/staging"

# ---- Helpers -----------------------------------------------------------------

err() { echo "ERROR: $*" >&2; exit 1; }
log() { echo "==> $*"; }

require_env() {
    local var="$1"
    if [[ -z "${!var:-}" ]]; then
        err "Environment variable ${var} is not set. See script header for setup."
    fi
}

require_cmd() {
    command -v "$1" >/dev/null 2>&1 || err "Required command not found: $1"
}

# ---- Preflight ---------------------------------------------------------------

require_env CENTERSPACE_SIGN_IDENTITY
require_cmd cmake
require_cmd codesign
require_cmd productbuild
require_cmd pkgutil

INSTALLER_IDENTITY="${CENTERSPACE_INSTALLER_IDENTITY:-}"
if [[ -z "${INSTALLER_IDENTITY}" ]]; then
    # Derive Installer identity from Application identity by name swap.
    # The two certs are issued to the same name/team, so the substring after
    # "Developer ID " is identical except for "Application" vs "Installer".
    INSTALLER_IDENTITY="${CENTERSPACE_SIGN_IDENTITY/Developer ID Application/Developer ID Installer}"
fi
log "Application identity: ${CENTERSPACE_SIGN_IDENTITY}"
log "Installer identity:   ${INSTALLER_IDENTITY}"

# Verify both identities exist in the keychain.
security find-identity -v -p codesigning | grep -q "${CENTERSPACE_SIGN_IDENTITY}" \
    || err "Application identity not found in keychain: ${CENTERSPACE_SIGN_IDENTITY}"
security find-identity -v -p basic | grep -q "${INSTALLER_IDENTITY}" \
    || err "Installer identity not found in keychain: ${INSTALLER_IDENTITY}"

# Read version from CMakeLists.txt to stamp into the pkg filename.
VERSION=$(grep -E "^project\(CenterSpace VERSION" "${PROJECT_ROOT}/CMakeLists.txt" \
    | sed -E 's/.*VERSION ([0-9]+\.[0-9]+\.[0-9]+).*/\1/')
[[ -n "${VERSION}" ]] || err "Could not parse version from CMakeLists.txt"
log "Building version ${VERSION}"

# ---- Step 1: Build Release ---------------------------------------------------

log "Configuring CMake (Release preset)"
# Show the "Developer ID signing ENABLED" status messages from CMakeLists.txt
# so we can verify signing is actually active before the build runs.
cmake --preset=macos 2>&1 | grep -E "Center Space:|Identity:|Team ID:|--" | sed 's/^/    /' || true

log "Building VST3 + AU targets (Release, signed)"
# Explicitly target only the formats we ship. The Standalone target is a
# developer convenience for offline DSP testing and is intentionally skipped
# here — building it slows down the pipeline and has caused stale-permission
# build failures in the past when its artefacts get touched by an installer
# run between builds.
cmake --build --preset=macos-release --target CenterSpace_VST3
cmake --build --preset=macos-release --target CenterSpace_AU

# ---- Step 2: Verify each bundle was signed correctly -------------------------

verify_bundle() {
    local bundle="$1"
    [[ -d "${bundle}" ]] || err "Missing bundle: ${bundle}"
    log "Verifying signature: $(basename "${bundle}")"
    codesign --verify --strict --verbose=2 "${bundle}" 2>&1 | sed 's/^/    /'
    # Entitlements + hardened runtime are applied at sign time during the build
    # (see Xcode's "CodeSign" step in the build log: `--options=runtime
    # --entitlements ....xcent`). We don't re-verify them here because
    # `codesign -d` output formats vary across macOS releases and produce false
    # negatives on macOS 26+. If signing was actually broken, notarytool will
    # reject the submission with a precise error pointing at the affected
    # binary, and `xcrun notarytool log <id>` will show what to fix.
}

readonly VST3_PATH="${ARTEFACTS_DIR}/VST3/${BUNDLE_NAME_VST3}"
readonly AU_PATH="${ARTEFACTS_DIR}/AU/${BUNDLE_NAME_AU}"

verify_bundle "${VST3_PATH}"
verify_bundle "${AU_PATH}"

# ---- Step 3: Stage files into install-path layout ---------------------------
# We install only the VST3 and AU bundles. The Standalone .app is a
# developer-only artifact for offline DSP testing and is intentionally not
# shipped.
#
# Layout mirrors where they'll end up on the target machine:
#   /Library/Audio/Plug-Ins/VST3/Center Space.vst3
#   /Library/Audio/Plug-Ins/Components/Center Space.component
#
# Both go to the system-wide /Library (not ~/Library) so they're visible to
# every user on the machine, which matches the convention almost every other
# commercial plugin installer follows.

log "Staging bundles for pkg"
rm -rf "${STAGING_DIR}"
mkdir -p "${STAGING_DIR}/Library/Audio/Plug-Ins/VST3"
mkdir -p "${STAGING_DIR}/Library/Audio/Plug-Ins/Components"

# Use ditto so extended attributes / code signatures survive the copy.
ditto "${VST3_PATH}" "${STAGING_DIR}/Library/Audio/Plug-Ins/VST3/${BUNDLE_NAME_VST3}"
ditto "${AU_PATH}"   "${STAGING_DIR}/Library/Audio/Plug-Ins/Components/${BUNDLE_NAME_AU}"

# ---- Step 4: Build the .pkg --------------------------------------------------
# Use pkgbuild rather than `productbuild --root`. The two have a subtle but
# critical difference:
#   - productbuild --root runs auto-component-detection that recognizes
#     .app / .component / .framework / .bundle as installer "components" and
#     SILENTLY DROPS anything else. VST3 (.vst3) isn't on its recognized list,
#     so the entire VST3 bundle gets stripped from the payload — the installer
#     runs, claims success, and the .vst3 never appears on disk.
#   - pkgbuild --root packages the staging tree literally. Every file in the
#     directory ends up in the BOM and on the target machine. No surprises.
# pkgbuild can sign directly, so we don't need a separate productbuild step.

PKG_NAME="CenterSpace-${VERSION}.pkg"
PKG_PATH="${DIST_DIR}/${PKG_NAME}"

log "Building ${PKG_NAME}"
pkgbuild \
    --root "${STAGING_DIR}" \
    --identifier "${PKG_IDENTIFIER}" \
    --version "${VERSION}" \
    --install-location "/" \
    --sign "${INSTALLER_IDENTITY}" \
    --timestamp \
    "${PKG_PATH}"

log "Verifying pkg signature"
pkgutil --check-signature "${PKG_PATH}" | sed 's/^/    /'

# ---- Step 5: Notarize --------------------------------------------------------
# notarytool wants a zip OR a pkg. We submit the pkg directly — simpler than
# zipping a bundle tree, and Apple staples notarization onto the pkg.

log "Submitting to Apple notary service (this can take a few minutes)"
xcrun notarytool submit "${PKG_PATH}" \
    --keychain-profile "${NOTARY_PROFILE}" \
    --wait

# ---- Step 6: Staple ----------------------------------------------------------

log "Stapling notarization ticket to pkg"
xcrun stapler staple "${PKG_PATH}"

log "Validating stapled ticket"
xcrun stapler validate "${PKG_PATH}"

# ---- Step 7: Gatekeeper check ------------------------------------------------

log "Gatekeeper assessment"
spctl --assess --type install --verbose=2 "${PKG_PATH}" 2>&1 | sed 's/^/    /'

# ---- Done --------------------------------------------------------------------

log "Cleaning staging directory"
rm -rf "${STAGING_DIR}"

echo
echo "=============================================================="
echo "  SUCCESS"
echo "  ${PKG_PATH}"
echo "  Signed, notarized, stapled, Gatekeeper-approved."
echo "=============================================================="
