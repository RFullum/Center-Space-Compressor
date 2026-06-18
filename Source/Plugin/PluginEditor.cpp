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
            .apvts              = &processor.parameters,
            .theme              =  Palette::DefaultTheme,
            .csLAndF            = &lAndF,
            .processor          = &processor,
            .getTooltipsEnabled = [&processor] { return processor.GetTooltipsEnabled(); },
            .setTooltipsEnabled = [&processor] (bool on) { processor.SetTooltipsEnabled(on); }
        };
    }

}   // namespace

//==============================================================================

CenterSpaceAudioProcessorEditor::CenterSpaceAudioProcessorEditor(CenterSpaceAudioProcessor &p)
: juce::AudioProcessorEditor(&p)
, csLAndF(std::make_unique<CSLookAndFeel>(Palette::DefaultTheme))
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

    // Attach our LookAndFeel at the editor level — every child component
    // (including sliders inside Tweak/Vibe sub-components) inherits it via
    // the component tree, so no per-slider setLookAndFeel call is needed.
    setLookAndFeel(csLAndF.get());

    resources.refreshTooltipWindow = [this] { UpdateTooltipWindow(); };

    UpdateTooltipWindow();
    
    addAndMakeVisible(titleHeader.get());
    addAndMakeVisible(titleFooter.get());
    
    addAndMakeVisible(inStereoSelector .get());
    addAndMakeVisible(outStereoSelector.get());
    GuiHelpers::SetupLabel(this
                            , inStereoLabel
                            , "In Stereo"
                            , resources.theme.textSecondary
                            , 11.0f);
    GuiHelpers::SetupLabel(this
                            , outStereoLabel
                            , "Out Stereo"
                            , resources.theme.textSecondary
                            , 11.0f);

    GuiHelpers::SetupSlider(this
                             , inGainSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.secondaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);
    GuiHelpers::SetupSlider(this
                             , outGainSlider
                             , juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag
                             , resources.theme.secondaryAccent
                             , juce::Colours::transparentBlack
                             , resources.theme.textPrimary);

    GuiHelpers::SetupLabel(this
                            , inGainLabel
                            , "In Gain"
                            , resources.theme.textSecondary
                            , 11.0f);
    GuiHelpers::SetupLabel(this
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

    // Override the SliderAttachment's default text formatter (which uses the
    // parameter's getText()) with our project-wide formatters. Must happen
    // AFTER the attachment is constructed — the attachment's ctor overwrites
    // textFromValueFunction with its own lambda.
    inGainSlider .textFromValueFunction = CenterSpace::SliderText::Db;
    outGainSlider.textFromValueFunction = CenterSpace::SliderText::Db;
    inGainSlider .updateText();
    outGainSlider.updateText();

    GuiHelpers::SetTip(inGainSlider,  "Set audio's Input Gain.");
    GuiHelpers::SetTip(outGainSlider, "Set audio's Output Gain.");
    inStereoSelector ->SetTooltip("Select LR if the source is in standard Left-Right stereo. Only pick M/S if the source is already Mid/Side encoded.");
    outStereoSelector->SetTooltip("Select LR to decode back to standard Left-Right stereo. Select M/S to leave the output Mid/Side encoded.");

    audioProcessor.parameters.addParameterListener("uiMode", this);

    Update();
}

CenterSpaceAudioProcessorEditor::~CenterSpaceAudioProcessorEditor()
{
    audioProcessor.parameters.removeParameterListener("uiMode", this);

    setLookAndFeel(nullptr);
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

void CenterSpaceAudioProcessorEditor::UpdateTooltipWindow()
{
    const bool want = resources.getTooltipsEnabled && resources.getTooltipsEnabled();

    if (want && tooltipWindow == nullptr)
    {
        // Non-opaque so CSLookAndFeel::drawTooltip's rounded background looks
        // rounded with no corners poking out.
        tooltipWindow = std::make_unique<juce::TooltipWindow>(this);
        tooltipWindow->setOpaque(false);
    }
    else if (! want && tooltipWindow != nullptr)
    {
        tooltipWindow.reset();
    }
}
