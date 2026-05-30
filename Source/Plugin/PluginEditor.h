/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include "VUMeter.h"
//#include "OtherLookAndFeel.h"
//#include "CSLookAndFeel.h"
//#include "TweakModeComponent.h"
//#include "VibeModeComponent.h"
#include "GuiResources.h"
#include "GuiHelpers.h"
#include "signal/Property.h"

class CenterSpaceAudioProcessor;
class TitleHeader;
class TitleFooter;
class StereoSelector;
class CSLookAndFeel;

class VibeDetection;
class VibeDynamics;
class TweakDetection;
class TweakDynamics;

//==============================================================================

class CenterSpaceAudioProcessorEditor
    : public  juce::AudioProcessorEditor
    , private juce::Timer
    , private juce::AudioProcessorValueTreeState::Listener
{
public:
    CenterSpaceAudioProcessorEditor(CenterSpaceAudioProcessor &);
    ~CenterSpaceAudioProcessorEditor() override;

    void paint(juce::Graphics &) override;
    void resized() override;

//    juce::Colour onyx           { (juce::uint8)53,  (juce::uint8)59,  (juce::uint8)60,  (juce::uint8)255 };
//    juce::Colour lightSlateGrey { (juce::uint8)130, (juce::uint8)146, (juce::uint8)152, (juce::uint8)255 };
//    juce::Colour magicMint      { (juce::uint8)174, (juce::uint8)255, (juce::uint8)216, (juce::uint8)255 };
//    juce::Colour fieryRose      { (juce::uint8)255, (juce::uint8)104, (juce::uint8)114, (juce::uint8)255 };
//    juce::Colour orangePeel     { (juce::uint8)252, (juce::uint8)151, (juce::uint8)0,   (juce::uint8)255 };

private:
    void timerCallback() override;
    void parameterChanged(const juce::String &paramId, float newValue) override;
//    void SliderSetup(juce::Slider &sliderInstance, juce::Slider::SliderStyle style, bool showTextBox);
//    void SliderLabelSetup(juce::Label &labelInstance, juce::String labelText, juce::Colour &labelColor, float fontSize);
//    void ComboSetup(juce::ComboBox &box, const juce::StringArray &items);

    void ApplyUiModeVisibility();
    void ApplyStyleVisibility();
    
    std::unique_ptr<CSLookAndFeel> csLAndF;
    GuiResources                   resources;
    
    std::unique_ptr<TitleHeader> titleHeader;
    std::unique_ptr<TitleFooter> titleFooter;
    
    std::unique_ptr<StereoSelector> inStereoSelector;
    std::unique_ptr<StereoSelector> outStereoSelector;
    juce::Label                     inStereoLabel;
    juce::Label                     outStereoLabel;
    
    std::unique_ptr<VibeDetection>  vibeDetection;
    std::unique_ptr<VibeDynamics>   vibeDynamics;
    std::unique_ptr<TweakDetection> tweakDetection;
    std::unique_ptr<TweakDynamics>  tweakDynamics;

//    OtherLookAndFeel compLookAndFeel;
//    OtherLookAndFeel dBLookAndFeel;
//    BoxLookAndFeel   boxLookAndFeel;

//    TitleHeader titleHeader;
//    TitleFooter titleFooter;
//
    juce::Slider inGainSlider;
//    juce::Slider sideChainGainSlider;
    juce::Slider outGainSlider;
//
//    juce::Slider thresholdSlider;
//    juce::Slider ratioSlider;
//    juce::Slider attackSlider;
//    juce::Slider releaseSlider;
//
//    float sliderSize      = 125.0f;
//    float ratioSliderSize = 175.0f;
//    float textBoxW        = 50.0f;
//    float textBoxH        = 25.0f;
//
    juce::Label inGainLabel;
//    juce::Label sideChainGainLabel;
    juce::Label outGainLabel;
//
//    juce::Label thresholdLabel;
//    juce::Label ratioLabel;
//    juce::Label attackLabel;
//    juce::Label releaseLabel;
//
//    float labelSize = 50.0f;
//    float labelW    = 100.0f;
//    float labelH    = 25.0f;
//
//    juce::ComboBox peakRMSBox;
//
//    juce::ComboBox uiModeBox;
//    juce::ComboBox inputTypeBox;
//    juce::ComboBox outputTypeBox;
//    juce::Label    uiModeLabel;
//    juce::Label    inputTypeLabel;
//    juce::Label    outputTypeLabel;
//
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inGainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outGainSliderAttachment;
//
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdSliderAttachment;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioSliderAttachment;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackSliderAttachment;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseSliderAttachment;
//
//    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> peakRMSAttachment;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> uiModeAttachment;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> inputTypeAttachment;
//    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> outputTypeAttachment;
//
//    TweakModeComponent tweakModeComp;
//    VibeModeComponent  vibeModeComp;
//
//    VUMeter inLeftMeter;
//    VUMeter inCenterMeter;
//    VUMeter inRightMeter;
//    VUMeter inSideMeter;
//
//    ReduceMeter gainReduceMeter;
//
//    VUMeter outLeftMeter;
//    VUMeter outCenterMeter;
//    VUMeter outRightMeter;

    CenterSpaceAudioProcessor &audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CenterSpaceAudioProcessorEditor)
};
