/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "TitleHeader.h"
#include "TitleFooter.h"
#include "Selector.h"
#include "CSLookAndFeel.h"
#include "VibeDynamics.h"
#include "VibeDetection.h"
#include "TweakDynamics.h"
#include "TweakDetection.h"
#include "Metering.h"

//==============================================================================

namespace
{

    GuiResources MakeResources(CenterSpaceAudioProcessor &processor
                               , juce::LookAndFeel       &lAndF)
    {
        return GuiResources
        {
            .apvts     = &processor.parameters,
            .theme     =  Palette::DefaultTheme,
            .csLAndF   = &lAndF,
            .processor = &processor
        };
    }

}   // namespace

//==============================================================================

CenterSpaceAudioProcessorEditor::CenterSpaceAudioProcessorEditor(CenterSpaceAudioProcessor &p)
: juce::AudioProcessorEditor(&p)
, csLAndF(std::make_unique<CSLookAndFeel>())
, resources(MakeResources(p, *csLAndF))
, titleHeader(std::make_unique<TitleHeader>(resources))
, titleFooter(std::make_unique<TitleFooter>(resources))
, inStereoSelector (std::make_unique<StereoSelector>(resources, "inputType"))
, outStereoSelector(std::make_unique<StereoSelector>(resources, "outputType"))
, vibeDetection (std::make_unique<VibeDetection> (resources))
, vibeDynamics  (std::make_unique<VibeDynamics>  (resources))
, tweakDetection(std::make_unique<TweakDetection>(resources))
, tweakDynamics (std::make_unique<TweakDynamics> (resources))
, metering(std::make_unique<Metering>(resources))
, audioProcessor(p)
{
    setSize(1280, 720);
    
    csLAndF->SetTrackBackground(resources.theme.structure);
    
    addAndMakeVisible(titleHeader.get());
    addAndMakeVisible(titleFooter.get());
    
    addAndMakeVisible(inStereoSelector .get());
    addAndMakeVisible(outStereoSelector.get());
    CenterSpace::SetupLabel(this
                            , inStereoLabel
                            , "In Stereo"
                            , resources.theme.textSecondary
                            , 11.0f);
    CenterSpace::SetupLabel(this
                            , outStereoLabel
                            , "Out Stereo"
                            , resources.theme.textSecondary
                            , 11.0f);

    CenterSpace::SetupSlider(this
                             , inGainSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.secondaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    CenterSpace::SetupSlider(this
                             , outGainSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.secondaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    
    CenterSpace::SetupLabel(this
                            , inGainLabel
                            , "In Gain"
                            , resources.theme.textSecondary
                            , 11.0f);
    CenterSpace::SetupLabel(this
                            , outGainLabel
                            , "Out Gain"
                            , resources.theme.textSecondary
                            , 11.0f);
    
    addChildComponent(vibeDetection.get());
    addChildComponent(vibeDynamics.get());
    addChildComponent(tweakDetection.get());
    addChildComponent(tweakDynamics.get());
    
    addAndMakeVisible(metering.get());
    
    inGainSliderAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "inGain",     inGainSlider);
    outGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "outGain",    outGainSlider);

    audioProcessor.parameters.addParameterListener("uiMode", this);

    Update();
}

CenterSpaceAudioProcessorEditor::~CenterSpaceAudioProcessorEditor()
{
    audioProcessor.parameters.removeParameterListener("uiMode", this);
    
    inGainSlider.setLookAndFeel (nullptr);
    outGainSlider.setLookAndFeel(nullptr);
}

void CenterSpaceAudioProcessorEditor::paint(juce::Graphics &g)
{
    static constexpr float separatorThickness = 2.0f;
    static constexpr float separatorMargin    = 20.0f;
    g.fillAll(resources.theme.background);
    
    auto bounds = getLocalBounds();
    
    const auto headerBounds    = titleHeader->getBounds().toFloat();
    const auto headBottomLeft  = headerBounds.getBottomLeft();
    const auto headBottomRight = headerBounds.getBottomRight();
    const auto headBottomY     = headerBounds.getBottom();
    
    auto line = juce::Line<float>(headBottomLeft, headBottomRight);
    
    g.setColour(resources.theme.structure);
    g.drawLine(line, separatorThickness);
    
    const float separatorLeftX = bounds.toFloat().proportionOfWidth(0.25f);
    line = juce::Line<float>(separatorLeftX
                             , headBottomY + separatorMargin
                             , separatorLeftX
                             , bounds.toFloat().getBottom() - separatorMargin);
    g.drawLine(line, separatorThickness);
    
    const float separatorRightX = bounds.toFloat().proportionOfWidth(0.75f);
    line = juce::Line<float>(separatorRightX
                             , headBottomY + separatorMargin
                             , separatorRightX
                             , bounds.toFloat().getBottom() - separatorMargin);
    g.drawLine(line, separatorThickness);
}

void CenterSpaceAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    titleHeader->setBounds(bounds.removeFromTop(80));
    titleFooter->setBounds(bounds.removeFromBottom(15));
    
    const auto panelSize = bounds.proportionOfWidth(0.25f);
    auto leftPanel  = bounds.removeFromLeft(panelSize);
    auto rightPanel = bounds.removeFromRight(panelSize);
    
    leftPanel .removeFromTop(10);
    rightPanel.removeFromTop(10);
    
    inStereoLabel .setBounds(leftPanel .removeFromTop(30));
    outStereoLabel.setBounds(rightPanel.removeFromTop(30));
    inStereoSelector ->setBounds(leftPanel .removeFromTop(30).withSizeKeepingCentre(88, 30));
    outStereoSelector->setBounds(rightPanel.removeFromTop(30).withSizeKeepingCentre(88, 30));
    
    leftPanel      .removeFromTop(22);
    rightPanel     .removeFromTop(22);
    inGainLabel    .setBounds(leftPanel .removeFromTop(15));
    outGainLabel   .setBounds(rightPanel.removeFromTop(15));
    inGainSlider   .setBounds(leftPanel .removeFromTop(100));
    outGainSlider  .setBounds(rightPanel.removeFromTop(100));
    vibeDetection ->setBounds(leftPanel);
    tweakDetection->setBounds(leftPanel);
    vibeDynamics  ->setBounds(rightPanel);
    tweakDynamics ->setBounds(rightPanel);
    
    metering->setBounds(bounds);
}

void CenterSpaceAudioProcessorEditor::parameterChanged(const juce::String &paramId, float /*newValue*/)
{
    // Listener fires on whichever thread set the value (incl. audio thread via
    // automation). Hop to the message thread before touching Components.
    juce::Component::SafePointer<CenterSpaceAudioProcessorEditor> safeThis(this);
    juce::MessageManager::callAsync([safeThis, paramId]
    {
        if (!safeThis)
            return;

        if (paramId == "uiMode")
            safeThis->Update();
    });
}

void CenterSpaceAudioProcessorEditor::Update()
{
    auto *uiModeRaw = audioProcessor.parameters.getRawParameterValue("uiMode");
    jassert(uiModeRaw);
    if (!uiModeRaw)
        return;
    
    const bool isTweak = (uiModeRaw != nullptr) && ((int)uiModeRaw->load() == 1);

    vibeDetection ->setVisible(!isTweak);
    vibeDynamics  ->setVisible(!isTweak);
    tweakDetection->setVisible( isTweak);
    tweakDynamics ->setVisible( isTweak);
}
