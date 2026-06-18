/*
  ==============================================================================

    Selector.h
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include "Signal/Property.h"
#include "CenterSpaceHelperes.h"
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
    Selector(GuiResources                      &resources
             , const juce::StringRef            paramID
             , std::unique_ptr<SelectorButton>  left
             , std::unique_ptr<SelectorButton>  right);
    ~Selector() override;
    
    void paint(juce::Graphics &) override;
    void resized() override;

    void SetTooltip(const juce::String &text);

protected:
    GuiResources &resources;
    
    std::unique_ptr<SelectorButton> leftButton;
    std::unique_ptr<SelectorButton> rightButton;
    
    juce::RangedAudioParameter                 *parameter = nullptr;
    std::unique_ptr<juce::ParameterAttachment>  attachment;

    bool syncingFromParam = false;
};

//==============================================================================

class StereoSelector
    : public Selector
{
public:
    StereoSelector(GuiResources            &resources
                   , const juce::StringRef  paramID);
    ~StereoSelector() override;
    
    Property_ST<CenterSpace::StereoType> Stereo { CenterSpace::StereoType::LeftRight };
    
private:
    void OnStereoChanged(CenterSpace::StereoType type);
};

//==============================================================================

class UIModeSelector
    : public Selector
{
public:
    UIModeSelector(GuiResources            &resources
                   , const juce::StringRef  paramID);
    ~UIModeSelector();
    
    Property_ST<CenterSpace::UIModeType> UIMode { CenterSpace::UIModeType::Vibe };
    
private:
    void OnUIModeChanged(CenterSpace::UIModeType type);
};

//==============================================================================

class LookaheadVibeSelector
    : public Selector
{
public:
    LookaheadVibeSelector(GuiResources            &resources
                          , const juce::StringRef  paramID);
    ~LookaheadVibeSelector();
    
    Property_ST<bool> LookaheadOn { false };
    
private:
    void OnLookaheadOnChanged(bool isOn);
};

//==============================================================================

class LookaheadTweakSelector
    : public Selector
{
public:
    LookaheadTweakSelector(GuiResources            &resources
                           , const juce::StringRef  paramID);
    ~LookaheadTweakSelector();
    
    void resized() override;
    
    Property_ST<CenterSpace::LookaheadMsType> LookaheadMS { CenterSpace::LookaheadMsType::Zero };
    
private:
    void OnLookaheadMSChanged(CenterSpace::LookaheadMsType type);
    
    std::unique_ptr<SelectorButton> zeroButton;
    std::unique_ptr<SelectorButton> oneButton;
    std::unique_ptr<SelectorButton> fourButton;
    std::unique_ptr<SelectorButton> tenButton;
};

//==============================================================================

class FeelSelector
    : public Selector
{
public:
    FeelSelector(GuiResources            &resources
                 , const juce::StringRef  paramID);
    ~FeelSelector();
    
    Property_ST<CenterSpace::FeelType> Feel { CenterSpace::FeelType::Clean };
    
private:
    void OnFeelChanged(CenterSpace::FeelType type);
};

//==============================================================================

class DetectionSelector
    : public Selector
{
public:
    DetectionSelector(GuiResources            &resources
                      , const juce::StringRef  paramID);
    ~DetectionSelector();
    
    Property_ST<CenterSpace::DetectionType> Detection { CenterSpace::DetectionType::Peak };
    
private:
    void OnDetectionChanged(CenterSpace::DetectionType type);
};

//==============================================================================

class StyleSelector
    : public Selector
{
public:
    StyleSelector(GuiResources            &resources
                  , const juce::StringRef  paramID);
    ~StyleSelector();
    
    Property_ST<CenterSpace::StyleType> Style { CenterSpace::StyleType::VCA };
    
private:
    void OnStyleChanged(CenterSpace::StyleType type);
};
