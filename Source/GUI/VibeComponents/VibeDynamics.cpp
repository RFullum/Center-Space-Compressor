/*
  ==============================================================================

 VibeDynamics.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "VibeDynamics.h"

#include "GUI/GuiHelpers.h"
#include "GUI/Selector.h"
#include "CenterSpaceHelpers.h"

//==============================================================================

VibeDynamics::VibeDynamics(GuiResources &resources)
: feelSelector(std::make_unique<Selector>(*resources.apvts
                                          , "feel"
                                          , resources.theme
                                          , juce::StringArray{"CLEAN","SMOOTH"}
                                          , CenterSpace::SelectorFontOptions))
{
    GuiHelpers::SetupSlider(this
                             , reactSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    reactSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    GuiHelpers::SetupLabel(this
                            , reactLabel
                            , "React"
                            , resources.theme.textPrimary
                            , 15.0f);
    GuiHelpers::SetupLabel(this
                            , feelLabel
                            , "FEEL"
                            , resources.theme.textPrimary
                            , 11.0f);
    
    addAndMakeVisible(feelSelector.get());
    
    reactAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "react", reactSlider);

    GuiHelpers::SetTip(reactSlider, "Reaction time: Left = fast attack & release; right = slow attack & release.");
    feelSelector->SetTooltip("Compressor character: Clean = Peak + Modern + Hard Knee; Smooth = RMS + Vintage + Soft Knee.");
}

VibeDynamics::~VibeDynamics() {}

void VibeDynamics::resized()
{
    VibeLayout::resized();
    
    reactLabel .setBounds(sliderLabelArea);
    reactSlider.setBounds(sliderArea);
    
    feelLabel    .setBounds(slot2LabelArea);
    feelSelector->setBounds(slot2Area);
}
