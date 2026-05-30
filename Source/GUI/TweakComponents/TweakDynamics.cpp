/*
  ==============================================================================

 TweakDynamics.h
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TweakDynamics.h"

#include "GuiHelpers.h"
#include "Selector.h"

//==============================================================================

TweakDynamics::TweakDynamics(GuiResources &resources)
: styleSelector(std::make_unique<StyleSelector>(resources, "style"))
{
    setOpaque(false);
    
    CenterSpace::SetupSlider(this
                             , ratioSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    CenterSpace::SetupSlider(this
                             , kneeSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    CenterSpace::SetupSlider(this
                             , atkSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    CenterSpace::SetupSlider(this
                             , relSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    
    CenterSpace::SetupLabel(this
                            , ratioLabel
                            , "Ratio"
                            , resources.theme.textPrimary
                            , 12.0f);
    CenterSpace::SetupLabel(this
                            , kneeLabel
                            , "Knee"
                            , resources.theme.textPrimary
                            , 12.0f);
    CenterSpace::SetupLabel(this
                            , atkLabel
                            , "Attack"
                            , resources.theme.textPrimary
                            , 12.0f);
    CenterSpace::SetupLabel(this
                            , relLabel
                            , "Release"
                            , resources.theme.textPrimary
                            , 12.0f);
    CenterSpace::SetupLabel(this
                            , styleLabel
                            , "STYLE"
                            , resources.theme.textPrimary
                            , 12.0f);
    
    
    addAndMakeVisible(styleSelector.get());
    
    ratioSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "ratio",   ratioSlider);
    kneeAttachment        = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "knee",    kneeSlider);
    atkSliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "attack",  atkSlider);
    relSliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "release", relSlider);
}

TweakDynamics::~TweakDynamics()
{
    ratioSlider.setLookAndFeel(nullptr);
    kneeSlider .setLookAndFeel(nullptr);
    atkSlider  .setLookAndFeel(nullptr);
    relSlider  .setLookAndFeel(nullptr);
}

void TweakDynamics::resized()
{
    static constexpr int labelH = 16;
    
    auto bounds = getLocalBounds();
    bounds.removeFromTop(20);
    
    auto slidersArea = bounds.removeFromTop(222)
                             .withSizeKeepingCentre(150, 222);
    auto topRow      = slidersArea.removeFromTop(slidersArea.proportionOfHeight(0.5f));
    auto ratioArea   = topRow.removeFromLeft(topRow.proportionOfWidth(0.5f));
    ratioLabel .setBounds(ratioArea.removeFromTop(labelH));
    ratioSlider.setBounds(ratioArea);
    kneeLabel  .setBounds(topRow.removeFromTop(labelH));
    kneeSlider .setBounds(topRow);
    
    auto atkArea = slidersArea.removeFromLeft(slidersArea.proportionOfWidth(0.5f));
    atkLabel .setBounds(atkArea.removeFromTop(labelH));
    atkSlider.setBounds(atkArea);
    relLabel .setBounds(slidersArea.removeFromTop(labelH));
    relSlider.setBounds(slidersArea);
    
    bounds.removeFromTop(20);
    styleLabel.setBounds(bounds.removeFromTop(labelH));
    styleSelector->setBounds(bounds.removeFromTop(30)
                                   .withSizeKeepingCentre(141, 30));
}
