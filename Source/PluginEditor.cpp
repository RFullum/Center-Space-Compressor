/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CenterDuckComp2AudioProcessorEditor::CenterDuckComp2AudioProcessorEditor (CenterDuckComp2AudioProcessor& p)
                                      : AudioProcessorEditor (&p),
                                        backgroundColor (Colour( (uint8)33,  (uint8)77,  (uint8)143, (uint8)255 ) ),
                                        sliderTrack     (Colour( (uint8)39,  (uint8)19,  (uint8)92,  (uint8)255 ) ),
                                        textColor       (Colour( (uint8)188, (uint8)204, (uint8)230, (uint8)255 ) ),
                                        dBSliderColor   (Colour( (uint8)255, (uint8)89,  (uint8)0,   (uint8)255 ) ),
                                        compSliderColor (Colour( (uint8)89,  (uint8)255, (uint8)0,   (uint8)255 ) ),
                                        buttonColor     (Colour( (uint8)19,  (uint8)145, (uint8)63,  (uint8)255 ) ),
                                        sliderSize(125.0f), ratioSliderSize(175.0f), textBoxW(50.0f), textBoxH(25.0f),
                                        labelSize(50.0f), labelW(100.0f), labelH(25.0f),
                                        audioProcessor (p)
{
    //
    // INITIAL CONSTRUCTOR SETUP
    //
    setSize (1000, 500);
    
    // Custom Look And Feel
    compLookAndFeel.setDialColor (compSliderColor);
    compLookAndFeel.setTickColor (Colours::black);
    dBLookAndFeel.setDialColor   (dBSliderColor);
    dBLookAndFeel.setTickColor   (Colours::darkblue);
    
    // Header
    titleHeader.setBackgroundColor ( buttonColor );
    addAndMakeVisible              ( titleHeader );
    
    
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
    sliderSetup                    ( inputGainSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup               ( inputGainLabel, "In Gain dB" );
    inputGainSlider.setLookAndFeel ( &dBLookAndFeel );
    
    // SideChain Input Gain
    sliderSetup                        ( sideChainGainSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup                   ( sideChainGainLabel, "SCh Gain dB" );
    sideChainGainSlider.setLookAndFeel ( &dBLookAndFeel );
    
    // Output Gain
    sliderSetup                     ( outputGainSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup                ( outputGainLabel, "Out Gain dB" );
    outputGainSlider.setLookAndFeel ( &dBLookAndFeel );
    
    
    //=== Compressor Sliders ===
    
    // Threshold
    sliderSetup                    ( thresholdSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup               ( thresholdLabel, "Threshold" );
    thresholdSlider.setLookAndFeel ( &dBLookAndFeel );
    
    // Ratio
    sliderSetup                ( ratioSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup           ( ratioLabel, "Ratio" );
    ratioSlider.setLookAndFeel ( &compLookAndFeel );
    
    // Attack
    sliderSetup                 ( attackSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup            ( attackLabel, "Attack ms" );
    attackSlider.setLookAndFeel ( &compLookAndFeel );
    
    // Release
    sliderSetup                  ( releaseSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup             ( releaseLabel, "Release ms" );
    releaseSlider.setLookAndFeel ( &compLookAndFeel );
    
    //=== Combo Box ===
    peakRMSBox.addItem              ( "Peak", 1 );
    peakRMSBox.addItem              ( "RMS", 2 );
    peakRMSBox.setJustificationType ( Justification::centred );
    peakRMSBox.setSelectedItemIndex ( 0 );
    addAndMakeVisible               ( peakRMSBox );
    
    //
    //=== ATTACHMENTS ===
    //
    
    // Gain Sliders
    inputGainSliderAttachment      = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "inGain", inputGainSlider);
    sideChainGainSliderAttachement = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "sideInGain", sideChainGainSlider);
    outputGainSliderAttachment     = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "outGain", outputGainSlider);
    
    // Compressor Sliders
    thresholdSliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "threshold", thresholdSlider);
    ratioSliderAttachment     = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "ratio", ratioSlider);
    attackSliderAttachment    = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "attack", attackSlider);
    releaseSliderAttachment   = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "release", releaseSlider);
    
    // Combo Boxes
    peakRMSAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.parameters, "peakRMS", peakRMSBox);
    
    //
    //=== METERING ===
    //
    
    addAndMakeVisible( inLeftMeter );
    addAndMakeVisible( inCenterMeter );
    addAndMakeVisible( inRightMeter );
    addAndMakeVisible( inSideMeter );
    addAndMakeVisible( gainReduceMeter );
    addAndMakeVisible( outLeftMeter );
    addAndMakeVisible( outCenterMeter );
    addAndMakeVisible( outRightMeter );
    
    
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
}

/// Timer Callback for drawing meters
void CenterDuckComp2AudioProcessorEditor::timerCallback()
{
    float SR = audioProcessor.getSampleRate();
    
    inLeftMeter.vuMeterLevel     ( audioProcessor.inLeftLevel,    SR );
    inCenterMeter.vuMeterLevel   ( audioProcessor.inMidLevel,     SR );
    inRightMeter.vuMeterLevel    ( audioProcessor.inRightLevel,   SR );
    inSideMeter.vuMeterLevel     ( audioProcessor.sideChainLevel, SR );
    gainReduceMeter.vuMeterLevel ( audioProcessor.gainReduction,  SR );
    outLeftMeter.vuMeterLevel    ( audioProcessor.outLeftLevel,   SR );
    outCenterMeter.vuMeterLevel  ( audioProcessor.outMidLevel,    SR );
    outRightMeter.vuMeterLevel   ( audioProcessor.outRightLevel,  SR );
    
}

// ***** ROUGH-UP: SIZES ARE ALL WRONG. COLORS ARE TEMP ******
void CenterDuckComp2AudioProcessorEditor::resized()
{
    // Total Plugin Area
    auto area = getLocalBounds();
    
    // Title Header
    Rectangle<int> titleHeaderArea = area.removeFromTop( 75 );
    
    titleHeader.setBounds( titleHeaderArea );
    
    // Input Section (Left Side)
    Rectangle<int> inputArea     = area.removeFromLeft      ( getLocalBounds().getWidth() * 0.33f );
    Rectangle<int> inMetersArea  = inputArea.removeFromLeft ( inputArea.getWidth() * 0.5f );
    Rectangle<int> inControlArea = inputArea;
    
    float inMeterWidth = inMetersArea.getWidth() * 0.33f;
    
    Rectangle<int> inMeterLArea = inMetersArea.removeFromLeft ( inMeterWidth );
    Rectangle<int> inMeterCArea = inMetersArea.removeFromLeft ( inMeterWidth );
    Rectangle<int> inMeterRArea = inMetersArea;
    
    inLeftMeter.setBounds   ( inMeterLArea );
    inCenterMeter.setBounds ( inMeterCArea );
    inRightMeter.setBounds  ( inMeterRArea );
    
    Rectangle<int> inGainLabelArea  = inControlArea.removeFromTop    ( labelH );
    Rectangle<int> peakRMSBoxArea   = inControlArea.removeFromBottom ( labelH );
    Rectangle<int> inGainSliderArea = inControlArea.removeFromTop    ( inControlArea.getHeight() * 0.5f );
    
    inputGainLabel.setBounds  ( inGainLabelArea );
    inputGainSlider.setBounds ( inGainSliderArea );
    peakRMSBox.setBounds      ( peakRMSBoxArea );
    
    // Output Section (Right Side)
    Rectangle<int> outputArea     = area.removeFromRight       ( getLocalBounds().getWidth() * 0.33f );
    Rectangle<int> outMetersArea  = outputArea.removeFromRight ( outputArea.getWidth() * 0.5f );
    Rectangle<int> outControlArea = outputArea;
    
    float outMeterWidth = outMetersArea.getWidth() * 0.33f;
    
    Rectangle<int> outMeterLArea = outMetersArea.removeFromLeft ( outMeterWidth );
    Rectangle<int> outMeterCArea = outMetersArea.removeFromLeft ( outMeterWidth );
    Rectangle<int> outMeterRArea = outMetersArea;
    
    outLeftMeter.setBounds   ( outMeterLArea );
    outCenterMeter.setBounds ( outMeterCArea );
    outRightMeter.setBounds  ( outMeterRArea );
    
    Rectangle<int> outGainLabelArea  = outControlArea.removeFromTop    ( labelH );
    Rectangle<int> outGainSpacerArea = outControlArea.removeFromBottom ( labelH );
    Rectangle<int> outGainSliderArea = outControlArea.removeFromTop    ( outControlArea.getHeight() * 0.5f );
    
    outputGainLabel.setBounds  ( outGainLabelArea );
    outputGainSlider.setBounds ( outGainSliderArea );
    
    // Compressor Section (Center)
    Rectangle<int> compressorArea = area;
    
    float compSectionsWidth = compressorArea.getWidth()  * 0.25f;
    float compHeightDivs    = compressorArea.getHeight() * 0.33f;
    
    Rectangle<int> scControlArea     = compressorArea.removeFromLeft ( compSectionsWidth );
    Rectangle<int> scGainMeterArea   = compressorArea.removeFromLeft ( compSectionsWidth );
    Rectangle<int> gainReductionArea = compressorArea.removeFromLeft ( compSectionsWidth );
    Rectangle<int> compControlArea   = compressorArea;
    
    // Sidechain Congrol area (left: SC Gain & Threshold)
    Rectangle<int> scArea      = scControlArea.removeFromTop ( compHeightDivs );
    Rectangle<int> scSpaceArea = scControlArea.removeFromTop ( compHeightDivs );
    Rectangle<int> threshArea  = scControlArea;
    
    Rectangle<int> scGainLabelArea = scArea.removeFromTop     ( labelH );
    Rectangle<int> threshLabelArea = threshArea.removeFromTop ( labelH );
    
    sideChainGainLabel.setBounds  ( scGainLabelArea );
    sideChainGainSlider.setBounds ( scArea );
    thresholdLabel.setBounds      ( threshLabelArea );
    thresholdSlider.setBounds     ( threshArea );
    
    // SC Metering
    inSideMeter.setBounds     ( scGainMeterArea );
    gainReduceMeter.setBounds ( gainReductionArea );    // *** NOT VISIBLE BECAUSE CLASS NOT DEFINED FROM PARENT YET
    
    // Comp Control area (right: Ratio, attack release)
    Rectangle<int> ratioArea   = compControlArea.removeFromTop ( compHeightDivs );
    Rectangle<int> attackArea  = compControlArea.removeFromTop ( compHeightDivs );
    Rectangle<int> releaseArea = compControlArea;
    
    Rectangle<int> ratioLabelArea   = ratioArea.removeFromTop   ( labelH );
    Rectangle<int> attackLabelArea  = attackArea.removeFromTop  ( labelH );
    Rectangle<int> releaseLabelArea = releaseArea.removeFromTop ( labelH );
    
    ratioLabel.setBounds    ( ratioLabelArea );
    ratioSlider.setBounds   ( ratioArea );
    attackLabel.setBounds   ( attackLabelArea );
    attackSlider.setBounds  ( attackArea );
    releaseLabel.setBounds  ( releaseLabelArea );
    releaseSlider.setBounds ( releaseArea );
    
    
}


/// Sets up Slider object instances in constructor. sliderInstance is the slider to set up, suffix is textValueSuffix, sliderFillColor is the slider color below the thumb
void CenterDuckComp2AudioProcessorEditor::sliderSetup(Slider& sliderInstance, Slider::SliderStyle style, bool showTextBox)
{
    sliderInstance.setSliderStyle(style);
    
    // If slider has a textbox, draw it, otherwise, don't
    if (showTextBox)
    {
        sliderInstance.setTextBoxStyle ( Slider::TextBoxBelow, false, 40, 20 );
        sliderInstance.setColour       ( Slider::textBoxOutlineColourId, Colour( (uint8)0, (uint8)0, (uint8)0, (uint8)0 ) );
        sliderInstance.setColour       ( Slider::textBoxTextColourId, textColor );
    }
    else
    {
        sliderInstance.setTextBoxStyle( Slider::NoTextBox, false, 0, 0 );
    }
    
    addAndMakeVisible(sliderInstance);
}

/// Sets up Label for the Slider instances. Takes the labelInstance and the text for setText
void CenterDuckComp2AudioProcessorEditor::sliderLabelSetup(Label& labelInstance, String labelText)
{
    labelInstance.setText(labelText, dontSendNotification);
    labelInstance.setJustificationType(Justification::centred);
    addAndMakeVisible(labelInstance);
}
