/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "VUMeter.h"

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
    Colour babyBlue = Colour((uint8)196, (uint8)253, (uint8)255, (uint8)255);
    Colour darkBlue = Colour((uint8)22, (uint8)10, (uint8)79, (uint8)255);
    Colour brightOrange = Colour((uint8)255, (uint8)89, (uint8)0, (uint8)255);
    Colour brightGreen = Colour((uint8)89, (uint8)255, (uint8)0, (uint8)255);
    Colour darkGreen = Colour((uint8)19, (uint8)145, (uint8)63, (uint8)255);

private:
    /// Sets up Slider object instances in constructor. sliderInstance is the slider to set up, suffix is textValueSuffix, sliderFillColor is the slider color below the thumb
    void sliderSetup(Slider& sliderInstance, String suffix, Colour sliderFillColor);
    
    /// Sets up Label for the Slider instances. Takes the labelInstance and the text for setText
    void sliderLabelSetup(Label& labelInstance, String labelText);
    
    
    // Sliders
    Slider inputGainSlider;
    Slider sideChainGainSlider;
    Slider outputGainSlider;
    
    Slider thresholdSlider;
    Slider ratioSlider;
    Slider attackSlider;
    Slider releaseSlider;
    
    float sliderSize = 125.0f;
    float ratioSliderSize = 175.0f;
    float textBoxW = 50.0f;
    float textBoxH = 25.0f;
    
    // Labels
    Label inputGainLabel;
    Label sideChainGainLabel;
    Label outputGainLabel;
    
    Label thresholdLabel;
    Label ratioLabel;
    Label attackLabel;
    Label releaseLabel;
    
    float labelSize = 50.0f;
    float labelW = 100.0f;
    float labelH = 25.0f;
    
    // Combo Box
    ComboBox peakRMSBox;
    
    // Attachments
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> inputGainSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sideChainGainSliderAttachement;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> outputGainSliderAttachment;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> thresholdSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ratioSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> attackSliderAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> releaseSliderAttachment;
    
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> peakRMSAttachment;
    
    
    // Meters
    
    Rectangle<float> inMeterL;
    //Rectangle<float> inMeterC;
    Rectangle<float> outMeterL;
    Rectangle<float> scMeter;
    Rectangle<float> grMeter;
    
    float meterWidth = 70.0f;
    float meterHeight = 160.0f;
    float meterOffsetCenter = 100.0f;
    
    VUMeter inLeftMeter;
    VUMeter inCenterMeter;
    VUMeter inRightMeter;
    VUMeter inSideMeter;
    
    VUMeter sidechainMeter;
    VUMeter gainReduceMeter;
    
    VUMeter outLeftMeter;
    VUMeter outCenterMeter;
    VUMeter outRightMeter;
    
    
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CenterDuckComp2AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CenterDuckComp2AudioProcessorEditor)
};
