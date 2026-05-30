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

void StereoSelector::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    
    g.setColour(resources.theme.structure.darker());
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    
    g.setColour(resources.theme.structure.brighter(0.1f));
    g.drawRoundedRectangle(bounds.toFloat(), 4.0f, 2.0f);
}

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
    if (attachment)
    {
        // LR = 0/false; M/S = 1/true
        attachment->setValueAsCompleteGesture(float(type == CenterSpace::StereoType::MidSide));
    }
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

void UIModeSelector::paint(juce::Graphics &g)
{
    auto bounds = getLocalBounds();
    
    g.setColour(resources.theme.structure.darker());
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    
    g.setColour(resources.theme.structure.brighter(0.1f));
    g.drawRoundedRectangle(bounds.toFloat(), 4.0f, 2.0f);
}

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
    if (attachment)
    {
        // Vibe = 0/false; Tweak = 1/true
        attachment->setValueAsPartOfGesture(float(type == CenterSpace::UIModeType::Tweak));
    }
}
