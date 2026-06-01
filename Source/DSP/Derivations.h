/*
  ==============================================================================

    Derivations.h

    Pure-math helpers for the Vibe/Tweak effective-value derivation layer.
    All functions are free functions in namespace CenterSpace::Derivations,
    take primitive inputs, and have no APVTS dependency. The processor's
    GetEffective*() member methods .load() the atomic params, branch on
    uiMode, and delegate to these.

    Kept header-only so the test harness can pull it in without linking
    against the plugin target.

  ==============================================================================
*/

#pragma once

#include <array>


namespace CenterSpace
{
namespace Derivations
{

//==============================================================================
// Enums. Integer underlying values match the APVTS choice indices so callers
// can cast directly from (int)choiceAtomic->load().

enum class Feel
{
    Clean = 0,
    Smooth = 1
};

enum class Style
{
    ModernVCA = 0,
    Opto = 1
};

enum class PeakMode
{
    Peak = 0,
    RMS = 1
};

enum class LookaheadChoice
{
    Ms0 = 0,
    Ms1 = 1,
    Ms4 = 2,
    Ms10 = 3
};

enum class FocusPreset
{
    FullRange = 0,
    ReduceBass,
    Vocal,
    Kick,
    Bass,
    Transients,
    Low,
    Mid,
    High
};

constexpr int kNumFocusPresets = 9;


//==============================================================================
// Value structs.

struct AttackReleaseRange
{
    float minMs;
    float maxMs;
};

struct ScCutoffs
{
    float hpfHz;
    float lpfHz;
};


//==============================================================================
// Internal helpers.

namespace detail
{
    inline constexpr float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    // Piecewise linear interpolation through three control points at x = 0, 0.5, 1.
    inline float PiecewiseLerp3(float t, float v0, float vMid, float v1)
    {
        if (t <= 0.0f) return v0;
        if (t >= 1.0f) return v1;

        if (t < 0.5f)
            return Lerp(v0, vMid, t * 2.0f);

        return Lerp(vMid, v1, (t - 0.5f) * 2.0f);
    }
}


//==============================================================================
// Compress macro (0..1) → sideInGain / threshold / ratio.
// Control points per CLAUDE.md "Vibe-mode macro derivations":
//   0.0 → +0 dB / 0 dB / 1:1
//   0.5 → +3 dB / -12 dB / 3:1
//   1.0 → +9 dB / -30 dB / 10:1

inline float CompressToSideInGainDb(float compress)
{
    return detail::PiecewiseLerp3(compress, 0.0f, 3.0f, 9.0f);
}

inline float CompressToThresholdDb(float compress)
{
    return detail::PiecewiseLerp3(compress, 0.0f, -12.0f, -30.0f);
}

inline float CompressToRatio(float compress)
{
    return detail::PiecewiseLerp3(compress, 1.0f, 3.0f, 10.0f);
}


//==============================================================================
// Feel bakes.

inline AttackReleaseRange GetAttackRangeForFeel(Feel feel)
{
    return (feel == Feel::Smooth) ? AttackReleaseRange { 5.0f, 100.0f }
                                  : AttackReleaseRange { 0.5f, 30.0f };
}

inline AttackReleaseRange GetReleaseRangeForFeel(Feel feel)
{
    return (feel == Feel::Smooth) ? AttackReleaseRange { 100.0f, 1000.0f }
                                  : AttackReleaseRange { 30.0f, 500.0f };
}

inline Style FeelToStyle(Feel feel)
{
    return (feel == Feel::Smooth) ? Style::Opto : Style::ModernVCA;
}

inline PeakMode FeelToPeakMode(Feel feel)
{
    return (feel == Feel::Smooth) ? PeakMode::RMS : PeakMode::Peak;
}

inline float FeelToKneeDb(Feel feel)
{
    return (feel == Feel::Smooth) ? 12.0f : 0.0f;
}

inline float FeelToLookaheadOnMs(Feel feel)
{
    return (feel == Feel::Smooth) ? 4.0f : 1.0f;
}


//==============================================================================
// React macro (0..1) → ms within a Feel-determined range. Linear interp.

inline float ReactToMs(float react, AttackReleaseRange range)
{
    const float clamped = (react < 0.0f) ? 0.0f : (react > 1.0f ? 1.0f : react);
    return detail::Lerp(range.minMs, range.maxMs, clamped);
}


//==============================================================================
// Focus preset → SC HPF/LPF pair. Table order matches the APVTS StringArray
// in PluginProcessor's parameter layout.

inline ScCutoffs FocusToScCutoffs(FocusPreset preset)
{
    constexpr std::array<ScCutoffs, kNumFocusPresets> table = {{
        {    20.0f, 20000.0f },   // Full Range
        {    80.0f, 20000.0f },   // Reduce Bass
        {   250.0f,  3000.0f },   // Vocal
        {    40.0f,    90.0f },   // Kick
        {    80.0f,   350.0f },   // Bass
        {   400.0f, 15000.0f },   // Transients
        {    20.0f,   250.0f },   // Low
        {   250.0f,   800.0f },   // Mid
        {   800.0f, 20000.0f }    // High
    }};

    const int idx = (int)preset;

    if (idx < 0 || idx >= kNumFocusPresets)
        return table[0];

    return table[(size_t)idx];
}


//==============================================================================
// Tweak-mode lookahead choice → ms.

inline float LookaheadChoiceToMs(LookaheadChoice choice)
{
    switch (choice)
    {
        case LookaheadChoice::Ms0:  return 0.0f;
        case LookaheadChoice::Ms1:  return 1.0f;
        case LookaheadChoice::Ms4:  return 4.0f;
        case LookaheadChoice::Ms10: return 10.0f;
    }
    return 0.0f;
}


//==============================================================================
// Sample-count conversion.

inline int MsToSamples(float ms, double sampleRate)
{
    if (ms <= 0.0f || sampleRate <= 0.0)
        return 0;

    return (int)(((double)ms * 0.001) * sampleRate + 0.5);
}


//==============================================================================
// Style override (applies in BOTH modes per CLAUDE.md): when style == Opto,
// peakMode is forced to RMS and knee to 12 dB regardless of user-set values.

inline PeakMode ApplyStyleOverrideToPeakMode(Style style, PeakMode userValue)
{
    return (style == Style::Opto) ? PeakMode::RMS : userValue;
}

inline float ApplyStyleOverrideToKneeDb(Style style, float userValueDb)
{
    return (style == Style::Opto) ? 12.0f : userValueDb;
}

} // namespace Derivations
} // namespace CenterSpace
