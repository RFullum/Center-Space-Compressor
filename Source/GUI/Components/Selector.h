/*
  ==============================================================================

    Selector.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include "signal/Property.h"
#include "GuiHelpers.h"
#include <JuceHeader.h>

//==============================================================================

class SelectorButton
    : public juce::Button
{
public:
    SelectorButton(juce::StringRef  name
                   , GuiResources  &resources);
    ~SelectorButton() override;
    
    void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
    Property_ST<bool> IsSelected { false };
    
private:
    const juce::String name;
    
    GuiResources &resources;
};

//==============================================================================

class Selector
    : public juce::Component
{
public:
    Selector(GuiResources &resources);
    ~Selector() override;
    
protected:
    GuiResources &resources;
};

//==============================================================================

class StereoSelector
    : public Selector
{
public:
    StereoSelector(GuiResources            &resources
                   , const juce::StringRef  paramID);
    ~StereoSelector() override;
    
    void paint(juce::Graphics &) override;
    void resized() override;
    
    Property_ST<CenterSpace::StereoType> Stereo { CenterSpace::StereoType::LeftRight };
    
private:
    void OnStereoChanged(CenterSpace::StereoType type);
    
    std::unique_ptr<SelectorButton> lrButton;
    std::unique_ptr<SelectorButton> msButton;
    
    juce::RangedAudioParameter                 *parameter = nullptr;
    std::unique_ptr<juce::ParameterAttachment>  attachment;
};
