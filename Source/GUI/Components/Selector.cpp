/*
  ==============================================================================

    Selector.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "Selector.h"

//==============================================================================

SelectorButton::SelectorButton(juce::StringRef  name
                               , GuiResources  &resources)
: juce::Button("")
, name(name)
, resources(resources)
{
    setOpaque(false);
    IsSelected.on_change.connect([&](bool){ repaint(); } );
}

SelectorButton::~SelectorButton() {}

void SelectorButton::paintButton(juce::Graphics &g, bool, bool)
{
    auto bounds = getLocalBounds();
    g.setColour(IsSelected.get() ? resources.theme.primaryAccent : juce::Colours::transparentBlack);
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    
    g.setColour(IsSelected.get() ? resources.theme.background : resources.theme.textPrimary);
    g.setFont(juce::Font(juce::FontOptions("Helvetica"
                                           , 11.0f
                                           , juce::Font::plain)));
    g.drawText(name, bounds, juce::Justification::centred);
}

//==============================================================================

Selector::Selector(GuiResources &resources)
: resources(resources)
{
    setOpaque(false);
}

Selector::~Selector() {}

void Selector::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    
    g.setColour(resources.theme.structure.darker());
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    
    g.setColour(resources.theme.structure.brighter(0.1f));
    g.drawRoundedRectangle(bounds.toFloat(), 4.0f, 2.0f);
}

//==============================================================================

StereoSelector::StereoSelector(GuiResources            &resources
                               , const juce::StringRef  paramID)
: Selector(resources)
, lrButton(std::make_unique<SelectorButton>("LR",  resources))
, msButton(std::make_unique<SelectorButton>("M/S", resources))
{
    parameter = resources.apvts->getParameter(paramID);
    
    jassert(parameter);
    if (parameter)
    {
        attachment = std::make_unique<juce::ParameterAttachment>
        (*parameter
         , [this](float newVal)
         {
             if (auto *choice = dynamic_cast<juce::AudioParameterChoice*>(parameter))
                 Stereo.set(choice->getIndex() == 0
                                ? CenterSpace::StereoType::LeftRight
                                : CenterSpace::StereoType::MidSide);
             else
                 Stereo.set((int)newVal == 0
                            ? CenterSpace::StereoType::LeftRight
                            : CenterSpace::StereoType::MidSide);
         });
    }
        
    
    addAndMakeVisible(lrButton.get());
    addAndMakeVisible(msButton.get());
    lrButton->onClick = [&]{ Stereo.set(CenterSpace::StereoType::LeftRight); };
    msButton->onClick = [&]{ Stereo.set(CenterSpace::StereoType::MidSide);   };
    
    Stereo.on_change.connect(&StereoSelector::OnStereoChanged, this);
    OnStereoChanged(Stereo.get());
}

StereoSelector::~StereoSelector() {}

void StereoSelector::resized()
{
    auto bounds = getLocalBounds();
    auto left = bounds.removeFromLeft(bounds.proportionOfWidth(0.5f));
    lrButton->setBounds(left.reduced(3));
    msButton->setBounds(bounds.reduced(3));
}

void StereoSelector::OnStereoChanged(CenterSpace::StereoType type)
{
    lrButton->IsSelected.set(type == CenterSpace::StereoType::LeftRight);
    msButton->IsSelected.set(type == CenterSpace::StereoType::MidSide);
    if (!attachment)
        return;
    
    // LR = 0/false; M/S = 1/true
    attachment->setValueAsCompleteGesture(float(type == CenterSpace::StereoType::MidSide));
}

//==============================================================================

UIModeSelector::UIModeSelector(GuiResources            &resources
                               , const juce::StringRef  paramID)
: Selector(resources)
, vibeButton (std::make_unique<SelectorButton>("VIBE",  resources))
, tweakButton(std::make_unique<SelectorButton>("TWEAK", resources))
{
    parameter = resources.apvts->getParameter(paramID);
    
    jassert(parameter);
    if (parameter)
    {
        attachment = std::make_unique<juce::ParameterAttachment>
        (*parameter
         , [this](float newVal)
         {
            if (auto *choice = dynamic_cast<juce::AudioParameterChoice*>(parameter))
                UIMode.set(choice->getIndex() == 0
                            ? CenterSpace::UIModeType::Vibe
                            : CenterSpace::UIModeType::Tweak);
            else
                UIMode.set((int)newVal == 0
                            ? CenterSpace::UIModeType::Vibe
                            : CenterSpace::UIModeType::Tweak);
        });
    }
    
    addAndMakeVisible(vibeButton.get());
    addAndMakeVisible(tweakButton.get());
    vibeButton ->onClick = [&]{ UIMode.set(CenterSpace::UIModeType::Vibe);  };
    tweakButton->onClick = [&]{ UIMode.set(CenterSpace::UIModeType::Tweak); };
    
    UIMode.on_change.connect(&UIModeSelector::OnUIModeChanged, this);
    OnUIModeChanged(UIMode.get());
}

UIModeSelector::~UIModeSelector() {}

void UIModeSelector::resized()
{
    auto bounds = getLocalBounds();
    auto left = bounds.removeFromLeft(bounds.proportionOfWidth(0.5f));
    vibeButton ->setBounds(left.reduced(3));
    tweakButton->setBounds(bounds.reduced(3));
}

void UIModeSelector::OnUIModeChanged(CenterSpace::UIModeType type)
{
    vibeButton ->IsSelected.set(type == CenterSpace::UIModeType::Vibe);
    tweakButton->IsSelected.set(type == CenterSpace::UIModeType::Tweak);
    if (!attachment)
        return;
    
    // Vibe = 0/false; Tweak = 1/true
    attachment->setValueAsCompleteGesture(float(type == CenterSpace::UIModeType::Tweak));
}

//==============================================================================

LookaheadVibeSelector::LookaheadVibeSelector(GuiResources            &resources
                                             , const juce::StringRef  paramID)
: Selector(resources)
, offButton(std::make_unique<SelectorButton>("OFF", resources))
, onButton (std::make_unique<SelectorButton>("ON",  resources))
{
    parameter = resources.apvts->getParameter(paramID);
    
    jassert(parameter);
    if (parameter)
    {
        attachment = std::make_unique<juce::ParameterAttachment>
        (*parameter
         , [this](float newVal)
         {
            if (auto *choice = dynamic_cast<juce::AudioParameterChoice*>(parameter))
                LookaheadOn.set((bool)choice->getIndex());
            else
                LookaheadOn.set((bool)newVal);
        });
    }
    
    addAndMakeVisible(offButton.get());
    addAndMakeVisible(onButton.get());
    offButton->onClick = [&]{ LookaheadOn.set(false); };
    onButton ->onClick = [&]{ LookaheadOn.set(true);  };
    
    LookaheadOn.on_change.connect(&LookaheadVibeSelector::OnLookaheadOnChanged, this);
    OnLookaheadOnChanged(LookaheadOn.get());
}

LookaheadVibeSelector::~LookaheadVibeSelector() {}

void LookaheadVibeSelector::resized()
{
    auto bounds = getLocalBounds();
    auto left = bounds.removeFromLeft(bounds.proportionOfWidth(0.5f));
    offButton->setBounds(left.reduced(3));
    onButton ->setBounds(bounds.reduced(3));
}

void LookaheadVibeSelector::OnLookaheadOnChanged(bool isOn)
{
    offButton->IsSelected.set(!isOn);
    onButton ->IsSelected.set( isOn);
    if (!attachment)
        return;
    
    attachment->setValueAsCompleteGesture(float(isOn));
}

//==============================================================================

LookaheadTweakSelector::LookaheadTweakSelector(GuiResources            &resources
                                               , const juce::StringRef  paramID)
: Selector(resources)
, zeroButton(std::make_unique<SelectorButton>("0",  resources))
, oneButton (std::make_unique<SelectorButton>("1",  resources))
, fourButton(std::make_unique<SelectorButton>("4",  resources))
, tenButton (std::make_unique<SelectorButton>("10", resources))
{
    parameter = resources.apvts->getParameter(paramID);
    
    jassert(parameter);
    if (parameter)
    {
        attachment = std::make_unique<juce::ParameterAttachment>
        (*parameter
         , [this](float newVal)
         {
            if (auto *choice = dynamic_cast<juce::AudioParameterChoice*>(parameter))
            {
                const auto lookaheadTypeChoice = CenterSpace::LookaheadMsType(choice->getIndex());
                LookaheadMS.set(lookaheadTypeChoice);
            }
            else
            {
                const auto lookaheadTypeChoice = CenterSpace::LookaheadMsType((int)newVal);
                LookaheadMS.set(lookaheadTypeChoice);
            }
        });
    }
    
    addAndMakeVisible(zeroButton.get());
    addAndMakeVisible(oneButton.get());
    addAndMakeVisible(fourButton.get());
    addAndMakeVisible(tenButton.get());
    zeroButton->onClick = [&]{ LookaheadMS.set(CenterSpace::LookaheadMsType::Zero); };
    oneButton ->onClick = [&]{ LookaheadMS.set(CenterSpace::LookaheadMsType::One);  };
    fourButton->onClick = [&]{ LookaheadMS.set(CenterSpace::LookaheadMsType::Four); };
    tenButton ->onClick = [&]{ LookaheadMS.set(CenterSpace::LookaheadMsType::Ten);  };
    
    LookaheadMS.on_change.connect(&LookaheadTweakSelector::OnLookaheadMSChanged, this);
    OnLookaheadMSChanged(LookaheadMS.get());
}

LookaheadTweakSelector::~LookaheadTweakSelector() {}

void LookaheadTweakSelector::resized()
{
    auto bounds = getLocalBounds();
    const int buttonW = bounds.proportionOfWidth(0.25f);
    zeroButton->setBounds(bounds.removeFromLeft(buttonW));
    oneButton ->setBounds(bounds.removeFromLeft(buttonW));
    fourButton->setBounds(bounds.removeFromLeft(buttonW));
    tenButton ->setBounds(bounds);
}

void LookaheadTweakSelector::OnLookaheadMSChanged(CenterSpace::LookaheadMsType type)
{
    zeroButton->IsSelected.set(type == CenterSpace::LookaheadMsType::Zero);
    oneButton ->IsSelected.set(type == CenterSpace::LookaheadMsType::One);
    fourButton->IsSelected.set(type == CenterSpace::LookaheadMsType::Four);
    tenButton ->IsSelected.set(type == CenterSpace::LookaheadMsType::Ten);
    if (!attachment)
        return;
    
    const int typeIdx = (int)type;
    attachment->setValueAsCompleteGesture(float(typeIdx));
}

//==============================================================================

FeelSelector::FeelSelector(GuiResources            &resources
                           , const juce::StringRef  paramID)
: Selector(resources)
, cleanButton (std::make_unique<SelectorButton>("CLEAN",  resources))
, smoothButton(std::make_unique<SelectorButton>("SMOOTH", resources))
{
    parameter = resources.apvts->getParameter(paramID);
    
    jassert(parameter);
    if (parameter)
    {
        attachment = std::make_unique<juce::ParameterAttachment>
        (*parameter
         , [this](float newVal)
         {
            if (auto *choice = dynamic_cast<juce::AudioParameterChoice*>(parameter))
                Feel.set(choice->getIndex() == 0
                            ? CenterSpace::FeelType::Clean
                            : CenterSpace::FeelType::Smooth);
            else
                Feel.set((int)newVal == 0
                            ? CenterSpace::FeelType::Clean
                            : CenterSpace::FeelType::Smooth);
        });
    }
    
    addAndMakeVisible(cleanButton.get());
    addAndMakeVisible(smoothButton.get());
    cleanButton ->onClick = [&]{ Feel.set(CenterSpace::FeelType::Clean);  };
    smoothButton->onClick = [&]{ Feel.set(CenterSpace::FeelType::Smooth); };
    
    Feel.on_change.connect(&FeelSelector::OnFeelChanged, this);
    OnFeelChanged(Feel.get());
}

FeelSelector::~FeelSelector() {}

void FeelSelector::resized()
{
    auto bounds = getLocalBounds();
    auto left = bounds.removeFromLeft(bounds.proportionOfWidth(0.5f));
    cleanButton ->setBounds(left.reduced(3));
    smoothButton->setBounds(bounds.reduced(3));
}

void FeelSelector::OnFeelChanged(CenterSpace::FeelType type)
{
    cleanButton ->IsSelected.set(type == CenterSpace::FeelType::Clean);
    smoothButton->IsSelected.set(type == CenterSpace::FeelType::Smooth);
    if (!attachment)
        return;
    
    // Clean = 0/false; smooth = 1/true
    attachment->setValueAsCompleteGesture(float(type ==CenterSpace::FeelType::Smooth));
}

//==============================================================================

DetectionSelector::DetectionSelector(GuiResources            &resources
                                     , const juce::StringRef  paramID)
: Selector(resources)
, peakButton(std::make_unique<SelectorButton>("PEAK", resources))
, rmsButton (std::make_unique<SelectorButton>("RMS",  resources))
{
    parameter = resources.apvts->getParameter(paramID);
    
    jassert(parameter);
    if (parameter)
    {
        attachment = std::make_unique<juce::ParameterAttachment>
        (*parameter
         , [this](float newVal)
         {
            if (auto *choice = dynamic_cast<juce::AudioParameterChoice*>(parameter))
                Detection.set(choice->getIndex() == 0
                                ? CenterSpace::DetectionType::Peak
                                : CenterSpace::DetectionType::RMS);
            else
                Detection.set((int)newVal == 0
                                ? CenterSpace::DetectionType::Peak
                                : CenterSpace::DetectionType::RMS);
        });
    }
    
    addAndMakeVisible(peakButton.get());
    addAndMakeVisible(rmsButton.get());
    peakButton->onClick = [&]{ Detection.set(CenterSpace::DetectionType::Peak); };
    rmsButton ->onClick = [&]{ Detection.set(CenterSpace::DetectionType::RMS);  };
    
    Detection.on_change.connect(&DetectionSelector::OnDetectionChanged, this);
    OnDetectionChanged(Detection.get());
}

DetectionSelector::~DetectionSelector() {}

void DetectionSelector::resized()
{
    auto bounds = getLocalBounds();
    auto left = bounds.removeFromLeft(bounds.proportionOfWidth(0.5f));
    peakButton->setBounds(left.reduced(3));
    rmsButton ->setBounds(bounds.reduced(3));
}

void DetectionSelector::OnDetectionChanged(CenterSpace::DetectionType type)
{
    peakButton->IsSelected.set(type == CenterSpace::DetectionType::Peak);
    rmsButton ->IsSelected.set(type == CenterSpace::DetectionType::RMS);
    
    if (!attachment)
        return;
    
    // Peak = 0/false; RMS = 1/true
    attachment->setValueAsCompleteGesture(float(type == CenterSpace::DetectionType::RMS));
}
