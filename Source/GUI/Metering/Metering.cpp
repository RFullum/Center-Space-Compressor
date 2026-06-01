/*
  ==============================================================================

    Metering.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "Metering.h"

#include "SidechainGainMeter.h"
#include "GainReductionMeter.h"
#include "StereoFieldMeter.h"
#include "MeterScale.h"

//==============================================================================

Metering::Metering(GuiResources &res)
: resources(res)
, sidechainGainMeter(std::make_unique<SidechainGainMeter>(res))
, gainReductionMeter(std::make_unique<GainReductionMeter>(res))
, stereoFieldMeter  (std::make_unique<StereoFieldMeter>  (res))
, meterScale  (std::make_unique<MeterScale>(res, MeterScale::Type::Level, juce::Justification::centred))
, grMeterScale(std::make_unique<MeterScale>(res, MeterScale::Type::GR,    juce::Justification::centredLeft))
{
    setOpaque(false);

    addAndMakeVisible(sidechainGainMeter.get());
    addAndMakeVisible(gainReductionMeter.get());
    addAndMakeVisible(stereoFieldMeter  .get());
    addAndMakeVisible(meterScale        .get());
    addAndMakeVisible(grMeterScale      .get());

    startTimerHz(60);
}

Metering::~Metering()
{
    stopTimer();
}

void Metering::paint(juce::Graphics &g)
{
    g.setColour(resources.theme.textSecondary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 12.0f
                                           , juce::Font::plain)));
    g.drawText("L", lArea, juce::Justification::bottomLeft);
    g.drawText("C", cArea, juce::Justification::centredBottom);
    g.drawText("R", rArea, juce::Justification::bottomRight);
    
    g.drawText("GR", grArea, juce::Justification::centredBottom);
    g.drawText("SC", scArea, juce::Justification::centredBottom);
    
    g.drawText("IN",  inTextArea,  juce::Justification::centred);
    g.drawText("OUT", outTextArea, juce::Justification::centred);
    
    g.setColour           (resources.theme.textSecondary);
    g.fillRoundedRectangle(inArea.withSizeKeepingCentre(colorBlockSize, colorBlockSize).toFloat(), 2.0f);
    g.setColour           (resources.theme.primaryAccent);
    g.fillRoundedRectangle(outArea.withSizeKeepingCentre(colorBlockSize, colorBlockSize).toFloat(), 2.0f);
}

void Metering::resized()
{
    static constexpr int yMargin = 50;
    static constexpr int barW    = 32;
    static constexpr int gap     = 4;
    static constexpr int letterW = 10;
    
    auto bounds = getLocalBounds();
    auto topRow = bounds.removeFromTop(yMargin);
    bounds.removeFromBottom(yMargin);
    bounds.reduce(10, 0);
    gainReductionMeter->setBounds(bounds.removeFromLeft(barW));

    bounds.removeFromLeft(gap);
    grMeterScale->setBounds(bounds.removeFromLeft(barW).expanded(0, MeterScale::labelPad));

    bounds.removeFromLeft(gap);
    sidechainGainMeter->setBounds(bounds.removeFromLeft(barW));

    bounds.removeFromLeft(gap);
    meterScale->setBounds(bounds.removeFromLeft(barW).expanded(0, MeterScale::labelPad));

    bounds.removeFromLeft(gap);
    stereoFieldMeter->setBounds(bounds);
    
    auto letterRow = topRow.removeFromBottom(topRow.proportionOfHeight(0.5f));
    const int letterY = letterRow.getY();
    const int letterH = letterRow.getHeight();
    lArea = juce::Rectangle<int>(stereoFieldMeter->getX()
                                 , letterY
                                 , letterW
                                 , letterH);
    rArea = juce::Rectangle<int>(stereoFieldMeter->getRight() - letterW
                                 , letterY
                                 , letterW
                                 , letterH);
    cArea = juce::Rectangle<int>(stereoFieldMeter->getBounds().getCentreX() - (letterW / 2)
                                 , letterY
                                 , letterW
                                 , letterH);
    
    grArea = juce::Rectangle<int>(gainReductionMeter->getX(), letterY, gainReductionMeter->getWidth(), letterH);
    scArea = juce::Rectangle<int>(sidechainGainMeter->getX(), letterY, sidechainGainMeter->getWidth(), letterH);
    
    topRow.removeFromRight(8);
    outTextArea = topRow.removeFromRight(34);
    outArea     = topRow.removeFromRight(colorBlockSize);
    topRow.removeFromRight(10);
    inTextArea  = topRow.removeFromRight(20);
    inArea      = topRow.removeFromRight(colorBlockSize);
}

void Metering::timerCallback()
{
    sidechainGainMeter->Update();
    gainReductionMeter->Update();
    stereoFieldMeter  ->Update();
}
