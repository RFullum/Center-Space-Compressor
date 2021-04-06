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
                                        onyx            (Colour( (uint8)53,  (uint8)59,  (uint8)60,  (uint8)255 ) ),
                                        lightSlateGrey  (Colour( (uint8)130, (uint8)146, (uint8)152, (uint8)255 ) ),
                                        magicMint       (Colour( (uint8)174, (uint8)255, (uint8)216, (uint8)255 ) ),
                                        fieryRose       (Colour( (uint8)255, (uint8)104, (uint8)114, (uint8)255 ) ),
                                        orangePeel      (Colour( (uint8)252, (uint8)151, (uint8)0,   (uint8)255 ) ),
                                        sliderSize(125.0f), ratioSliderSize(175.0f), textBoxW(50.0f), textBoxH(25.0f),
                                        labelSize(50.0f), labelW(100.0f), labelH(25.0f),
                                        audioProcessor (p)
{
    //
    // INITIAL CONSTRUCTOR SETUP
    //
    setSize (1100, 540);
    
    // Custom Look And Feel
    compLookAndFeel.setDialColor ( fieryRose  );
    compLookAndFeel.setTickColor ( onyx       );
    compLookAndFeel.setBackColor ( magicMint  );
    dBLookAndFeel.setDialColor   ( orangePeel );
    dBLookAndFeel.setTickColor   ( onyx       );
    dBLookAndFeel.setBackColor   ( magicMint  );
    
    boxLookAndFeel.setOutlineColor ( magicMint );
    
    // Headers
    titleHeader.setBackgroundColor ( onyx );
    titleHeader.setTextColor       ( magicMint );
    addAndMakeVisible              ( titleHeader );
    
    titleFooter.setBackgroundColor ( onyx );
    titleFooter.setTextColor       ( magicMint );
    addAndMakeVisible              ( titleFooter );
    
    
    
    
    //
    //=== SLIDERS AND LABELS SETUP ===
    //
    
    //=== Look and Feel (global) ===
    getLookAndFeel().setColour(Label::textColourId, magicMint );
    
    
    //=== Gain Sliders ===
    float largeFontSize = 27.0f;
    float smallFontSize = 22.0f;
    
    // Input Gain
    sliderSetup                    ( inputGainSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup               ( inputGainLabel, "In Gain dB", orangePeel, largeFontSize );
    inputGainSlider.setLookAndFeel ( &dBLookAndFeel );
    
    // SideChain Input Gain
    sliderSetup                        ( sideChainGainSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup                   ( sideChainGainLabel, "SCh Gain dB", orangePeel, smallFontSize );
    sideChainGainSlider.setLookAndFeel ( &dBLookAndFeel );
    
    // Output Gain
    sliderSetup                     ( outputGainSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup                ( outputGainLabel, "Out Gain dB", orangePeel, largeFontSize );
    outputGainSlider.setLookAndFeel ( &dBLookAndFeel );
    
    
    //=== Compressor Sliders ===
    
    // Threshold
    sliderSetup                    ( thresholdSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup               ( thresholdLabel, "Threshold dB", orangePeel, smallFontSize );
    thresholdSlider.setLookAndFeel ( &dBLookAndFeel );
    
    // Ratio
    sliderSetup                ( ratioSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup           ( ratioLabel, "Ratio", fieryRose, smallFontSize );
    ratioSlider.setLookAndFeel ( &compLookAndFeel );
    
    // Attack
    sliderSetup                 ( attackSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup            ( attackLabel, "Attack ms", fieryRose, smallFontSize );
    attackSlider.setLookAndFeel ( &compLookAndFeel );
    
    // Release
    sliderSetup                  ( releaseSlider, Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup             ( releaseLabel, "Release ms", fieryRose, smallFontSize );
    releaseSlider.setLookAndFeel ( &compLookAndFeel );
    
    //=== Combo Box ===
    peakRMSBox.addItem              ( "Peak", 1 );
    peakRMSBox.addItem              ( "RMS", 2 );
    peakRMSBox.setJustificationType ( Justification::centred );
    peakRMSBox.setSelectedItemIndex ( 0 );
    peakRMSBox.setColour            ( ComboBox::backgroundColourId, onyx );
    peakRMSBox.setColour            ( ComboBox::arrowColourId, Colours::white );
    peakRMSBox.setColour            ( ComboBox::outlineColourId, lightSlateGrey );
    peakRMSBox.setLookAndFeel       ( &boxLookAndFeel );
    addAndMakeVisible               ( peakRMSBox );
    
    //
    //=== ATTACHMENTS ===
    //
    
    // Gain Sliders
    inputGainSliderAttachment      = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "inGain", inputGainSlider );
    sideChainGainSliderAttachement = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "sideInGain", sideChainGainSlider );
    outputGainSliderAttachment     = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "outGain", outputGainSlider );
    
    // Compressor Sliders
    thresholdSliderAttachment      = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "threshold", thresholdSlider );
    ratioSliderAttachment          = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "ratio", ratioSlider );
    attackSliderAttachment         = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "attack", attackSlider );
    releaseSliderAttachment        = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "release", releaseSlider );
    
    // Combo Boxes
    peakRMSAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>( audioProcessor.parameters, "peakRMS", peakRMSBox );
    
    //
    //=== METERING ===
    //
    
    inLeftMeter.setColors     ( magicMint, fieryRose );
    inCenterMeter.setColors   ( magicMint, fieryRose );
    inRightMeter.setColors    ( magicMint, fieryRose );
    inSideMeter.setColors     ( magicMint, fieryRose );
    gainReduceMeter.setColors ( magicMint, fieryRose );
    outLeftMeter.setColors    ( magicMint, fieryRose );
    outCenterMeter.setColors  ( magicMint, fieryRose );
    outRightMeter.setColors   ( magicMint, fieryRose );
    
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
    g.fillAll(onyx);
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
    
    float flanksSize = 0.25f;
    
    // Header & Footer
    
    // Title Header & Footer
    Rectangle<int> titleHeaderArea = area.removeFromTop( 75 );
    
    titleHeader.setBounds( titleHeaderArea );
    
    Rectangle<int> titleFooterArea = area.removeFromBottom( 15 );
    
    titleFooter.setBounds( titleFooterArea );
    
    // Input Section (Left Side)
    Rectangle<int> inputArea     = area.removeFromLeft      ( getLocalBounds().getWidth() * flanksSize );
    Rectangle<int> inMetersArea  = inputArea.removeFromLeft ( inputArea.getWidth() * 0.5f ).reduced( 10, 30 );
    Rectangle<int> inControlArea = inputArea;
    
    float inMeterWidth = inMetersArea.getWidth() * 0.33f;
    
    Rectangle<int> inMeterLArea = inMetersArea.removeFromLeft ( inMeterWidth );
    Rectangle<int> inMeterCArea = inMetersArea.removeFromLeft ( inMeterWidth );
    Rectangle<int> inMeterRArea = inMetersArea;
    
    inLeftMeter.setBounds   ( inMeterLArea );
    inCenterMeter.setBounds ( inMeterCArea );
    inRightMeter.setBounds  ( inMeterRArea );
    
    Rectangle<int> inGainLabelArea  = inControlArea.removeFromTop    ( labelH );
    Rectangle<int> inGainSliderArea = inControlArea.removeFromTop    ( inControlArea.getHeight() * 0.5f - labelH );
    Rectangle<int> peakRMSBoxArea   = inControlArea.removeFromBottom ( labelH * 4.0f );
    
    inputGainLabel.setBounds  ( inGainLabelArea );
    inputGainSlider.setBounds ( inGainSliderArea );
    peakRMSBox.setBounds      ( peakRMSBoxArea.reduced( 8, 32) );
    
    // Output Section (Right Side)
    Rectangle<int> outputArea     = area.removeFromRight       ( getLocalBounds().getWidth() * flanksSize );
    Rectangle<int> outMetersArea  = outputArea.removeFromRight ( outputArea.getWidth() * 0.5f ).reduced( 10, 30 );
    Rectangle<int> outControlArea = outputArea;
    
    float outMeterWidth = outMetersArea.getWidth() * 0.33f;
    
    Rectangle<int> outMeterLArea = outMetersArea.removeFromLeft ( outMeterWidth );
    Rectangle<int> outMeterCArea = outMetersArea.removeFromLeft ( outMeterWidth );
    Rectangle<int> outMeterRArea = outMetersArea;
    
    outLeftMeter.setBounds   ( outMeterLArea );
    outCenterMeter.setBounds ( outMeterCArea );
    outRightMeter.setBounds  ( outMeterRArea );
    
    Rectangle<int> outGainLabelArea  = outControlArea.removeFromTop    ( labelH );
    Rectangle<int> outGainSliderArea = outControlArea.removeFromTop    ( outControlArea.getHeight() * 0.5f - labelH );
    Rectangle<int> outGainSpacerArea = outControlArea.removeFromBottom ( labelH * 4.0f );
    
    outputGainLabel.setBounds  ( outGainLabelArea );
    outputGainSlider.setBounds ( outGainSliderArea );
    
    // Compressor Section (Center)
    Rectangle<int> compressorArea = area;
    
    float compSectionsWidth = compressorArea.getWidth();
    float compHeightDivs    = compressorArea.getHeight() * 0.33f;
    
    float scOuterWeight       = 0.7f;                           // Weight of control areas of SC section combined 0.0f to 1.0f
    float scControlAreaWeight = scOuterWeight * 0.5f;           // Weight of control areas individually
    
    Rectangle<int> scControlArea         = compressorArea.removeFromLeft        ( compSectionsWidth * scControlAreaWeight );
    Rectangle<int> compControlArea       = compressorArea.removeFromRight       ( compSectionsWidth * scControlAreaWeight );
    Rectangle<int> compressorAreaReduced = compressorArea.reduced               ( 10, 30 );
    Rectangle<int> scGainMeterArea       = compressorAreaReduced.removeFromLeft ( compressorAreaReduced.getWidth() * 0.5f );
    Rectangle<int> gainReductionArea     = compressorAreaReduced;
    
    
    // Sidechain Congrol area (left: SC Gain & Threshold)
    Rectangle<int> scArea      = scControlArea.removeFromTop ( compHeightDivs );
    Rectangle<int> scSpaceArea = scControlArea.removeFromTop ( compHeightDivs );    // Empty Spacer
    Rectangle<int> threshArea  = scControlArea;
    
    Rectangle<int> scGainLabelArea = scArea.removeFromTop     ( labelH );
    Rectangle<int> threshLabelArea = threshArea.removeFromTop ( labelH );
    
    sideChainGainLabel.setBounds  ( scGainLabelArea );
    sideChainGainSlider.setBounds ( scArea );
    thresholdLabel.setBounds      ( threshLabelArea );
    thresholdSlider.setBounds     ( threshArea );
    
    // SC Metering
    inSideMeter.setBounds     ( scGainMeterArea   );
    gainReduceMeter.setBounds ( gainReductionArea );
    
    // Comp Control area (right: Ratio, attack release)
    Rectangle<int> ratioArea   = compControlArea.removeFromTop ( compHeightDivs ).reduced ( 0, 5 );
    Rectangle<int> attackArea  = compControlArea.removeFromTop ( compHeightDivs ).reduced ( 0, 5 );
    Rectangle<int> releaseArea = compControlArea.reduced       ( 0, 5 );
    
    Rectangle<int> ratioLabelArea   = ratioArea.removeFromTop   ( labelH );
    Rectangle<int> attackLabelArea  = attackArea.removeFromTop  ( labelH );
    Rectangle<int> releaseLabelArea = releaseArea.removeFromTop ( labelH );
    
    ratioLabel.setBounds    ( ratioLabelArea   );
    ratioSlider.setBounds   ( ratioArea        );
    attackLabel.setBounds   ( attackLabelArea  );
    attackSlider.setBounds  ( attackArea       );
    releaseLabel.setBounds  ( releaseLabelArea );
    releaseSlider.setBounds ( releaseArea      );
    
    
}


/// Sets up Slider object instances in constructor. sliderInstance is the slider to set up, suffix is textValueSuffix, sliderFillColor is the slider color below the thumb
void CenterDuckComp2AudioProcessorEditor::sliderSetup(Slider& sliderInstance, Slider::SliderStyle style, bool showTextBox)
{
    sliderInstance.setSliderStyle(style);
    
    // If slider has a textbox, draw it, otherwise, don't
    if (showTextBox)
    {
        sliderInstance.setTextBoxStyle ( Slider::TextBoxBelow, false, 50, 15 );
        sliderInstance.setColour       ( Slider::textBoxOutlineColourId, Colour( (uint8)0, (uint8)0, (uint8)0, (uint8)0 ) );
        sliderInstance.setColour       ( Slider::textBoxTextColourId, Colours::white );
        
    }
    else
    {
        sliderInstance.setTextBoxStyle( Slider::NoTextBox, false, 0, 0 );
    }
    
    addAndMakeVisible(sliderInstance);
}

/// Sets up Label for the Slider instances. Takes the labelInstance and the text for setText
void CenterDuckComp2AudioProcessorEditor::sliderLabelSetup(Label& labelInstance, String labelText, Colour& labelColor, float fontSize)
{
    labelInstance.setText              ( labelText, dontSendNotification     );
    labelInstance.setJustificationType ( Justification::centred              );
    labelInstance.setColour            ( Label::textColourId, Colours::white );
    labelInstance.setFont              ( Font( "futura", fontSize, 0 )       );
    addAndMakeVisible                  ( labelInstance                       );
}
