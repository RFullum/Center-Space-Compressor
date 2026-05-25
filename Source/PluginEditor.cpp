/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CenterSpaceAudioProcessorEditor::CenterSpaceAudioProcessorEditor (CenterSpaceAudioProcessor& p)
                                      : juce::AudioProcessorEditor (&p),
                                        onyx            (juce::Colour( (juce::uint8)53,  (juce::uint8)59,  (juce::uint8)60,  (juce::uint8)255 ) ),
                                        lightSlateGrey  (juce::Colour( (juce::uint8)130, (juce::uint8)146, (juce::uint8)152, (juce::uint8)255 ) ),
                                        magicMint       (juce::Colour( (juce::uint8)174, (juce::uint8)255, (juce::uint8)216, (juce::uint8)255 ) ),
                                        fieryRose       (juce::Colour( (juce::uint8)255, (juce::uint8)104, (juce::uint8)114, (juce::uint8)255 ) ),
                                        orangePeel      (juce::Colour( (juce::uint8)252, (juce::uint8)151, (juce::uint8)0,   (juce::uint8)255 ) ),
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
    titleHeader.setDesignColor     ( fieryRose );
    titleHeader.setTextColor       ( orangePeel  );
    addAndMakeVisible              ( titleHeader );
    
    titleFooter.setBackgroundColor ( onyx );
    titleFooter.setTextColor       ( magicMint );
    addAndMakeVisible              ( titleFooter );
    
    
    
    
    //
    //=== SLIDERS AND LABELS SETUP ===
    //
    
    //=== Look and Feel (global) ===
    getLookAndFeel().setColour(juce::Label::textColourId, magicMint );


    //=== Gain Sliders ===
    float largeFontSize = 27.0f;
    float smallFontSize = 22.0f;

    // Input Gain
    sliderSetup                    ( inputGainSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup               ( inputGainLabel, "In Gain dB", orangePeel, largeFontSize );
    inputGainSlider.setLookAndFeel ( &dBLookAndFeel );

    // SideChain Input Gain
    sliderSetup                        ( sideChainGainSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup                   ( sideChainGainLabel, "SCh Gain dB", orangePeel, smallFontSize );
    sideChainGainSlider.setLookAndFeel ( &dBLookAndFeel );

    // Output Gain
    sliderSetup                     ( outputGainSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup                ( outputGainLabel, "Out Gain dB", orangePeel, largeFontSize );
    outputGainSlider.setLookAndFeel ( &dBLookAndFeel );


    //=== Compressor Sliders ===

    // Threshold
    sliderSetup                    ( thresholdSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup               ( thresholdLabel, "Threshold dB", orangePeel, smallFontSize );
    thresholdSlider.setLookAndFeel ( &dBLookAndFeel );

    // Ratio
    sliderSetup                ( ratioSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup           ( ratioLabel, "Ratio", fieryRose, smallFontSize );
    ratioSlider.setLookAndFeel ( &compLookAndFeel );

    // Attack
    sliderSetup                 ( attackSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup            ( attackLabel, "Attack ms", fieryRose, smallFontSize );
    attackSlider.setLookAndFeel ( &compLookAndFeel );

    // Release
    sliderSetup                  ( releaseSlider, juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, true );
    sliderLabelSetup             ( releaseLabel, "Release ms", fieryRose, smallFontSize );
    releaseSlider.setLookAndFeel ( &compLookAndFeel );

    //=== Combo Box ===
    peakRMSBox.addItem              ( "Peak", 1 );
    peakRMSBox.addItem              ( "RMS", 2 );
    peakRMSBox.setJustificationType ( juce::Justification::centred );
    peakRMSBox.setSelectedItemIndex ( 0 );
    peakRMSBox.setColour            ( juce::ComboBox::backgroundColourId, onyx );
    peakRMSBox.setColour            ( juce::ComboBox::arrowColourId, juce::Colours::white );
    peakRMSBox.setColour            ( juce::ComboBox::outlineColourId, lightSlateGrey );
    peakRMSBox.setLookAndFeel       ( &boxLookAndFeel );
    addAndMakeVisible               ( peakRMSBox );
    
    //
    //=== ATTACHMENTS ===
    //
    
    // Gain Sliders
    inputGainSliderAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "inGain", inputGainSlider );
    sideChainGainSliderAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "sideInGain", sideChainGainSlider );
    outputGainSliderAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "outGain", outputGainSlider );

    // Compressor Sliders
    thresholdSliderAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "threshold", thresholdSlider );
    ratioSliderAttachment          = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "ratio", ratioSlider );
    attackSliderAttachment         = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "attack", attackSlider );
    releaseSliderAttachment        = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>( audioProcessor.parameters, "release", releaseSlider );

    // Combo Boxes
    peakRMSAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>( audioProcessor.parameters, "peakRMS", peakRMSBox );
    
    //
    //=== METERING ===
    //
    
    inLeftMeter.setColors     ( magicMint, fieryRose, lightSlateGrey );
    inCenterMeter.setColors   ( magicMint, fieryRose, lightSlateGrey );
    inRightMeter.setColors    ( magicMint, fieryRose, lightSlateGrey );
    inSideMeter.setColors     ( magicMint, fieryRose, lightSlateGrey );
    gainReduceMeter.setColors ( magicMint, fieryRose, lightSlateGrey );
    outLeftMeter.setColors    ( magicMint, fieryRose, lightSlateGrey );
    outCenterMeter.setColors  ( magicMint, fieryRose, lightSlateGrey );
    outRightMeter.setColors   ( magicMint, fieryRose, lightSlateGrey );
    
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
    juce::Timer::startTimerHz(60);

}

CenterSpaceAudioProcessorEditor::~CenterSpaceAudioProcessorEditor()
{
    juce::Timer::stopTimer();
}

//==============================================================================
void CenterSpaceAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(onyx);
}

/// Timer Callback for drawing meters
void CenterSpaceAudioProcessorEditor::timerCallback()
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


void CenterSpaceAudioProcessorEditor::resized()
{
    // Total Plugin Area
    auto area = getLocalBounds();
    
    float flanksSize = 0.25f;
    
    // Header & Footer
    
    // Title Header & Footer
    juce::Rectangle<int> titleHeaderArea = area.removeFromTop( 121 ).removeFromBottom( 109 );
    
    titleHeader.setBounds( titleHeaderArea );
    
    juce::Rectangle<int> titleFooterArea = area.removeFromBottom( 15 );
    
    titleFooter.setBounds( titleFooterArea );
    
    // Input Section (Left Side)
    juce::Rectangle<int> inputArea     = area.removeFromLeft      ( getLocalBounds().getWidth() * flanksSize );
    juce::Rectangle<int> inMetersArea  = inputArea.removeFromLeft ( inputArea.getWidth() * 0.5f ).reduced( 10, 30 );
    juce::Rectangle<int> inControlArea = inputArea;
    
    float inMeterWidth = inMetersArea.getWidth() * 0.33f;
    
    juce::Rectangle<int> inMeterLArea = inMetersArea.removeFromLeft ( inMeterWidth );
    juce::Rectangle<int> inMeterCArea = inMetersArea.removeFromLeft ( inMeterWidth );
    juce::Rectangle<int> inMeterRArea = inMetersArea;
    
    inLeftMeter.setBounds   ( inMeterLArea );
    inCenterMeter.setBounds ( inMeterCArea );
    inRightMeter.setBounds  ( inMeterRArea );
    
    juce::Rectangle<int> inGainLabelArea  = inControlArea.removeFromTop    ( labelH );
    juce::Rectangle<int> inGainSliderArea = inControlArea.removeFromTop    ( inControlArea.getHeight() * 0.5f - labelH );
    juce::Rectangle<int> peakRMSBoxArea   = inControlArea.removeFromBottom ( labelH * 4.0f );
    
    inputGainLabel.setBounds  ( inGainLabelArea );
    inputGainSlider.setBounds ( inGainSliderArea );
    peakRMSBox.setBounds      ( peakRMSBoxArea.reduced( 8, 32) );
    
    // Output Section (Right Side)
    juce::Rectangle<int> outputArea     = area.removeFromRight       ( getLocalBounds().getWidth() * flanksSize );
    juce::Rectangle<int> outMetersArea  = outputArea.removeFromRight ( outputArea.getWidth() * 0.5f ).reduced( 10, 30 );
    juce::Rectangle<int> outControlArea = outputArea;
    
    float outMeterWidth = outMetersArea.getWidth() * 0.33f;
    
    juce::Rectangle<int> outMeterLArea = outMetersArea.removeFromLeft ( outMeterWidth );
    juce::Rectangle<int> outMeterCArea = outMetersArea.removeFromLeft ( outMeterWidth );
    juce::Rectangle<int> outMeterRArea = outMetersArea;
    
    outLeftMeter.setBounds   ( outMeterLArea );
    outCenterMeter.setBounds ( outMeterCArea );
    outRightMeter.setBounds  ( outMeterRArea );
    
    juce::Rectangle<int> outGainLabelArea  = outControlArea.removeFromTop    ( labelH );
    juce::Rectangle<int> outGainSliderArea = outControlArea.removeFromTop    ( outControlArea.getHeight() * 0.5f - labelH );
    juce::Rectangle<int> outGainSpacerArea = outControlArea.removeFromBottom ( labelH * 4.0f );
    
    outputGainLabel.setBounds  ( outGainLabelArea );
    outputGainSlider.setBounds ( outGainSliderArea );
    
    // Compressor Section (Center)
    juce::Rectangle<int> compressorArea = area;
    
    float compSectionsWidth = compressorArea.getWidth();
    float compHeightDivs    = compressorArea.getHeight() * 0.33f;
    
    float scOuterWeight       = 0.7f;                           // Weight of control areas of SC section combined 0.0f to 1.0f
    float scControlAreaWeight = scOuterWeight * 0.5f;           // Weight of control areas individually
    
    juce::Rectangle<int> scControlArea         = compressorArea.removeFromLeft        ( compSectionsWidth * scControlAreaWeight );
    juce::Rectangle<int> compControlArea       = compressorArea.removeFromRight       ( compSectionsWidth * scControlAreaWeight );
    juce::Rectangle<int> compressorAreaReduced = compressorArea.reduced               ( 10, 30 );
    juce::Rectangle<int> scGainMeterArea       = compressorAreaReduced.removeFromLeft ( compressorAreaReduced.getWidth() * 0.5f );
    juce::Rectangle<int> gainReductionArea     = compressorAreaReduced;
    
    
    // Sidechain Congrol area (left: SC Gain & Threshold)
    juce::Rectangle<int> scArea      = scControlArea.removeFromTop ( compHeightDivs );
    juce::Rectangle<int> scSpaceArea = scControlArea.removeFromTop ( compHeightDivs );    // Empty Spacer
    juce::Rectangle<int> threshArea  = scControlArea;
    
    juce::Rectangle<int> scGainLabelArea = scArea.removeFromTop     ( labelH );
    juce::Rectangle<int> threshLabelArea = threshArea.removeFromTop ( labelH );
    
    sideChainGainLabel.setBounds  ( scGainLabelArea );
    sideChainGainSlider.setBounds ( scArea );
    thresholdLabel.setBounds      ( threshLabelArea );
    thresholdSlider.setBounds     ( threshArea );
    
    // SC Metering
    inSideMeter.setBounds     ( scGainMeterArea   );
    gainReduceMeter.setBounds ( gainReductionArea );
    
    // Comp Control area (right: Ratio, attack release)
    juce::Rectangle<int> ratioArea   = compControlArea.removeFromTop ( compHeightDivs ).reduced ( 0, 5 );
    juce::Rectangle<int> attackArea  = compControlArea.removeFromTop ( compHeightDivs ).reduced ( 0, 5 );
    juce::Rectangle<int> releaseArea = compControlArea.reduced       ( 0, 5 );
    
    juce::Rectangle<int> ratioLabelArea   = ratioArea.removeFromTop   ( labelH );
    juce::Rectangle<int> attackLabelArea  = attackArea.removeFromTop  ( labelH );
    juce::Rectangle<int> releaseLabelArea = releaseArea.removeFromTop ( labelH );
    
    ratioLabel.setBounds    ( ratioLabelArea   );
    ratioSlider.setBounds   ( ratioArea        );
    attackLabel.setBounds   ( attackLabelArea  );
    attackSlider.setBounds  ( attackArea       );
    releaseLabel.setBounds  ( releaseLabelArea );
    releaseSlider.setBounds ( releaseArea      );
    
    
}


/// Sets up Slider object instances in constructor. sliderInstance is the slider to set up, suffix is textValueSuffix, sliderFillColor is the slider color below the thumb
void CenterSpaceAudioProcessorEditor::sliderSetup(juce::Slider& sliderInstance, juce::Slider::SliderStyle style, bool showTextBox)
{
    sliderInstance.setSliderStyle(style);

    // If slider has a textbox, draw it, otherwise, don't
    if (showTextBox)
    {
        sliderInstance.setTextBoxStyle ( juce::Slider::TextBoxBelow, false, 50, 15 );
        sliderInstance.setColour       ( juce::Slider::textBoxOutlineColourId, juce::Colour( (juce::uint8)0, (juce::uint8)0, (juce::uint8)0, (juce::uint8)0 ) );
        sliderInstance.setColour       ( juce::Slider::textBoxTextColourId, juce::Colours::white );

    }
    else
    {
        sliderInstance.setTextBoxStyle( juce::Slider::NoTextBox, false, 0, 0 );
    }

    addAndMakeVisible(sliderInstance);
}

/// Sets up Label for the Slider instances. Takes the labelInstance and the text for setText
void CenterSpaceAudioProcessorEditor::sliderLabelSetup(juce::Label& labelInstance, juce::String labelText, juce::Colour& labelColor, float fontSize)
{
    labelInstance.setText              ( labelText, juce::dontSendNotification     );
    labelInstance.setJustificationType ( juce::Justification::centred              );
    labelInstance.setColour            ( juce::Label::textColourId, juce::Colours::white );
    labelInstance.setFont              ( juce::Font( "futura", fontSize, 0 )       );
    addAndMakeVisible                  ( labelInstance                             );
}
