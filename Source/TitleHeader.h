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
    void setBackgroundColor(juce::Colour& bg);
    
    /// Sets the textColor
    void setTextColor(juce::Colour& textC);
    
    /// Sets the designColor
    void setDesignColor(juce::Colour& designC);
    
protected:
    juce::Colour backgroundColor;
    juce::Colour textColor;
    juce::Colour designColor;

private:
    juce::Rectangle<int> fullumMusicArea;
    juce::Rectangle<int> centerArea;
    juce::Rectangle<int> spaceArea;
    
    juce::Rectangle<int> block1L;
    juce::Rectangle<int> block4L;
    juce::Rectangle<int> block7L;
    juce::Rectangle<int> block10L;
    juce::Rectangle<int> block13L;
    
    juce::Rectangle<int> block1R;
    juce::Rectangle<int> block4R;
    juce::Rectangle<int> block7R;
    juce::Rectangle<int> block10R;
    juce::Rectangle<int> block13R;
    
    juce::ImageComponent logoComponent;
    
    
    //Rectangle<int> duckArea;
    //Rectangle<int> compressorArea;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TitleHeader)
};


//==============================================================================
//==============================================================================
//==============================================================================

class TitleFooter : public TitleHeader
{
public:
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    juce::Rectangle<int> versionArea;
    juce::Rectangle<int> urlArea;
};



