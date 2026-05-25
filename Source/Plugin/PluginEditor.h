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

    juce::Colour onyx;
    juce::Colour lightSlateGrey;
    juce::Colour magicMint;
    juce::Colour fieryRose;
    juce::Colour orangePeel;

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

    float sliderSize;
    float ratioSliderSize;
    float textBoxW;
    float textBoxH;

    juce::Label inputGainLabel;
    juce::Label sideChainGainLabel;
    juce::Label outputGainLabel;

    juce::Label thresholdLabel;
    juce::Label ratioLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;

    float labelSize;
    float labelW;
    float labelH;

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
