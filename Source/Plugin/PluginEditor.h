/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "GuiResources.h"
#include "GUI/GuiHelpers.h"
#include "GUI/Format.h"

#include <JuceHeader.h>


class CenterSpaceAudioProcessor;
class TitleHeader;
class TitleFooter;
class Selector;
class FullumLookAndFeel;

class VibeDetection;
class VibeDynamics;
class TweakDetection;
class TweakDynamics;

class Metering;

//==============================================================================

class CenterSpaceAudioProcessorEditor
    : public  juce::AudioProcessorEditor
    , private juce::AudioProcessorValueTreeState::Listener
{
public:
    CenterSpaceAudioProcessorEditor(CenterSpaceAudioProcessor &);
    ~CenterSpaceAudioProcessorEditor() override;

    void paint(juce::Graphics &) override;
    void resized() override;

private:
    void parameterChanged(const juce::String &paramId, float newValue) override;

    void Update();
    void UpdateTooltipWindow();  
    
    std::unique_ptr<FullumLookAndFeel>   csLAndF;
    GuiResources                         resources;
    std::unique_ptr<juce::TooltipWindow> tooltipWindow;
    
    std::unique_ptr<TitleHeader> titleHeader;
    std::unique_ptr<TitleFooter> titleFooter;
    
    std::unique_ptr<Selector> inStereoSelector;
    std::unique_ptr<Selector> outStereoSelector;
    juce::Label               inStereoLabel;
    juce::Label               outStereoLabel;
    
    std::unique_ptr<VibeDetection>  vibeDetection;
    std::unique_ptr<VibeDynamics>   vibeDynamics;
    std::unique_ptr<TweakDetection> tweakDetection;
    std::unique_ptr<TweakDynamics>  tweakDynamics;
    
    std::unique_ptr<Metering> metering;

    juce::Slider inGainSlider;
    juce::Slider outGainSlider;
    
    juce::Label inGainLabel;
    juce::Label outGainLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inGainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outGainSliderAttachment;

    CenterSpaceAudioProcessor &audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CenterSpaceAudioProcessorEditor)
};
