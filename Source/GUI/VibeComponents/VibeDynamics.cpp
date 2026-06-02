/*
  ==============================================================================

 VibeDynamics.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "VibeDynamics.h"

#include "GuiHelpers.h"
#include "Selector.h"

//==============================================================================

VibeDynamics::VibeDynamics(GuiResources &resources)
: feelSelector(std::make_unique<FeelSelector>(resources, "feel"))
{
    CenterSpace::SetupSlider(this
                             , reactSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    reactSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    CenterSpace::SetupLabel(this
                            , reactLabel
                            , "React"
                            , resources.theme.textPrimary
                            , 15.0f);
    CenterSpace::SetupLabel(this
                            , feelLabel
                            , "FEEL"
                            , resources.theme.textPrimary
                            , 11.0f);
    
    addAndMakeVisible(feelSelector.get());
    
    reactAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "react", reactSlider);

    CenterSpace::SetTip(reactSlider, "Reaction time: Left = fast attack & release; right = slow attack & release.");
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
