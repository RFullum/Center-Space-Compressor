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
/**
*/
class CenterDuckComp2AudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    CenterDuckComp2AudioProcessorEditor (CenterDuckComp2AudioProcessor&);
    ~CenterDuckComp2AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    // Colors
    juce::Colour onyx;
    juce::Colour lightSlateGrey;
    juce::Colour magicMint;
    juce::Colour fieryRose;
    juce::Colour orangePeel;

private:
    // Custom look and feel
    OtherLookAndFeel compLookAndFeel;
    OtherLookAndFeel dBLookAndFeel;
    
    BoxLookAndFeel boxLookAndFeel;
    
    // Title Header
    TitleHeader titleHeader;
    TitleFooter titleFooter;
    
    /// Sets up Slider object instances in constructor. sliderInstance is the slider to set up, suffix is textValueSuffix, sliderFillColor is the slider color below the thumb
    void sliderSetup(juce::Slider& sliderInstance, juce::Slider::SliderStyle style, bool showTextBox);

    /// Sets up Label for the Slider instances. Takes the labelInstance and the text for setText
    void sliderLabelSetup(juce::Label& labelInstance, juce::String labelText, juce::Colour& labelColor, float fontSize);


    // Sliders
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

    // Labels
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

    // Combo Box
    juce::ComboBox peakRMSBox;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sideChainGainSliderAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainSliderAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseSliderAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> peakRMSAttachment;
    
    
    // Meters
    VUMeter inLeftMeter;
    VUMeter inCenterMeter;
    VUMeter inRightMeter;
    VUMeter inSideMeter;
    
    //VUMeter sidechainMeter;
    ReduceMeter gainReduceMeter;
    
    VUMeter outLeftMeter;
    VUMeter outCenterMeter;
    VUMeter outRightMeter;
    
    
    CenterDuckComp2AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CenterDuckComp2AudioProcessorEditor)
};
