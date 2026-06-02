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
: resources(resources)
, styleSelector(std::make_unique<StyleSelector>(resources, "style"))
{
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
    
    resources.apvts->addParameterListener("style",  this);
    
    ratioSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "ratio",   ratioSlider);
    kneeAttachment        = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "knee",    kneeSlider);
    atkSliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "attack",  atkSlider);
    relSliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "release", relSlider);

    // Must happen AFTER attachments are constructed — see PluginEditor.cpp.
    ratioSlider.textFromValueFunction = CenterSpace::SliderText::Ratio;
    kneeSlider .textFromValueFunction = CenterSpace::SliderText::Db;
    atkSlider  .textFromValueFunction = CenterSpace::SliderText::Ms;
    relSlider  .textFromValueFunction = CenterSpace::SliderText::Ms;
    ratioSlider.updateText();
    kneeSlider .updateText();
    atkSlider  .updateText();
    relSlider  .updateText();

    CenterSpace::SetTip(ratioSlider, "Compression ratio. 1:1 = no compression. Over 10:1 gets into the limiting range.");
    CenterSpace::SetTip(kneeSlider,  "Soft-knee width. 0 dB = hard knee. Wider knee = gentler transition past threshold.");
    CenterSpace::SetTip(atkSlider,   "How quickly the compressor clamps down once the sidechain crosses the threshold.");
    CenterSpace::SetTip(relSlider,   "How quickly the compressor recovers once the sidechain falls below threshold.");
    styleSelector->SetTooltip("Compressor character. VCA: modern, precise, transparent. Opto: smoother, slower.");
    
    Update();
}

TweakDynamics::~TweakDynamics()
{
    resources.apvts->removeParameterListener("style",  this);
}

void TweakDynamics::resized()
{
    TweakLayout::resized();
    
    ratioLabel .setBounds(sliderLabel1Area);
    ratioSlider.setBounds(slider1Area);
    kneeLabel  .setBounds(sliderLabel2Area);
    kneeSlider .setBounds(slider2Area);
    atkLabel   .setBounds(sliderLabel3Area);
    atkSlider  .setBounds(slider3Area);
    relLabel   .setBounds(sliderLabel4Area);
    relSlider  .setBounds(slider4Area);
    
    styleLabel    .setBounds(slot2LabelArea);
    styleSelector->setBounds(slot2Area);
}

void TweakDynamics::parameterChanged(const juce::String &paramId, float /*newValue*/)
{
    // Listener fires on whichever thread set the value (incl. audio thread via
    // automation). Hop to the message thread before touching Components.
    juce::Component::SafePointer<TweakDynamics> safeThis(this);
    juce::MessageManager::callAsync([safeThis, paramId]
    {
        if (!safeThis)
            return;
        
       if (paramId == "style")
           safeThis->Update();
    });
}

void TweakDynamics::Update()
{
    auto *styleRaw = resources.apvts->getRawParameterValue("style");
    jassert(styleRaw);
    if (!styleRaw)
        return;
    
    const bool isVCA = (int)styleRaw->load() == 0;
    if (isShowing())
    {
        kneeLabel .setVisible(isVCA);
        kneeSlider.setVisible(isVCA);
    }
}
