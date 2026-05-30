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
    setOpaque(false);
    
    CenterSpace::SetupSlider(this
                             , compressSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
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
//        box.setLookAndFeel(&boxLnF);
        addAndMakeVisible(box);
    };
    ComboSetup(focusBox, juce::StringArray({"Full Range"
                                            , "Reduce Bass"
                                            , "Transient Focus"
                                            , "Lows"
                                            , "Low Mid"
                                            , "High Mid"
                                            , "High"
                                            , "Vocal Body"
                                            , "Vocal Clarity"
                                            , "Kick Thump"
                                            , "Kick Smack"
                                            , "Snare Thump"
                                            , "Snare Smack"
                                            , "Bass Body"
                                            , "Hats Range"}));
    
    compressAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (*resources.apvts, "compress", compressSlider);
    focusAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(*resources.apvts, "focus",    focusBox);
}

VibeDetection::~VibeDetection()
{
    compressSlider.setLookAndFeel(nullptr);
}

// TODO: Adjust layout
void VibeDetection::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(20);
    
    compressLabel .setBounds(bounds.removeFromTop(15));
    compressSlider.setBounds(bounds.removeFromTop(122));
    
    bounds.removeFromTop(20);
    laLabel.setBounds(bounds.removeFromTop(15));
    laSelector->setBounds(bounds.removeFromTop(30)
                                .withSizeKeepingCentre(141, 30));
    
    bounds.removeFromTop(20);
    focusLabel.setBounds(bounds.removeFromTop(15));
    focusBox.setBounds(bounds.removeFromTop(37)
                             .withSizeKeepingCentre(172, 37));
}
