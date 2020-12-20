/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CenterDuckComp2AudioProcessorEditor::CenterDuckComp2AudioProcessorEditor (CenterDuckComp2AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //
    // INITIAL CONSTRUCTOR SETUP
    //
    setSize (1000, 400);
    
    inLeftMeter.setSampleRate   (audioProcessor.getSampleRate());
    inCenterMeter.setSampleRate (audioProcessor.getSampleRate());
    inRightMeter.setSampleRate  (audioProcessor.getSampleRate());
    inSideMeter.setSampleRate   (audioProcessor.getSampleRate());
    
    sidechainMeter.setSampleRate  (audioProcessor.getSampleRate());
    gainReduceMeter.setSampleRate (audioProcessor.getSampleRate());
    
    outLeftMeter.setSampleRate   (audioProcessor.getSampleRate());
    outCenterMeter.setSampleRate (audioProcessor.getSampleRate());
    outRightMeter.setSampleRate  (audioProcessor.getSampleRate());
    
    
    //
    //=== SLIDERS AND LABELS SETUP ===
    //
    
    //=== Look and Feel (global) ===
    getLookAndFeel().setColour(Slider::textBoxTextColourId, textColor);
    getLookAndFeel().setColour(Slider::thumbColourId, buttonColor);
    getLookAndFeel().setColour(Slider::rotarySliderOutlineColourId, sliderTrack);
    getLookAndFeel().setColour(Label::textColourId, textColor);
    
    
    //=== Gain Sliders ===
    
    // Input Gain
    sliderSetup(inputGainSlider, " dB", dBSliderColor);
    sliderLabelSetup(inputGainLabel, "In Gain dB");
    
    // SideChain Input Gain
    sliderSetup(sideChainGainSlider, " dB", dBSliderColor);
    sliderLabelSetup(sideChainGainLabel, "SCh Gain dB");
    
    // Output Gain
    sliderSetup(outputGainSlider, " dB", dBSliderColor);
    sliderLabelSetup(outputGainLabel, "Out Gain dB");
    
    
    //=== Compressor Sliders ===
    
    // Threshold
    sliderSetup(thresholdSlider, " dB", compSliderColor);
    sliderLabelSetup(thresholdLabel, "Threshold");
    
    // Ratio
    sliderSetup(ratioSlider, ":1", compSliderColor);
    sliderLabelSetup(ratioLabel, "Ratio");
    
    // Attack
    sliderSetup(attackSlider, " ms", compSliderColor);
    sliderLabelSetup(attackLabel, "Attack ms");
    
    // Release
    sliderSetup(releaseSlider, " ms", compSliderColor);
    sliderLabelSetup(releaseLabel, "Release ms");
    
    //=== Combo Box ===
    peakRMSBox.addItem("Peak", 1);
    peakRMSBox.addItem("RMS", 2);
    peakRMSBox.setJustificationType(Justification::centred);
    peakRMSBox.setSelectedItemIndex(0);
    addAndMakeVisible(peakRMSBox);
    
    //
    //=== ATTACHMENTS ===
    //
    
    // Gain Sliders
    inputGainSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "inGain", inputGainSlider);
    sideChainGainSliderAttachement = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "sideInGain", sideChainGainSlider);
    outputGainSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "outGain", outputGainSlider);
    
    // Compressor Sliders
    thresholdSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "threshold", thresholdSlider);
    ratioSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "ratio", ratioSlider);
    attackSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "attack", attackSlider);
    releaseSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "release", releaseSlider);
    
    // Combo Boxes
    peakRMSAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.parameters, "peakRMS", peakRMSBox);
    
    //
    //=== METERING ===
    //
    
    addAndMakeVisible(inLeftMeter);
    addAndMakeVisible(inCenterMeter);
    addAndMakeVisible(inRightMeter);
    addAndMakeVisible(inSideMeter);
    
    addAndMakeVisible(sidechainMeter);
    addAndMakeVisible(gainReduceMeter);
    
    addAndMakeVisible(outLeftMeter);
    addAndMakeVisible(outCenterMeter);
    addAndMakeVisible(outRightMeter);
    
    //
    //=== TIMER ===
    //
    Timer::startTimerHz(60);

}

CenterDuckComp2AudioProcessorEditor::~CenterDuckComp2AudioProcessorEditor()
{
    Timer::stopTimer();
}

//==============================================================================
void CenterDuckComp2AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(backgroundColor);
    
    g.setColour(textColor);

}

/// Timer Callback for drawing meters
void CenterDuckComp2AudioProcessorEditor::timerCallback()
{
    inLeftMeter.meterProcess    ( audioProcessor.inLeftLevel, audioProcessor.getSampleRate() );
    inCenterMeter.meterProcess  ( audioProcessor.inMidLevel, audioProcessor.getSampleRate() );
    inRightMeter.meterProcess   ( audioProcessor.inRightLevel, audioProcessor.getSampleRate() );
    inSideMeter.meterProcess    ( audioProcessor.inSideLevel, audioProcessor.getSampleRate() );
    
    sidechainMeter.meterProcess  (audioProcessor.sideChainLevel, audioProcessor.getSampleRate() );
    gainReduceMeter.meterProcess (audioProcessor.gainReduction, audioProcessor.getSampleRate() );
    
    outLeftMeter.meterProcess   (audioProcessor.outLeftLevel, audioProcessor.getSampleRate() );
    outCenterMeter.meterProcess (audioProcessor.outMidLevel, audioProcessor.getSampleRate() );
    outRightMeter.meterProcess  (audioProcessor.outRightLevel, audioProcessor.getSampleRate() );
}

void CenterDuckComp2AudioProcessorEditor::resized()
{
    // Sliders
    inputGainSlider.setBounds( ( getWidth() * (1.0f / 8.0f) ) - (sliderSize * 0.5f), (getHeight() * 0.5f) - (sliderSize * 0.5f), sliderSize, sliderSize );
    inputGainLabel.setBounds ( ( getWidth() * (1.0f / 8.0f) ) - (labelW * 0.5f), (getHeight() * 0.5f) - (labelH * 0.5f) - (sliderSize * 0.5f), labelW, labelH );
    
    sideChainGainSlider.setBounds( (getWidth() * (3.0f / 8.0f) ) - (sliderSize * 0.5f), (getHeight() * 0.25f ) - (sliderSize * 0.5f), sliderSize, sliderSize );
    sideChainGainLabel.setBounds( (getWidth() * (3.0f / 8.0f) ) - (labelW * 0.5f), (getHeight() * 0.25f ) - (labelH * 0.5f) - (sliderSize * 0.5f), labelW, labelH );
    
    outputGainSlider.setBounds( (getWidth() * (7.0f / 8.0f) ) - (sliderSize * 0.5f), (getHeight() * 0.5f) - (sliderSize * 0.5f), sliderSize, sliderSize );
    outputGainLabel.setBounds( (getWidth() * (7.0f / 8.0f) ) - (labelW * 0.5f), (getHeight() * 0.5f) - (labelH * 0.5f) - (sliderSize * 0.5f), labelW, labelH );
    
    thresholdSlider.setBounds( (getWidth() * (3.0f / 8.0f) ) - (sliderSize * 0.5f), (getHeight() * 0.75f ) - (sliderSize * 0.5f), sliderSize, sliderSize );
    thresholdLabel.setBounds( (getWidth() * (3.0f / 8.0f) ) - (labelW * 0.5f), (getHeight() * 0.75f) - (labelH * 0.5f) - (sliderSize * 0.5f), labelW, labelH );
    
    ratioSlider.setBounds( (getWidth() * (4.0f / 8.0f) ) - (ratioSliderSize * 0.5f), (getHeight() * 0.5f) - (ratioSliderSize * 0.5f), ratioSliderSize, ratioSliderSize );
    ratioLabel.setBounds( (getWidth() * (4.0f / 8.0f) ) - (labelW * 0.5f), (getHeight() * 0.5f) - (labelH * 0.5f) - (ratioSliderSize * 0.5f), labelW, labelH );
    
    attackSlider.setBounds( (getWidth() * (5.0f / 8.0f) ) - (sliderSize * 0.5f), (getHeight() * 0.25f) - (sliderSize * 0.5f), sliderSize, sliderSize );
    attackLabel.setBounds( (getWidth() * (5.0f / 8.0f) ) - (labelW * 0.5f), (getHeight() * 0.25f) - (labelH * 0.5f) - (sliderSize * 0.5f), labelW, labelH );
    
    releaseSlider.setBounds( (getWidth() * (5.0f / 8.0f) ) - (sliderSize * 0.5f), (getHeight() * 0.75f) - (sliderSize * 0.5f), sliderSize, sliderSize );
    releaseLabel.setBounds( (getWidth() * (5.0f / 8.0f) ) - (labelW * 0.5f), (getHeight() * 0.75f) - (labelH * 0.5f) - (sliderSize * 0.5f), labelW, labelH );
    
    // Combo Box
    peakRMSBox.setBounds( getWidth() * (6.0f / 8.0f), getHeight() * 0.5f, 100, 25);
    
    // Meter Vals
    inLeftMeter.setBounds   ( getWidth() * (1.0f / 10.0f), getHeight() / 2.0f, inLeftMeter.getMeterWidth(), inLeftMeter.getMeterHeight() );
    inCenterMeter.setBounds ( getWidth() * (2.0f / 10.0f), getHeight() / 2.0f, inCenterMeter.getMeterWidth(), inCenterMeter.getMeterHeight() );
    inRightMeter.setBounds  ( getWidth() * (3.0f / 10.0f), getHeight() / 2.0f, inRightMeter.getMeterWidth(), inRightMeter.getMeterHeight() );
    inSideMeter.setBounds   ( getWidth() * (4.0f / 10.0f), getHeight() / 2.0f, inSideMeter.getMeterWidth(), inSideMeter.getMeterHeight() );
    
    sidechainMeter.setBounds  ( getWidth() * (5.0f / 10.0f), getHeight() / 2.0f, sidechainMeter.getMeterWidth(), sidechainMeter.getMeterHeight() );
    gainReduceMeter.setBounds ( getWidth() * (6.0f / 10.0f), getHeight() / 2.0f, gainReduceMeter.getMeterWidth(), gainReduceMeter.getMeterHeight() );
    
    outLeftMeter.setBounds   ( getWidth() * (7.0f / 10.0f), getHeight() / 2.0f, outLeftMeter.getMeterWidth(), outLeftMeter.getMeterHeight() );
    outCenterMeter.setBounds ( getWidth() * (8.0f / 10.0f), getHeight() / 2.0f, outCenterMeter.getMeterWidth(), outCenterMeter.getMeterHeight() );
    outRightMeter.setBounds  ( getWidth() * (9.0f / 10.0f), getHeight() / 2.0f, outRightMeter.getMeterWidth(), outRightMeter.getMeterHeight() );
}


/// Sets up Slider object instances in constructor. sliderInstance is the slider to set up, suffix is textValueSuffix, sliderFillColor is the slider color below the thumb
void CenterDuckComp2AudioProcessorEditor::sliderSetup(Slider& sliderInstance, String suffix, Colour sliderFillColor)
{
    sliderInstance.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    sliderInstance.setTextBoxStyle(Slider::TextBoxBelow, false, textBoxW, textBoxH);
    sliderInstance.setTextValueSuffix(suffix);
    sliderInstance.setColour(Slider::rotarySliderFillColourId, sliderFillColor);
    addAndMakeVisible(sliderInstance);
}

/// Sets up Label for the Slider instances. Takes the labelInstance and the text for setText
void CenterDuckComp2AudioProcessorEditor::sliderLabelSetup(Label& labelInstance, String labelText)
{
    labelInstance.setText(labelText, dontSendNotification);
    labelInstance.setJustificationType(Justification::centred);
    addAndMakeVisible(labelInstance);
}
