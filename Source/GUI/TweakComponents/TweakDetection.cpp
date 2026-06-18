/*
  ==============================================================================

 TweakDetection.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TweakDetection.h"

#include "GUI/GuiHelpers.h"
#include "GUI/Format.h"
#include "GUI/Selector.h"
#include "CenterSpaceHelpers.h"

//==============================================================================

TweakDetection::TweakDetection(GuiResources &resources)
: resources(resources)
, detectionSelector(std::make_unique<Selector>(*resources.apvts
                                               , "peakRMS"
                                               , resources.theme
                                               , juce::StringArray{"PEAK","RMS"}
                                               , CenterSpace::SelectorFontOptions))
, laSelector       (std::make_unique<Selector>(*resources.apvts
                                               , "lookahead"
                                               , resources.theme
                                               , juce::StringArray{"0","1","4","10"}
                                               , CenterSpace::SelectorFontOptions))
{
    GuiHelpers::SetupSlider(this
                             , scGainSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    GuiHelpers::SetupSlider(this
                             , threshSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    GuiHelpers::SetupSlider(this
                             , scHpfSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    GuiHelpers::SetupSlider(this
                             , scLpfSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);

    GuiHelpers::SetupLabel(this
                            , scGainLabel
                            , "SC Gain"
                            , resources.theme.textPrimary
                            , 12.0f);
    GuiHelpers::SetupLabel(this
                            , threshLabel
                            , "Threshold"
                            , resources.theme.textPrimary
                            , 12.0f);
    GuiHelpers::SetupLabel(this
                            , scHpfLabel
                            , "SC HPF"
                            , resources.theme.textPrimary
                            , 12.0f);
    GuiHelpers::SetupLabel(this
                            , scLpfLabel
                            , "SC LPF"
                            , resources.theme.textPrimary
                            , 12.0f);
    GuiHelpers::SetupLabel(this
                            , detectionLabel
                            , "DETECTION"
                            , resources.theme.textPrimary
                            , 12.0f);
    GuiHelpers::SetupLabel(this
                            , laLabel
                            , "LOOKAHEAD (ms)"
                            , resources.theme.textPrimary
                            , 12.0f);
    
    addChildComponent(detectionSelector.get());
    addAndMakeVisible(laSelector.get());
    
    resources.apvts->addParameterListener("style",  this);
    
    scGainSliderAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "sideInGain", scGainSlider);
    threshSliderAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "threshold",  threshSlider);
    scHpfAttachment         = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "scHpfHz",    scHpfSlider);
    scLpfAttachment         = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "scLpfHz",    scLpfSlider);

    // Must happen AFTER attachments are constructed — see PluginEditor.cpp.
    scGainSlider.textFromValueFunction = Format::Db;
    threshSlider.textFromValueFunction = Format::Db;
    scHpfSlider .textFromValueFunction = Format::Hz;
    scLpfSlider .textFromValueFunction = Format::Hz;
    scGainSlider.updateText();
    threshSlider.updateText();
    scHpfSlider .updateText();
    scLpfSlider .updateText();

    GuiHelpers::SetTip(scGainSlider, "Set Sidechain's input Gain.");
    GuiHelpers::SetTip(threshSlider, "Set compression Threshold. Sidechain signal above threshold triggers compressor.");
    GuiHelpers::SetTip(scHpfSlider,  "Sidechain HPF: Remove low frequencies from the sidechain signal before it hits the threshold.");
    GuiHelpers::SetTip(scLpfSlider,  "Sidechain LPF: Remove high frequencies from the sidechain signal before it hits the threshold.");
    detectionSelector->SetTooltip("Peak: max voltage (instantaneous); RMS: average power (slower, smoother).");
    laSelector       ->SetTooltip("Sidechain Lookahead in milliseconds. (Reports latency to the host)");

    Update();
}

TweakDetection::~TweakDetection()
{
    resources.apvts->removeParameterListener("style",  this);
}

void TweakDetection::resized()
{
    TweakLayout::resized();
    
    scGainLabel .setBounds(sliderLabel1Area);
    scGainSlider.setBounds(slider1Area);
    threshLabel .setBounds(sliderLabel2Area);
    threshSlider.setBounds(slider2Area);
    scHpfLabel  .setBounds(sliderLabel3Area);
    scHpfSlider .setBounds(slider3Area);
    scLpfLabel  .setBounds(sliderLabel4Area);
    scLpfSlider .setBounds(slider4Area);
    
    detectionLabel    .setBounds(slot1LabelArea);
    detectionSelector->setBounds(slot1Area);
    
    laLabel    .setBounds(slot2LabelArea);
    laSelector->setBounds(slot2Area);
}

void TweakDetection::parameterChanged(const juce::String &paramId, float /*newValue*/)
{
    // Listener fires on whichever thread set the value (incl. audio thread via
    // automation). Hop to the message thread before touching Components.
    juce::Component::SafePointer<TweakDetection> safeThis(this);
    juce::MessageManager::callAsync([safeThis, paramId]
    {
        if (!safeThis)
            return;
        
       if (paramId == "style")
           safeThis->Update();
    });
}

void TweakDetection::Update()
{
    auto *styleRaw = resources.apvts->getRawParameterValue("style");
    jassert(styleRaw);
    if (!styleRaw)
        return;
    
    const bool isVCA = (int)styleRaw->load() == 0;
    if (isShowing())
    {
        detectionLabel    .setVisible(isVCA);
        detectionSelector->setVisible(isVCA);
    }
}
