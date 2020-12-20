/*
  ==============================================================================

    Background.h
    Created: 20 Dec 2020 7:26:12pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Background  : public juce::Component
{
public:
    Background();
    ~Background() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Background)
};
