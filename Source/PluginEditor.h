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
class CenterDuckComp2AudioProcessorEditor  : public juce::AudioProcessorEditor, public Timer
{
public:
    CenterDuckComp2AudioProcessorEditor (CenterDuckComp2AudioProcessor&);
    ~CenterDuckComp2AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    // Colors
    Colour onyx;
    Colour lightSlateGrey;
    Colour magicMint;
    Colour fieryRose;
    Colour orangePeel;

private:
    // Custom look and feel
    OtherLookAndFeel compLookAndFeel;
    OtherLookAndFeel dBLookAndFeel;
    
    BoxLookAndFeel boxLookAndFeel;
    
    // Title Header
    TitleHeader titleHeader;
    TitleFooter titleFooter;
    
    /// Sets up Slider object instances in constructor. sliderInstance is the slider to set up, suffix is textValueSuffix, sliderFillColor is the slider color below the thumb
    void sliderSetup(Slider& sliderInstance, Slider::SliderStyle style, bool showTextBox);
    
    /// Sets up Label for the Slider instances. Takes the labelInstance and the text for setText
    void sliderLabelSetup(Label& labelInstance, String labelText, Colour& labelColor, float fontSize);
    
    
    // Sliders
    Slider inputGainSlider;
    Slider sideChainGainSlider;
    Slider outputGainSlider;
    
    Slider thresholdSlider;
    //Slider thresholdMeterSlider;
    Slider ratioSlider;
    Slider attackSlider;
    Slider releaseSlider;
    
    float sliderSize;
    float ratioSliderSize;
    float textBoxW;
    float textBoxH;
    
    // Labels
    Label inputGainLabel;
    Label sideChainGainLabel;
    Label outputGainLabel;
    
    Label thresholdLabel;
    Label ratioLabel;
    Label attackLabel;
    Label releaseLabel;
    
    float labelSize;
    float labelW;
    float labelH;
    
    // Combo Box
    ComboBox peakRMSBox;
    
    // Attachments
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> inputGainSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sideChainGainSliderAttachement;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> outputGainSliderAttachment;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> thresholdSliderAttachment;
    //std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> thresholdSliderMeterAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ratioSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> attackSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> releaseSliderAttachment;
    
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> peakRMSAttachment;
    
    
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
