/*
  ==============================================================================

    ABButton.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "ColorPalette.h"
#include "State/ABCompareManager.h"
#include <JuceHeader.h>

//==============================================================================

class ABButton
    : public juce::Button
{
public:
    ABButton(const Palette::Theme &theme, ABCompareManager &abCompare);
    ~ABButton();
    
    void paintButton(juce::Graphics &, bool highlighted, bool down) override;
    
private:
    const Palette::Theme &theme;
    ABCompareManager &abCompare;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ABButton)
};
