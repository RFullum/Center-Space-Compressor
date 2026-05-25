/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================

CenterSpaceAudioProcessorEditor::CenterSpaceAudioProcessorEditor(CenterSpaceAudioProcessor &p)
: juce::AudioProcessorEditor(&p)
, onyx          (juce::Colour((juce::uint8)53,  (juce::uint8)59,  (juce::uint8)60,  (juce::uint8)255))
, lightSlateGrey(juce::Colour((juce::uint8)130, (juce::uint8)146, (juce::uint8)152, (juce::uint8)255))
, magicMint     (juce::Colour((juce::uint8)174, (juce::uint8)255, (juce::uint8)216, (juce::uint8)255))
, fieryRose     (juce::Colour((juce::uint8)255, (juce::uint8)104, (juce::uint8)114, (juce::uint8)255))
, orangePeel    (juce::Colour((juce::uint8)252, (juce::uint8)151, (juce::uint8)0,   (juce::uint8)255))
, sliderSize(125.0f)
, ratioSliderSize(175.0f)
, textBoxW(50.0f)
, textBoxH(25.0f)
, labelSize(50.0f)
, labelW(100.0f)
, labelH(25.0f)
, audioProcessor(p)
{
    setSize(1100, 540);

    compLookAndFeel.SetDialColor(fieryRose);
    compLookAndFeel.SetTickColor(onyx);
    compLookAndFeel.SetBackColor(magicMint);
    dBLookAndFeel.SetDialColor  (orangePeel);
    dBLookAndFeel.SetTickColor  (onyx);
    dBLookAndFeel.SetBackColor  (magicMint);

    boxLookAndFeel.SetOutlineColor(magicMint);

    titleHeader.SetBackgroundColor(onyx);
    titleHeader.SetDesignColor    (fieryRose);
    titleHeader.SetTextColor      (orangePeel);
    addAndMakeVisible(titleHeader);

    titleFooter.SetBackgroundColor(onyx);
    titleFooter.SetTextColor      (magicMint);
    addAndMakeVisible(titleFooter);

    getLookAndFeel().setColour(juce::Label::textColourId, magicMint);

    float largeFontSize = 27.0f;
    float smallFontSize = 22.0f;

    SliderSetup(inputGainSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
    SliderLabelSetup(inputGainLabel, "In Gain dB", orangePeel, largeFontSize);
    inputGainSlider.setLookAndFeel(&dBLookAndFeel);

    SliderSetup(sideChainGainSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
    SliderLabelSetup(sideChainGainLabel, "SCh Gain dB", orangePeel, smallFontSize);
    sideChainGainSlider.setLookAndFeel(&dBLookAndFeel);

    SliderSetup(outputGainSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
    SliderLabelSetup(outputGainLabel, "Out Gain dB", orangePeel, largeFontSize);
    outputGainSlider.setLookAndFeel(&dBLookAndFeel);

    SliderSetup(thresholdSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
    SliderLabelSetup(thresholdLabel, "Threshold dB", orangePeel, smallFontSize);
    thresholdSlider.setLookAndFeel(&dBLookAndFeel);

    SliderSetup(ratioSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
    SliderLabelSetup(ratioLabel, "Ratio", fieryRose, smallFontSize);
    ratioSlider.setLookAndFeel(&compLookAndFeel);

    SliderSetup(attackSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
    SliderLabelSetup(attackLabel, "Attack ms", fieryRose, smallFontSize);
    attackSlider.setLookAndFeel(&compLookAndFeel);

    SliderSetup(releaseSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
    SliderLabelSetup(releaseLabel, "Release ms", fieryRose, smallFontSize);
    releaseSlider.setLookAndFeel(&compLookAndFeel);

    peakRMSBox.addItem("Peak", 1);
    peakRMSBox.addItem("RMS",  2);
    peakRMSBox.setJustificationType(juce::Justification::centred);
    peakRMSBox.setSelectedItemIndex(0);
    peakRMSBox.setColour(juce::ComboBox::backgroundColourId, onyx);
    peakRMSBox.setColour(juce::ComboBox::arrowColourId,      juce::Colours::white);
    peakRMSBox.setColour(juce::ComboBox::outlineColourId,    lightSlateGrey);
    peakRMSBox.setLookAndFeel(&boxLookAndFeel);
    addAndMakeVisible(peakRMSBox);

    inputGainSliderAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "inGain",     inputGainSlider);
    sideChainGainSliderAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "sideInGain", sideChainGainSlider);
    outputGainSliderAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "outGain",    outputGainSlider);

    thresholdSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "threshold", thresholdSlider);
    ratioSliderAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "ratio",     ratioSlider);
    attackSliderAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "attack",    attackSlider);
    releaseSliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "release",   releaseSlider);

    peakRMSAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.parameters, "peakRMS", peakRMSBox);

    inLeftMeter.SetColors    (magicMint, fieryRose, lightSlateGrey);
    inCenterMeter.SetColors  (magicMint, fieryRose, lightSlateGrey);
    inRightMeter.SetColors   (magicMint, fieryRose, lightSlateGrey);
    inSideMeter.SetColors    (magicMint, fieryRose, lightSlateGrey);
    gainReduceMeter.SetColors(magicMint, fieryRose, lightSlateGrey);
    outLeftMeter.SetColors   (magicMint, fieryRose, lightSlateGrey);
    outCenterMeter.SetColors (magicMint, fieryRose, lightSlateGrey);
    outRightMeter.SetColors  (magicMint, fieryRose, lightSlateGrey);

    addAndMakeVisible(inLeftMeter);
    addAndMakeVisible(inCenterMeter);
    addAndMakeVisible(inRightMeter);
    addAndMakeVisible(inSideMeter);
    addAndMakeVisible(gainReduceMeter);
    addAndMakeVisible(outLeftMeter);
    addAndMakeVisible(outCenterMeter);
    addAndMakeVisible(outRightMeter);

    juce::Timer::startTimerHz(60);
}

CenterSpaceAudioProcessorEditor::~CenterSpaceAudioProcessorEditor()
{
    juce::Timer::stopTimer();
}

void CenterSpaceAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.fillAll(onyx);
}

void CenterSpaceAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    float flanksSize = 0.25f;

    juce::Rectangle<int> titleHeaderArea = bounds.removeFromTop(121).removeFromBottom(109);
    titleHeader.setBounds(titleHeaderArea);

    juce::Rectangle<int> titleFooterArea = bounds.removeFromBottom(15);
    titleFooter.setBounds(titleFooterArea);

    juce::Rectangle<int> inputArea     = bounds.removeFromLeft    (getLocalBounds().getWidth() * flanksSize);
    juce::Rectangle<int> inMetersArea  = inputArea.removeFromLeft (inputArea.getWidth() * 0.5f).reduced(10, 30);
    juce::Rectangle<int> inControlArea = inputArea;

    float inMeterWidth = inMetersArea.getWidth() * 0.33f;

    juce::Rectangle<int> inMeterLArea = inMetersArea.removeFromLeft(inMeterWidth);
    juce::Rectangle<int> inMeterCArea = inMetersArea.removeFromLeft(inMeterWidth);
    juce::Rectangle<int> inMeterRArea = inMetersArea;

    inLeftMeter.setBounds  (inMeterLArea);
    inCenterMeter.setBounds(inMeterCArea);
    inRightMeter.setBounds (inMeterRArea);

    juce::Rectangle<int> inGainLabelArea  = inControlArea.removeFromTop   (labelH);
    juce::Rectangle<int> inGainSliderArea = inControlArea.removeFromTop   (inControlArea.getHeight() * 0.5f - labelH);
    juce::Rectangle<int> peakRMSBoxArea   = inControlArea.removeFromBottom(labelH * 4.0f);

    inputGainLabel.setBounds (inGainLabelArea);
    inputGainSlider.setBounds(inGainSliderArea);
    peakRMSBox.setBounds     (peakRMSBoxArea.reduced(8, 32));

    juce::Rectangle<int> outputArea     = bounds.removeFromRight    (getLocalBounds().getWidth() * flanksSize);
    juce::Rectangle<int> outMetersArea  = outputArea.removeFromRight(outputArea.getWidth() * 0.5f).reduced(10, 30);
    juce::Rectangle<int> outControlArea = outputArea;

    float outMeterWidth = outMetersArea.getWidth() * 0.33f;

    juce::Rectangle<int> outMeterLArea = outMetersArea.removeFromLeft(outMeterWidth);
    juce::Rectangle<int> outMeterCArea = outMetersArea.removeFromLeft(outMeterWidth);
    juce::Rectangle<int> outMeterRArea = outMetersArea;

    outLeftMeter.setBounds  (outMeterLArea);
    outCenterMeter.setBounds(outMeterCArea);
    outRightMeter.setBounds (outMeterRArea);

    juce::Rectangle<int> outGainLabelArea  = outControlArea.removeFromTop   (labelH);
    juce::Rectangle<int> outGainSliderArea = outControlArea.removeFromTop   (outControlArea.getHeight() * 0.5f - labelH);
    juce::Rectangle<int> outGainSpacerArea = outControlArea.removeFromBottom(labelH * 4.0f);

    outputGainLabel.setBounds (outGainLabelArea);
    outputGainSlider.setBounds(outGainSliderArea);

    juce::Rectangle<int> compressorArea = bounds;

    float compSectionsWidth = compressorArea.getWidth();
    float compHeightDivs    = compressorArea.getHeight() * 0.33f;

    float scOuterWeight       = 0.7f;                  // Weight of control areas of SC section combined 0.0f to 1.0f
    float scControlAreaWeight = scOuterWeight * 0.5f;  // Weight of control areas individually

    juce::Rectangle<int> scControlArea         = compressorArea.removeFromLeft       (compSectionsWidth * scControlAreaWeight);
    juce::Rectangle<int> compControlArea       = compressorArea.removeFromRight      (compSectionsWidth * scControlAreaWeight);
    juce::Rectangle<int> compressorAreaReduced = compressorArea.reduced              (10, 30);
    juce::Rectangle<int> scGainMeterArea       = compressorAreaReduced.removeFromLeft(compressorAreaReduced.getWidth() * 0.5f);
    juce::Rectangle<int> gainReductionArea     = compressorAreaReduced;

    juce::Rectangle<int> scArea      = scControlArea.removeFromTop(compHeightDivs);
    juce::Rectangle<int> scSpaceArea = scControlArea.removeFromTop(compHeightDivs);    // Empty Spacer
    juce::Rectangle<int> threshArea  = scControlArea;

    juce::Rectangle<int> scGainLabelArea = scArea.removeFromTop    (labelH);
    juce::Rectangle<int> threshLabelArea = threshArea.removeFromTop(labelH);

    sideChainGainLabel.setBounds (scGainLabelArea);
    sideChainGainSlider.setBounds(scArea);
    thresholdLabel.setBounds     (threshLabelArea);
    thresholdSlider.setBounds    (threshArea);

    inSideMeter.setBounds    (scGainMeterArea);
    gainReduceMeter.setBounds(gainReductionArea);

    juce::Rectangle<int> ratioArea   = compControlArea.removeFromTop(compHeightDivs).reduced(0, 5);
    juce::Rectangle<int> attackArea  = compControlArea.removeFromTop(compHeightDivs).reduced(0, 5);
    juce::Rectangle<int> releaseArea = compControlArea.reduced      (0, 5);

    juce::Rectangle<int> ratioLabelArea   = ratioArea.removeFromTop  (labelH);
    juce::Rectangle<int> attackLabelArea  = attackArea.removeFromTop (labelH);
    juce::Rectangle<int> releaseLabelArea = releaseArea.removeFromTop(labelH);

    ratioLabel.setBounds   (ratioLabelArea);
    ratioSlider.setBounds  (ratioArea);
    attackLabel.setBounds  (attackLabelArea);
    attackSlider.setBounds (attackArea);
    releaseLabel.setBounds (releaseLabelArea);
    releaseSlider.setBounds(releaseArea);
}

void CenterSpaceAudioProcessorEditor::timerCallback()
{
    float SR = audioProcessor.getSampleRate();

    inLeftMeter.VuMeterLevel    (audioProcessor.inLeftLevel,    SR);
    inCenterMeter.VuMeterLevel  (audioProcessor.inMidLevel,     SR);
    inRightMeter.VuMeterLevel   (audioProcessor.inRightLevel,   SR);
    inSideMeter.VuMeterLevel    (audioProcessor.sideChainLevel, SR);
    gainReduceMeter.VuMeterLevel(audioProcessor.gainReduction,  SR);
    outLeftMeter.VuMeterLevel   (audioProcessor.outLeftLevel,   SR);
    outCenterMeter.VuMeterLevel (audioProcessor.outMidLevel,    SR);
    outRightMeter.VuMeterLevel  (audioProcessor.outRightLevel,  SR);
}

void CenterSpaceAudioProcessorEditor::SliderSetup(juce::Slider &sliderInstance, juce::Slider::SliderStyle style, bool showTextBox)
{
    sliderInstance.setSliderStyle(style);

    // If slider has a textbox, draw it, otherwise, don't
    if (showTextBox)
    {
        sliderInstance.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 15);
        sliderInstance.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour((juce::uint8)0, (juce::uint8)0, (juce::uint8)0, (juce::uint8)0));
        sliderInstance.setColour(juce::Slider::textBoxTextColourId,    juce::Colours::white);
    }
    else
    {
        sliderInstance.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    }

    addAndMakeVisible(sliderInstance);
}

void CenterSpaceAudioProcessorEditor::SliderLabelSetup(juce::Label &labelInstance, juce::String labelText, juce::Colour &labelColor, float fontSize)
{
    labelInstance.setText             (labelText, juce::dontSendNotification);
    labelInstance.setJustificationType(juce::Justification::centred);
    labelInstance.setColour           (juce::Label::textColourId, juce::Colours::white);
    labelInstance.setFont             (juce::Font("futura", fontSize, 0));
    addAndMakeVisible(labelInstance);
}
