/*
  ==============================================================================

     CSLookAndFeel.cpp

  ==============================================================================
*/

#include "CSLookAndFeel.h"

//==============================================================================

CSLookAndFeel::CSLookAndFeel()
{}

void CSLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height
                                        , float sliderPos, float rotaryStartAngle, float rotaryEndAngle
                                        , juce::Slider &slider)
{
    const float boundsSize = (float)juce::jmin(width, height);
    const float cx         = (float)x + (float)width  * 0.5f;
    const float cy         = (float)y + (float)height * 0.5f;

    const float arcRadius  = boundsSize * 0.42f;
    const float arcThick   = juce::jmax(2.0f, boundsSize * 0.06f);

    const float curAngle   = juce::jmap(sliderPos, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);

    const auto fillColor   = slider.findColour(juce::Slider::rotarySliderFillColourId);

    // Background arc
    juce::Path bgArc;
    bgArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(trackBackground);
    g.strokePath(bgArc, juce::PathStrokeType(arcThick, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Value arc
    juce::Path valueArc;
    valueArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, curAngle, true);
    g.setColour(fillColor);
    g.strokePath(valueArc, juce::PathStrokeType(arcThick, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // "snapAt50": radial tick at centre, brightens within 1.5% of dead centre.
    // Visual cue only, no actual snapping.
    const bool snapAt50 = (bool) slider.getProperties().getWithDefault("snapAt50", false);

    if (snapAt50)
    {
        const float centreAngle = juce::jmap(0.5f, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);
        const bool  atCentre    = std::abs(sliderPos - 0.5f) < 0.015f;

        const auto  thumbColor  = slider.findColour(juce::Slider::thumbColourId);
        g.setColour(atCentre ? thumbColor : thumbColor.withAlpha(0.35f));

        const float r1 = arcRadius - arcThick * 0.5f - 1.0f;
        const float r2 = arcRadius + arcThick * 0.5f + 1.0f;

        const float x1 = cx + std::sin(centreAngle) * r1;
        const float y1 = cy - std::cos(centreAngle) * r1;
        const float x2 = cx + std::sin(centreAngle) * r2;
        const float y2 = cy - std::cos(centreAngle) * r2;

        g.drawLine(x1, y1, x2, y2, 1.5f);
    }
}

void CSLookAndFeel::SetTrackBackground(juce::Colour color)
{
    trackBackground = color;
}
