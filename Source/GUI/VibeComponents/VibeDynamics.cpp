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
    setOpaque(false);
    
    CenterSpace::SetupSlider(this
                             , reactSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
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
}

VibeDynamics::~VibeDynamics()
{
    reactSlider.setLookAndFeel(nullptr);
}

// TODO: Adjust layout
void VibeDynamics::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(20);
    
    reactLabel .setBounds(bounds.removeFromTop(15));
    reactSlider.setBounds(bounds.removeFromTop(122));
    
    bounds.removeFromTop(20);
    feelLabel.setBounds(bounds.removeFromTop(15));
    feelSelector->setBounds(bounds.removeFromTop(30).withSizeKeepingCentre(89, 30));
}
