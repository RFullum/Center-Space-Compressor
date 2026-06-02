/*
  ==============================================================================

 TweakDetection.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TweakDetection.h"

#include "GuiHelpers.h"
#include "Selector.h"

//==============================================================================

TweakDetection::TweakDetection(GuiResources &resources)
: resources(resources)
, detectionSelector(std::make_unique<DetectionSelector>     (resources, "peakRMS"))
, laSelector       (std::make_unique<LookaheadTweakSelector>(resources, "lookahead"))
{
    setOpaque(false);
    
    CenterSpace::SetupSlider(this
                             , scGainSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    CenterSpace::SetupSlider(this
                             , threshSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    CenterSpace::SetupSlider(this
                             , scHpfSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    CenterSpace::SetupSlider(this
                             , scLpfSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.primaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);

    CenterSpace::SetupLabel(this
                            , scGainLabel
                            , "SC Gain"
                            , resources.theme.textPrimary
                            , 12.0f);
    CenterSpace::SetupLabel(this
                            , threshLabel
                            , "Threshold"
                            , resources.theme.textPrimary
                            , 12.0f);
    CenterSpace::SetupLabel(this
                            , scHpfLabel
                            , "SC HPF"
                            , resources.theme.textPrimary
                            , 12.0f);
    CenterSpace::SetupLabel(this
                            , scLpfLabel
                            , "SC LPF"
                            , resources.theme.textPrimary
                            , 12.0f);
    CenterSpace::SetupLabel(this
                            , detectionLabel
                            , "DETECTION"
                            , resources.theme.textPrimary
                            , 12.0f);
    CenterSpace::SetupLabel(this
                            , laLabel
                            , "LOOKAHEAD (ms)"
                            , resources.theme.textPrimary
                            , 12.0f);
    
    // TODO: just add, not visible. (VCA/Opto style switches)
    addAndMakeVisible(detectionSelector.get());
    addAndMakeVisible(laSelector.get());
    
    resources.apvts->addParameterListener("style",  this);
    
    scGainSliderAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "sideInGain", scGainSlider);
    threshSliderAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "threshold",  threshSlider);
    scHpfAttachment         = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "scHpfHz",    scHpfSlider);
    scLpfAttachment         = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*resources.apvts, "scLpfHz",    scLpfSlider);

    // Must happen AFTER attachments are constructed — see PluginEditor.cpp.
    scGainSlider.textFromValueFunction = CenterSpace::SliderText::Db;
    threshSlider.textFromValueFunction = CenterSpace::SliderText::Db;
    scHpfSlider .textFromValueFunction = CenterSpace::SliderText::Hz;
    scLpfSlider .textFromValueFunction = CenterSpace::SliderText::Hz;
    scGainSlider.updateText();
    threshSlider.updateText();
    scHpfSlider .updateText();
    scLpfSlider .updateText();

    CenterSpace::SetTip(scGainSlider, "Set Sidechain's input Gain.");
    CenterSpace::SetTip(threshSlider, "Set compression Threshold. Sidechain signal above threshold triggers compressor.");
    CenterSpace::SetTip(scHpfSlider,  "Sidechain HPF: Remove low frequencies from the sidechain signal before it hits the threshold.");
    CenterSpace::SetTip(scLpfSlider,  "Sidechain LPF: Remove high frequencies from the sidechain signal before it hits the threshold.");
    detectionSelector->SetTooltip("Peak: max voltage (instantaneous); RMS: average power (slower, smoother).");
    laSelector       ->SetTooltip("Sidechain Lookahead in milliseconds. (Reports latency to the host)");

    Update();
}

TweakDetection::~TweakDetection()
{
    scGainSlider.setLookAndFeel(nullptr);
    threshSlider.setLookAndFeel(nullptr);
    scHpfSlider .setLookAndFeel(nullptr);
    scLpfSlider .setLookAndFeel(nullptr);
    
    resources.apvts->removeParameterListener("style",  this);
}

void TweakDetection::resized()
{
    static constexpr int labelH = 16;
    
    auto bounds = getLocalBounds();
    bounds.removeFromTop(20);
    
    auto slidersArea = bounds.removeFromTop(222)
                             .withSizeKeepingCentre(150, 222);
    auto topRow      = slidersArea.removeFromTop(slidersArea.proportionOfHeight(0.5f));
    auto scGainArea  = topRow.removeFromLeft(topRow.proportionOfWidth(0.5f));
    scGainLabel .setBounds(scGainArea.removeFromTop(labelH));
    scGainSlider.setBounds(scGainArea);
    threshLabel .setBounds(topRow.removeFromTop(labelH));
    threshSlider.setBounds(topRow);
    
    auto hpfArea = slidersArea.removeFromLeft(slidersArea.proportionOfWidth(0.5f));
    scHpfLabel .setBounds(hpfArea.removeFromTop(labelH));
    scHpfSlider.setBounds(hpfArea);
    scLpfLabel .setBounds(slidersArea.removeFromTop(labelH));
    scLpfSlider.setBounds(slidersArea);
    
    bounds.removeFromTop(20);
    detectionLabel.setBounds(bounds.removeFromTop(labelH));
    detectionSelector->setBounds(bounds.removeFromTop(30)
                                       .withSizeKeepingCentre(141, 30));
    bounds.removeFromTop(20);
    laLabel.setBounds(bounds.removeFromTop(labelH));
    laSelector->setBounds(bounds.removeFromTop(30)
                                .withSizeKeepingCentre(141, 30));
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
