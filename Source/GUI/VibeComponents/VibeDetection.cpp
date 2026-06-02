/*
  ==============================================================================

 VibeDetection.h
    Author:  Robert Fullum

  ==============================================================================
*/

#include "VibeDetection.h"

#include "GuiHelpers.h"
#include "Selector.h"

//==============================================================================

VibeDetection::VibeDetection(GuiResources &resources)
: laSelector(std::make_unique<LookaheadVibeSelector>(resources, "lookaheadOnOff"))
{
    CenterSpace::SetupSlider(this
                             , compressSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    compressSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    CenterSpace::SetupLabel(this
                            , compressLabel
                            , "Compress"
                            , resources.theme.textPrimary
                            , 15.0f);
    CenterSpace::SetupLabel(this
                            , laLabel
                            , "LOOKAHEAD"
                            , resources.theme.textPrimary
                            , 11.0f);
    CenterSpace::SetupLabel(this
                            , focusLabel
                            , "FOCUS"
                            , resources.theme.textPrimary
                            , 11.0f);
    
    addAndMakeVisible(laSelector.get());
    
    auto ComboSetup = [&](juce::ComboBox &box, const juce::StringArray &items)
    {
        box.addItemList(items, 1);
        box.setJustificationType(juce::Justification::centred);
        box.setColour(juce::ComboBox::backgroundColourId, juce::Colour((juce::uint8)53, (juce::uint8)59, (juce::uint8)60, (juce::uint8)255));
        box.setColour(juce::ComboBox::arrowColourId,      juce::Colours::white);
        box.setColour(juce::ComboBox::outlineColourId,    juce::Colours::white);
        addAndMakeVisible(box);
    };
    // Item order must match the `focus` APVTS StringArray in PluginProcessor.cpp.
    ComboSetup(focusBox, juce::StringArray({"Full Range"
                                            , "Reduce Bass"
                                            , "Vocal"
                                            , "Kick"
                                            , "Bass"
                                            , "Transients"
                                            , "Low"
                                            , "Mid"
                                            , "High"}));

    compressAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (*resources.apvts, "compress", compressSlider);
    focusAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(*resources.apvts, "focus",    focusBox);
 
    CenterSpace::SetTip(compressSlider, "Turning up increases the Sidechain gain and ratio, and lowers the threshold.");
    laSelector->SetTooltip("4ms Lookahead on/off. (Reports latency to the host when on).");
    CenterSpace::SetTip(focusBox, "Filter the Sidechain to focus on specific parts of the signal.");
}

VibeDetection::~VibeDetection() {}

void VibeDetection::resized()
{
    VibeLayout::resized();
    
    compressLabel .setBounds(sliderLabelArea);
    compressSlider.setBounds(sliderArea);
    
    focusLabel.setBounds(slot1LabelArea);
    focusBox  .setBounds(slot1Area);
    
    laLabel    .setBounds(slot2LabelArea);
    laSelector->setBounds(slot2Area);
}
