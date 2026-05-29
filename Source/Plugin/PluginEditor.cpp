/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "TitleHeader.h"
#include "TitleFooter.h"

//==============================================================================

namespace
{

    GuiResources MakeResources()
    {
        return GuiResources
        {
            .theme = Palette::DefaultTheme
        };
    }

}   // namespace

//==============================================================================

CenterSpaceAudioProcessorEditor::CenterSpaceAudioProcessorEditor(CenterSpaceAudioProcessor &p)
: juce::AudioProcessorEditor(&p)
, resources(MakeResources())
, titleHeader(std::make_unique<TitleHeader>(resources))
, titleFooter(std::make_unique<TitleFooter>(resources))
//, tweakModeComp(p.parameters, dBLookAndFeel, boxLookAndFeel)
//, vibeModeComp (p.parameters, compLookAndFeel, boxLookAndFeel)
, audioProcessor(p)
{
    setSize(1280, 720);
    
    addAndMakeVisible(titleHeader.get());
    addAndMakeVisible(titleFooter.get());

//    compLookAndFeel.SetDialColor(fieryRose);
//    compLookAndFeel.SetTickColor(onyx);
//    compLookAndFeel.SetBackColor(magicMint);
//    dBLookAndFeel.SetDialColor  (orangePeel);
//    dBLookAndFeel.SetTickColor  (onyx);
//    dBLookAndFeel.SetBackColor  (magicMint);
//
//    boxLookAndFeel.SetOutlineColor(magicMint);
//
//    titleHeader.SetBackgroundColor(onyx);
//    titleHeader.SetDesignColor    (fieryRose);
//    titleHeader.SetTextColor      (orangePeel);
//    addAndMakeVisible(titleHeader);
//
//    titleFooter.SetBackgroundColor(onyx);
//    titleFooter.SetTextColor      (magicMint);
//    addAndMakeVisible(titleFooter);
//
//    getLookAndFeel().setColour(juce::Label::textColourId, magicMint);
//
//    float largeFontSize = 27.0f;
//    float smallFontSize = 22.0f;

//    SliderSetup(inputGainSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
//    SliderLabelSetup(inputGainLabel, "In Gain dB", orangePeel, largeFontSize);
//    inputGainSlider.setLookAndFeel(&dBLookAndFeel);
//
//    SliderSetup(sideChainGainSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
//    SliderLabelSetup(sideChainGainLabel, "SCh Gain dB", orangePeel, smallFontSize);
//    sideChainGainSlider.setLookAndFeel(&dBLookAndFeel);
//
//    SliderSetup(outputGainSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
//    SliderLabelSetup(outputGainLabel, "Out Gain dB", orangePeel, largeFontSize);
//    outputGainSlider.setLookAndFeel(&dBLookAndFeel);
//
//    SliderSetup(thresholdSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
//    SliderLabelSetup(thresholdLabel, "Threshold dB", orangePeel, smallFontSize);
//    thresholdSlider.setLookAndFeel(&dBLookAndFeel);
//
//    SliderSetup(ratioSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
//    SliderLabelSetup(ratioLabel, "Ratio", fieryRose, smallFontSize);
//    ratioSlider.setLookAndFeel(&compLookAndFeel);
//
//    SliderSetup(attackSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
//    SliderLabelSetup(attackLabel, "Attack ms", fieryRose, smallFontSize);
//    attackSlider.setLookAndFeel(&compLookAndFeel);
//
//    SliderSetup(releaseSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true);
//    SliderLabelSetup(releaseLabel, "Release ms", fieryRose, smallFontSize);
//    releaseSlider.setLookAndFeel(&compLookAndFeel);
//
//    peakRMSBox.addItem("Peak", 1);
//    peakRMSBox.addItem("RMS",  2);
//    peakRMSBox.setJustificationType(juce::Justification::centred);
//    peakRMSBox.setSelectedItemIndex(0);
//    peakRMSBox.setColour(juce::ComboBox::backgroundColourId, onyx);
//    peakRMSBox.setColour(juce::ComboBox::arrowColourId,      juce::Colours::white);
//    peakRMSBox.setColour(juce::ComboBox::outlineColourId,    lightSlateGrey);
//    peakRMSBox.setLookAndFeel(&boxLookAndFeel);
//    addAndMakeVisible(peakRMSBox);
//
//    ComboSetup(uiModeBox,     juce::StringArray({"Vibe", "Tweak"}));
//    ComboSetup(inputTypeBox,  juce::StringArray({"LR", "M/S"}));
//    ComboSetup(outputTypeBox, juce::StringArray({"LR", "M/S"}));
//    SliderLabelSetup(uiModeLabel,     "Mode",        magicMint, 16.0f);
//    SliderLabelSetup(inputTypeLabel,  "Input Type",  magicMint, 16.0f);
//    SliderLabelSetup(outputTypeLabel, "Output Type", magicMint, 16.0f);

//    addAndMakeVisible(tweakModeComp);
//    addAndMakeVisible(vibeModeComp);
//
//    inputGainSliderAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "inGain",     inputGainSlider);
//    sideChainGainSliderAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "sideInGain", sideChainGainSlider);
//    outputGainSliderAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "outGain",    outputGainSlider);
//
//    thresholdSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "threshold", thresholdSlider);
//    ratioSliderAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "ratio",     ratioSlider);
//    attackSliderAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "attack",    attackSlider);
//    releaseSliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "release",   releaseSlider);
//
//    peakRMSAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.parameters, "peakRMS",    peakRMSBox);
//    uiModeAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.parameters, "uiMode",     uiModeBox);
//    inputTypeAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.parameters, "inputType",  inputTypeBox);
//    outputTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.parameters, "outputType", outputTypeBox);
//
//    inLeftMeter.SetColors    (magicMint, fieryRose, lightSlateGrey);
//    inCenterMeter.SetColors  (magicMint, fieryRose, lightSlateGrey);
//    inRightMeter.SetColors   (magicMint, fieryRose, lightSlateGrey);
//    inSideMeter.SetColors    (magicMint, fieryRose, lightSlateGrey);
//    gainReduceMeter.SetColors(magicMint, fieryRose, lightSlateGrey);
//    outLeftMeter.SetColors   (magicMint, fieryRose, lightSlateGrey);
//    outCenterMeter.SetColors (magicMint, fieryRose, lightSlateGrey);
//    outRightMeter.SetColors  (magicMint, fieryRose, lightSlateGrey);
//
//    addAndMakeVisible(inLeftMeter);
//    addAndMakeVisible(inCenterMeter);
//    addAndMakeVisible(inRightMeter);
//    addAndMakeVisible(inSideMeter);
//    addAndMakeVisible(gainReduceMeter);
//    addAndMakeVisible(outLeftMeter);
//    addAndMakeVisible(outCenterMeter);
//    addAndMakeVisible(outRightMeter);

    audioProcessor.parameters.addParameterListener("uiMode", this);
    audioProcessor.parameters.addParameterListener("style",  this);

    ApplyUiModeVisibility();
    ApplyStyleVisibility();

    juce::Timer::startTimerHz(60);
}

CenterSpaceAudioProcessorEditor::~CenterSpaceAudioProcessorEditor()
{
    audioProcessor.parameters.removeParameterListener("uiMode", this);
    audioProcessor.parameters.removeParameterListener("style",  this);

//    inputGainSlider.setLookAndFeel    (nullptr);
//    sideChainGainSlider.setLookAndFeel(nullptr);
//    outputGainSlider.setLookAndFeel   (nullptr);
//    thresholdSlider.setLookAndFeel    (nullptr);
//    ratioSlider.setLookAndFeel        (nullptr);
//    attackSlider.setLookAndFeel       (nullptr);
//    releaseSlider.setLookAndFeel      (nullptr);
//    peakRMSBox.setLookAndFeel         (nullptr);
//    uiModeBox.setLookAndFeel          (nullptr);
//    inputTypeBox.setLookAndFeel       (nullptr);
//    outputTypeBox.setLookAndFeel      (nullptr);

    juce::Timer::stopTimer();
}

void CenterSpaceAudioProcessorEditor::paint(juce::Graphics &g)
{
//    g.fillAll(onyx);
    g.fillAll(resources.theme.background);
}

void CenterSpaceAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    titleHeader->setBounds(bounds.removeFromTop(80));
    titleFooter->setBounds(bounds.removeFromBottom(15));

//    float flanksSize = 0.25f;
//
//    juce::Rectangle<int> titleHeaderArea = bounds.removeFromTop(121).removeFromBottom(109);
//    titleHeader.setBounds(titleHeaderArea);
//
//    juce::Rectangle<int> titleFooterArea = bounds.removeFromBottom(15);
//    titleFooter.setBounds(titleFooterArea);
//
//    juce::Rectangle<int> topStrip = bounds.removeFromTop(56);
//    {
//        const int cellW = topStrip.getWidth() / 3;
//        auto leftCell   = topStrip.removeFromLeft(cellW).reduced(8, 4);
//        auto midCell    = topStrip.removeFromLeft(cellW).reduced(8, 4);
//        auto rightCell  = topStrip.reduced(8, 4);
//
//        uiModeLabel.setBounds(leftCell.removeFromTop(20));
//        uiModeBox.setBounds  (leftCell.removeFromTop(26));
//
//        inputTypeLabel.setBounds(midCell.removeFromTop(20));
//        inputTypeBox.setBounds  (midCell.removeFromTop(26));
//
//        outputTypeLabel.setBounds(rightCell.removeFromTop(20));
//        outputTypeBox.setBounds  (rightCell.removeFromTop(26));
//    }
//
//    juce::Rectangle<int> modeStrip = bounds.removeFromBottom(140);
//    tweakModeComp.setBounds(modeStrip);
//    vibeModeComp.setBounds (modeStrip);
//
//    juce::Rectangle<int> inputArea     = bounds.removeFromLeft    (getLocalBounds().getWidth() * flanksSize);
//    juce::Rectangle<int> inMetersArea  = inputArea.removeFromLeft (inputArea.getWidth() * 0.5f).reduced(10, 30);
//    juce::Rectangle<int> inControlArea = inputArea;
//
//    float inMeterWidth = inMetersArea.getWidth() * 0.33f;
//
//    juce::Rectangle<int> inMeterLArea = inMetersArea.removeFromLeft(inMeterWidth);
//    juce::Rectangle<int> inMeterCArea = inMetersArea.removeFromLeft(inMeterWidth);
//    juce::Rectangle<int> inMeterRArea = inMetersArea;
//
//    inLeftMeter.setBounds  (inMeterLArea);
//    inCenterMeter.setBounds(inMeterCArea);
//    inRightMeter.setBounds (inMeterRArea);
//
//    juce::Rectangle<int> inGainLabelArea  = inControlArea.removeFromTop   (labelH);
//    juce::Rectangle<int> inGainSliderArea = inControlArea.removeFromTop   (inControlArea.getHeight() * 0.5f - labelH);
//    juce::Rectangle<int> peakRMSBoxArea   = inControlArea.removeFromBottom(labelH * 4.0f);
//
//    inputGainLabel.setBounds (inGainLabelArea);
//    inputGainSlider.setBounds(inGainSliderArea);
//    peakRMSBox.setBounds     (peakRMSBoxArea.reduced(8, 32));
//
//    juce::Rectangle<int> outputArea     = bounds.removeFromRight    (getLocalBounds().getWidth() * flanksSize);
//    juce::Rectangle<int> outMetersArea  = outputArea.removeFromRight(outputArea.getWidth() * 0.5f).reduced(10, 30);
//    juce::Rectangle<int> outControlArea = outputArea;
//
//    float outMeterWidth = outMetersArea.getWidth() * 0.33f;
//
//    juce::Rectangle<int> outMeterLArea = outMetersArea.removeFromLeft(outMeterWidth);
//    juce::Rectangle<int> outMeterCArea = outMetersArea.removeFromLeft(outMeterWidth);
//    juce::Rectangle<int> outMeterRArea = outMetersArea;
//
//    outLeftMeter.setBounds  (outMeterLArea);
//    outCenterMeter.setBounds(outMeterCArea);
//    outRightMeter.setBounds (outMeterRArea);
//
//    juce::Rectangle<int> outGainLabelArea  = outControlArea.removeFromTop   (labelH);
//    juce::Rectangle<int> outGainSliderArea = outControlArea.removeFromTop   (outControlArea.getHeight() * 0.5f - labelH);
//    juce::Rectangle<int> outGainSpacerArea = outControlArea.removeFromBottom(labelH * 4.0f);
//
//    outputGainLabel.setBounds (outGainLabelArea);
//    outputGainSlider.setBounds(outGainSliderArea);
//
//    juce::Rectangle<int> compressorArea = bounds;
//
//    float compSectionsWidth = compressorArea.getWidth();
//    float compHeightDivs    = compressorArea.getHeight() * 0.33f;
//
//    float scOuterWeight       = 0.7f;                  // Weight of control areas of SC section combined 0.0f to 1.0f
//    float scControlAreaWeight = scOuterWeight * 0.5f;  // Weight of control areas individually
//
//    juce::Rectangle<int> scControlArea         = compressorArea.removeFromLeft       (compSectionsWidth * scControlAreaWeight);
//    juce::Rectangle<int> compControlArea       = compressorArea.removeFromRight      (compSectionsWidth * scControlAreaWeight);
//    juce::Rectangle<int> compressorAreaReduced = compressorArea.reduced              (10, 30);
//    juce::Rectangle<int> scGainMeterArea       = compressorAreaReduced.removeFromLeft(compressorAreaReduced.getWidth() * 0.5f);
//    juce::Rectangle<int> gainReductionArea     = compressorAreaReduced;
//
//    juce::Rectangle<int> scArea      = scControlArea.removeFromTop(compHeightDivs);
//    juce::Rectangle<int> scSpaceArea = scControlArea.removeFromTop(compHeightDivs);    // Empty Spacer
//    juce::Rectangle<int> threshArea  = scControlArea;
//
//    juce::Rectangle<int> scGainLabelArea = scArea.removeFromTop    (labelH);
//    juce::Rectangle<int> threshLabelArea = threshArea.removeFromTop(labelH);
//
//    sideChainGainLabel.setBounds (scGainLabelArea);
//    sideChainGainSlider.setBounds(scArea);
//    thresholdLabel.setBounds     (threshLabelArea);
//    thresholdSlider.setBounds    (threshArea);
//
//    inSideMeter.setBounds    (scGainMeterArea);
//    gainReduceMeter.setBounds(gainReductionArea);
//
//    juce::Rectangle<int> ratioArea   = compControlArea.removeFromTop(compHeightDivs).reduced(0, 5);
//    juce::Rectangle<int> attackArea  = compControlArea.removeFromTop(compHeightDivs).reduced(0, 5);
//    juce::Rectangle<int> releaseArea = compControlArea.reduced      (0, 5);
//
//    juce::Rectangle<int> ratioLabelArea   = ratioArea.removeFromTop  (labelH);
//    juce::Rectangle<int> attackLabelArea  = attackArea.removeFromTop (labelH);
//    juce::Rectangle<int> releaseLabelArea = releaseArea.removeFromTop(labelH);
//
//    ratioLabel.setBounds   (ratioLabelArea);
//    ratioSlider.setBounds  (ratioArea);
//    attackLabel.setBounds  (attackLabelArea);
//    attackSlider.setBounds (attackArea);
//    releaseLabel.setBounds (releaseLabelArea);
//    releaseSlider.setBounds(releaseArea);
}

void CenterSpaceAudioProcessorEditor::timerCallback()
{
    float SR = audioProcessor.getSampleRate();

//    inLeftMeter.VuMeterLevel    (audioProcessor.inLevelChan0.load(),  SR);
//    inCenterMeter.VuMeterLevel  (audioProcessor.inMidLevel.load(),    SR);
//    inRightMeter.VuMeterLevel   (audioProcessor.inLevelChan1.load(),  SR);
//    inSideMeter.VuMeterLevel    (audioProcessor.sideChainLevel.load(),SR);
//    gainReduceMeter.VuMeterLevel(audioProcessor.gainReduction.load(), SR);
//    outLeftMeter.VuMeterLevel   (audioProcessor.outLevelChan0.load(), SR);
//    outCenterMeter.VuMeterLevel (audioProcessor.outMidLevel.load(),   SR);
//    outRightMeter.VuMeterLevel  (audioProcessor.outLevelChan1.load(), SR);
}

void CenterSpaceAudioProcessorEditor::parameterChanged(const juce::String &paramId, float /*newValue*/)
{
    // Listener fires on whichever thread set the value (incl. audio thread via
    // automation). Hop to the message thread before touching Components.
    juce::Component::SafePointer<CenterSpaceAudioProcessorEditor> safeThis(this);
    juce::MessageManager::callAsync([safeThis, paramId]
    {
        if (safeThis == nullptr)
            return;

        if (paramId == "uiMode")
            safeThis->ApplyUiModeVisibility();
        else if (paramId == "style")
            safeThis->ApplyStyleVisibility();
    });
}

void CenterSpaceAudioProcessorEditor::ApplyUiModeVisibility()
{
    auto *uiModeRaw = audioProcessor.parameters.getRawParameterValue("uiMode");
    const bool isTweak = (uiModeRaw != nullptr) && ((int)uiModeRaw->load() == 1);

    // Tweak-only controls
//    sideChainGainSlider.setVisible(isTweak);
//    sideChainGainLabel .setVisible(isTweak);
//    thresholdSlider    .setVisible(isTweak);
//    thresholdLabel     .setVisible(isTweak);
//    ratioSlider        .setVisible(isTweak);
//    ratioLabel         .setVisible(isTweak);
//    attackSlider       .setVisible(isTweak);
//    attackLabel        .setVisible(isTweak);
//    releaseSlider      .setVisible(isTweak);
//    releaseLabel       .setVisible(isTweak);
//    peakRMSBox         .setVisible(isTweak);
//
//    tweakModeComp.setVisible(isTweak);
//    vibeModeComp.setVisible (!isTweak);

    if (isTweak)
        ApplyStyleVisibility();
}

void CenterSpaceAudioProcessorEditor::ApplyStyleVisibility()
{
    auto *styleRaw = audioProcessor.parameters.getRawParameterValue("style");
    const bool isModernVca = (styleRaw != nullptr) && ((int)styleRaw->load() == 0);

    auto *uiModeRaw = audioProcessor.parameters.getRawParameterValue("uiMode");
    const bool isTweak = (uiModeRaw != nullptr) && ((int)uiModeRaw->load() == 1);

    // Knee + Peak/RMS visible only in Tweak mode AND when style == Modern VCA.
    const bool kneeAndPeakVisible = isTweak && isModernVca;

//    peakRMSBox.setVisible(kneeAndPeakVisible);
//    tweakModeComp.SetKneeVisible(kneeAndPeakVisible);
}

//void CenterSpaceAudioProcessorEditor::SliderSetup(juce::Slider &sliderInstance, juce::Slider::SliderStyle style, bool showTextBox)
//{
//    sliderInstance.setSliderStyle(style);
//
//    // If slider has a textbox, draw it, otherwise, don't
//    if (showTextBox)
//    {
//        sliderInstance.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 15);
//        sliderInstance.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour((juce::uint8)0, (juce::uint8)0, (juce::uint8)0, (juce::uint8)0));
//        sliderInstance.setColour(juce::Slider::textBoxTextColourId,    juce::Colours::white);
//    }
//    else
//    {
//        sliderInstance.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
//    }
//
//    addAndMakeVisible(sliderInstance);
//}
//
//void CenterSpaceAudioProcessorEditor::SliderLabelSetup(juce::Label &labelInstance, juce::String labelText, juce::Colour &labelColor, float fontSize)
//{
//    labelInstance.setText             (labelText, juce::dontSendNotification);
//    labelInstance.setJustificationType(juce::Justification::centred);
//    labelInstance.setColour           (juce::Label::textColourId, juce::Colours::white);
//    labelInstance.setFont             (juce::Font("futura", fontSize, 0));
//    addAndMakeVisible(labelInstance);
//}
//
//void CenterSpaceAudioProcessorEditor::ComboSetup(juce::ComboBox &box, const juce::StringArray &items)
//{
//    box.addItemList(items, 1);
//    box.setJustificationType(juce::Justification::centred);
//    box.setColour(juce::ComboBox::backgroundColourId, onyx);
//    box.setColour(juce::ComboBox::arrowColourId, juce::Colours::white);
//    box.setColour(juce::ComboBox::outlineColourId, lightSlateGrey);
//    box.setLookAndFeel(&boxLookAndFeel);
//    addAndMakeVisible(box);
//}
