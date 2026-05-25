/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "VUMeter.h"
#include "OtherLookAndFeel.h"
#include "TitleHeader.h"


//==============================================================================

class CenterSpaceAudioProcessorEditor
    : public juce::AudioProcessorEditor
    , public juce::Timer
{
public:
    CenterSpaceAudioProcessorEditor(CenterSpaceAudioProcessor &);
    ~CenterSpaceAudioProcessorEditor() override;

    void paint(juce::Graphics &) override;
    void resized() override;
    void timerCallback() override;

    juce::Colour onyx           { (juce::uint8)53,  (juce::uint8)59,  (juce::uint8)60,  (juce::uint8)255 };
    juce::Colour lightSlateGrey { (juce::uint8)130, (juce::uint8)146, (juce::uint8)152, (juce::uint8)255 };
    juce::Colour magicMint      { (juce::uint8)174, (juce::uint8)255, (juce::uint8)216, (juce::uint8)255 };
    juce::Colour fieryRose      { (juce::uint8)255, (juce::uint8)104, (juce::uint8)114, (juce::uint8)255 };
    juce::Colour orangePeel     { (juce::uint8)252, (juce::uint8)151, (juce::uint8)0,   (juce::uint8)255 };

private:
    void SliderSetup(juce::Slider &sliderInstance, juce::Slider::SliderStyle style, bool showTextBox);
    void SliderLabelSetup(juce::Label &labelInstance, juce::String labelText, juce::Colour &labelColor, float fontSize);

    OtherLookAndFeel compLookAndFeel;
    OtherLookAndFeel dBLookAndFeel;
    BoxLookAndFeel   boxLookAndFeel;

    TitleHeader titleHeader;
    TitleFooter titleFooter;

    juce::Slider inputGainSlider;
    juce::Slider sideChainGainSlider;
    juce::Slider outputGainSlider;

    juce::Slider thresholdSlider;
    juce::Slider ratioSlider;
    juce::Slider attackSlider;
    juce::Slider releaseSlider;

    float sliderSize      = 125.0f;
    float ratioSliderSize = 175.0f;
    float textBoxW        = 50.0f;
    float textBoxH        = 25.0f;

    juce::Label inputGainLabel;
    juce::Label sideChainGainLabel;
    juce::Label outputGainLabel;

    juce::Label thresholdLabel;
    juce::Label ratioLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;

    float labelSize = 50.0f;
    float labelW    = 100.0f;
    float labelH    = 25.0f;

    juce::ComboBox peakRMSBox;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sideChainGainSliderAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainSliderAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseSliderAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> peakRMSAttachment;

    VUMeter inLeftMeter;
    VUMeter inCenterMeter;
    VUMeter inRightMeter;
    VUMeter inSideMeter;

    ReduceMeter gainReduceMeter;

    VUMeter outLeftMeter;
    VUMeter outCenterMeter;
    VUMeter outRightMeter;

    CenterSpaceAudioProcessor &audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CenterSpaceAudioProcessorEditor)
};
