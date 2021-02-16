/*
  ==============================================================================

    TitleHeader.h
    Created: 4 Feb 2021 11:47:30am
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class TitleHeader  : public juce::Component
{
public:
    TitleHeader();
    ~TitleHeader() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    /// Sets the background color
    void setBackgroundColor(Colour& bg);
    
    /// Sets the textColor
    void setTextColor(Colour& textC);

private:
    Colour backgroundColor;
    Colour textColor;
    
    Rectangle<int> fullumMusicArea;
    Rectangle<int> centerArea;
    Rectangle<int> duckArea;
    Rectangle<int> compressorArea;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TitleHeader)
};
