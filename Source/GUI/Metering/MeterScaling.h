/*
  ==============================================================================

    MeterScaling.h

    Single source for the dB ranges and tick positions
      - StereoFieldMeter (level range, grid lines)
      - SidechainGainMeter (level range, fill position, peak-hold position)
      - GainReductionMeter (GR range, fill position)
      - MeterScale (level range, label positions)

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <array>


namespace MeterScaling
{
    //==========================================================================
    // Ranges. Adjust here and every consumer follows.

    /// Range used by SC bar, the stereo-field meter axis, and the dB MeterScale
    /// labels. Top = +3 dB (a touch of headroom above 0). Bottom = -36 dB.
    inline constexpr float levelMinDb = -36.0f;
    inline constexpr float levelMaxDb =   3.0f;

    /// Range used by the GR bar. Positive values represent dB-of-reduction.
    /// 0 dB = no reduction (empty bar); 24 dB = full reduction (full bar).
    inline constexpr float grMinDb = 0.0f;
    inline constexpr float grMaxDb = 24.0f;

    //==========================================================================
    // Canonical tick values. Order is top-to-bottom on screen for level ticks
    // (loud at top), bottom-to-top for GR ticks (no-reduction at top).

    inline constexpr std::array<float, 5> levelTicksDb { 0.0f, -6.0f, -12.0f, -24.0f, -36.0f };
    inline constexpr std::array<float, 5> grTicksDb    { 0.0f,  6.0f,  12.0f,  18.0f,  24.0f };

    //==========================================================================
    // Mapping functions. Convention: `top` and `bottom` are pixel coordinates
    // within the consumer's bounds (top < bottom in JUCE's Y-down system).

    /// Generic dB → Y. Higher dB ends up at a smaller Y (visually higher).
    /// Out-of-range dB values clamp to the edges.
    inline float dbToY(float db, float minDb, float maxDb, float top, float bottom) noexcept
    {
        const float clamped = juce::jlimit(minDb, maxDb, db);
        const float t       = (clamped - minDb) / (maxDb - minDb);
        return juce::jmap(t, 0.0f, 1.0f, bottom, top);
    }

    /// Level-meter mapping (range [levelMinDb, levelMaxDb]). Bottom of the
    /// component is levelMinDb, top is levelMaxDb.
    inline float levelDbToY(float db, float top, float bottom) noexcept
    {
        return dbToY(db, levelMinDb, levelMaxDb, top, bottom);
    }

    /// GR-meter mapping (range [grMinDb, grMaxDb]). Inverted convention:
    /// 0 dB GR sits at the TOP of the component, max GR sits at the bottom —
    /// the bar fills downward as reduction increases.
    inline float grDbToY(float db, float top, float bottom) noexcept
    {
        const float clamped = juce::jlimit(grMinDb, grMaxDb, db);
        const float t       = (clamped - grMinDb) / (grMaxDb - grMinDb);
        return juce::jmap(t, 0.0f, 1.0f, top, bottom);
    }

}   // namespace MeterScaling
