/*
  ==============================================================================

    TweakModeComponent.cpp

  ==============================================================================
*/

#include "TweakModeComponent.h"


//==============================================================================

TweakModeComponent::TweakModeComponent(juce::AudioProcessorValueTreeState &apvts
                                       , OtherLookAndFeel                 &dialLookAndFeel
                                       , BoxLookAndFeel                   &comboLookAndFeel)
: dialLnF(dialLookAndFeel)
, boxLnF (comboLookAndFeel)
{
    auto SliderSetup = [&](juce::Slider &slider, juce::LookAndFeel &lf)
    {
        slider.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour((juce::uint8)0, (juce::uint8)0, (juce::uint8)0, (juce::uint8)0));
        slider.setColour(juce::Slider::textBoxTextColourId,    juce::Colours::white);
        slider.setLookAndFeel(&lf);
        addAndMakeVisible(slider);
    };
    SliderSetup(scHpfSlider, dialLnF);
    SliderSetup(scLpfSlider, dialLnF);
    SliderSetup(kneeSlider,  dialLnF);

    auto ComboSetup = [&](juce::ComboBox &box, const juce::StringArray &items)
    {
        box.addItemList(items, 1);
        box.setJustificationType(juce::Justification::centred);
        box.setColour(juce::ComboBox::backgroundColourId, juce::Colour((juce::uint8)53, (juce::uint8)59, (juce::uint8)60, (juce::uint8)255));
        box.setColour(juce::ComboBox::arrowColourId,      juce::Colours::white);
        box.setColour(juce::ComboBox::outlineColourId,    juce::Colours::white);
        box.setLookAndFeel(&boxLnF);
        addAndMakeVisible(box);
    };
    ComboSetup(styleBox,     juce::StringArray({"Modern VCA", "Opto"}));
    ComboSetup(lookaheadBox, juce::StringArray({"0 ms", "1 ms", "4 ms", "10 ms"}));

    auto LabelSetup = [&](juce::Label &label, const juce::String &text)
    {
        label.setText             (text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour           (juce::Label::textColourId, juce::Colours::white);
        label.setFont             (juce::Font("futura", 16.0f, 0));
        addAndMakeVisible(label);
    };
    LabelSetup(scHpfLabel,     "SC HPF Hz");
    LabelSetup(scLpfLabel,     "SC LPF Hz");
    LabelSetup(kneeLabel,      "Knee dB");
    LabelSetup(styleLabel,     "Style");
    LabelSetup(lookaheadLabel, "Lookahead");

    scHpfAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (apvts, "scHpfHz",   scHpfSlider);
    scLpfAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (apvts, "scLpfHz",   scLpfSlider);
    kneeAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (apvts, "knee",      kneeSlider);
    styleAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "style",     styleBox);
    lookaheadAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "lookahead", lookaheadBox);
}

TweakModeComponent::~TweakModeComponent()
{
    scHpfSlider .setLookAndFeel(nullptr);
    scLpfSlider .setLookAndFeel(nullptr);
    kneeSlider  .setLookAndFeel(nullptr);
    styleBox    .setLookAndFeel(nullptr);
    lookaheadBox.setLookAndFeel(nullptr);
}

void TweakModeComponent::paint(juce::Graphics &) {}

void TweakModeComponent::resized()
{
    auto bounds = getLocalBounds().reduced(6);

    const int cellW = bounds.getWidth() / 5;
    const int labelH = 22;

    auto layoutCell = [&] (juce::Component &control, juce::Label &label)
    {
        auto cell = bounds.removeFromLeft(cellW).reduced(4, 0);
        label.setBounds  (cell.removeFromTop(labelH));
        control.setBounds(cell);
    };

    layoutCell(scHpfSlider, scHpfLabel);
    layoutCell(scLpfSlider, scLpfLabel);
    layoutCell(kneeSlider,  kneeLabel);

    auto styleCell = bounds.removeFromLeft(cellW).reduced(4, 0);
    styleLabel.setBounds(styleCell.removeFromTop(labelH));
    styleBox  .setBounds(styleCell.removeFromTop(28));

    auto laCell = bounds.reduced(4, 0);
    lookaheadLabel.setBounds(laCell.removeFromTop(labelH));
    lookaheadBox  .setBounds(laCell.removeFromTop(28));
}

void TweakModeComponent::SetKneeVisible(bool shouldBeVisible)
{
    kneeSlider.setVisible(shouldBeVisible);
    kneeLabel .setVisible(shouldBeVisible);
}
