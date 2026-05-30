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
    
    void paint(juce::Graphics &) override;
    
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
    
    void resized() override;
    
    Property_ST<CenterSpace::StereoType> Stereo { CenterSpace::StereoType::LeftRight };
    
private:
    void OnStereoChanged(CenterSpace::StereoType type);
    
    std::unique_ptr<SelectorButton> lrButton;
    std::unique_ptr<SelectorButton> msButton;
    
    juce::RangedAudioParameter                 *parameter = nullptr;
    std::unique_ptr<juce::ParameterAttachment>  attachment;
};

//==============================================================================

class UIModeSelector
    : public Selector
{
public:
    UIModeSelector(GuiResources            &resources
                   , const juce::StringRef  paramID);
    ~UIModeSelector();
    
    void resized() override;
    
    Property_ST<CenterSpace::UIModeType> UIMode { CenterSpace::UIModeType::Vibe };
    
private:
    void OnUIModeChanged(CenterSpace::UIModeType type);
    
    std::unique_ptr<SelectorButton> vibeButton;
    std::unique_ptr<SelectorButton> tweakButton;
    
    juce::RangedAudioParameter                 *parameter = nullptr;
    std::unique_ptr<juce::ParameterAttachment>  attachment;
};

//==============================================================================

class LookaheadVibeSelector
    : public Selector
{
public:
    LookaheadVibeSelector(GuiResources            &resources
                          , const juce::StringRef  paramID);
    ~LookaheadVibeSelector();
    
    void resized() override;
    
    Property_ST<bool> LookaheadOn { false };
    
private:
    void OnLookaheadOnChanged(bool isOn);
    
    std::unique_ptr<SelectorButton> offButton;
    std::unique_ptr<SelectorButton> onButton;
    
    juce::RangedAudioParameter                 *parameter = nullptr;
    std::unique_ptr<juce::ParameterAttachment>  attachment;
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
    
    juce::RangedAudioParameter                 *parameter = nullptr;
    std::unique_ptr<juce::ParameterAttachment>  attachment;
};

//==============================================================================

class FeelSelector
    : public Selector
{
public:
    FeelSelector(GuiResources            &resources
                 , const juce::StringRef  paramID);
    ~FeelSelector();
    
    void resized() override;
    
    Property_ST<CenterSpace::FeelType> Feel { CenterSpace::FeelType::Clean };
    
private:
    void OnFeelChanged(CenterSpace::FeelType type);
    
    std::unique_ptr<SelectorButton> cleanButton;
    std::unique_ptr<SelectorButton> smoothButton;
    
    juce::RangedAudioParameter                 *parameter = nullptr;
    std::unique_ptr<juce::ParameterAttachment>  attachment;
};

//==============================================================================

class DetectionSelector
    : public Selector
{
public:
    DetectionSelector(GuiResources            &resources
                      , const juce::StringRef  paramID);
    ~DetectionSelector();
    
    void resized() override;
    
    Property_ST<CenterSpace::DetectionType> Detection { CenterSpace::DetectionType::Peak };
    
private:
    void OnDetectionChanged(CenterSpace::DetectionType type);
    
    std::unique_ptr<SelectorButton> peakButton;
    std::unique_ptr<SelectorButton> rmsButton;
    
    juce::RangedAudioParameter                 *parameter = nullptr;
    std::unique_ptr<juce::ParameterAttachment>  attachment;
};

//==============================================================================

class StyleSelector
    : public Selector
{
public:
    StyleSelector(GuiResources            &resources
                  , const juce::StringRef  paramID);
    ~StyleSelector();
    
    void resized() override;
    
    Property_ST<CenterSpace::StyleType> Style { CenterSpace::StyleType::VCA };
    
private:
    void OnStyleChanged(CenterSpace::StyleType type);
    
    std::unique_ptr<SelectorButton> vcaButton;
    std::unique_ptr<SelectorButton> optoButton;
    
    juce::RangedAudioParameter                 *parameter = nullptr;
    std::unique_ptr<juce::ParameterAttachment>  attachment;
};
