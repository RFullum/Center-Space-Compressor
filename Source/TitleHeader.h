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
    
    /// Sets the designColor
    void setDesignColor(Colour& designC);
    
protected:
    Colour backgroundColor;
    Colour textColor;
    Colour designColor;

private:
    Rectangle<int> fullumMusicArea;
    Rectangle<int> centerArea;
    Rectangle<int> spaceArea;
    
    Rectangle<int> block1L;
    Rectangle<int> block4L;
    Rectangle<int> block7L;
    Rectangle<int> block10L;
    Rectangle<int> block13L;
    
    Rectangle<int> block1R;
    Rectangle<int> block4R;
    Rectangle<int> block7R;
    Rectangle<int> block10R;
    Rectangle<int> block13R;
    
    ImageComponent logoComponent;
    
    
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
    Rectangle<int> versionArea;
    Rectangle<int> urlArea;
};



