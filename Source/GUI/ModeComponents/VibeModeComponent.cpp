/*
  ==============================================================================

    VibeModeComponent.cpp

  ==============================================================================
*/

#include "VibeModeComponent.h"


//==============================================================================

VibeModeComponent::VibeModeComponent(juce::AudioProcessorValueTreeState &apvts
                                     , OtherLookAndFeel                 &dialLookAndFeel
                                     , BoxLookAndFeel                   &comboLookAndFeel)
: dialLnF(dialLookAndFeel)
, boxLnF (comboLookAndFeel)
{
    SliderSetup(compressSlider, dialLnF);
    SliderSetup(reactSlider,    dialLnF);

    ComboSetup(feelBox,  juce::StringArray({"Clean", "Smooth"}));
    ComboSetup(focusBox, juce::StringArray({"Full Range"
                                            , "Reduce Bass"
                                            , "Transient Focus"
                                            , "Lows"
                                            , "Low Mid"
                                            , "High Mid"
                                            , "High"
                                            , "Vocal Body"
                                            , "Vocal Clarity"
                                            , "Kick Thump"
                                            , "Kick Smack"
                                            , "Snare Thump"
                                            , "Snare Smack"
                                            , "Bass Body"
                                            , "Hats Range"}));

    lookaheadOnOffButton.setButtonText({});
    lookaheadOnOffButton.setColour(juce::ToggleButton::textColourId,         juce::Colours::white);
    lookaheadOnOffButton.setColour(juce::ToggleButton::tickColourId,         juce::Colours::white);
    lookaheadOnOffButton.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::grey);
    addAndMakeVisible(lookaheadOnOffButton);

    LabelSetup(feelLabel,             "Feel");
    LabelSetup(compressLabel,         "Compress");
    LabelSetup(reactLabel,            "React");
    LabelSetup(focusLabel,            "Focus");
    LabelSetup(lookaheadOnOffLabel,   "Lookahead");

    feelAttachment           = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "feel",           feelBox);
    compressAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (apvts, "compress",       compressSlider);
    reactAttachment          = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>  (apvts, "react",          reactSlider);
    focusAttachment          = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "focus",          focusBox);
    lookaheadOnOffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>  (apvts, "lookaheadOnOff", lookaheadOnOffButton);
}

VibeModeComponent::~VibeModeComponent()
{
    compressSlider.setLookAndFeel(nullptr);
    reactSlider.setLookAndFeel   (nullptr);
    feelBox.setLookAndFeel       (nullptr);
    focusBox.setLookAndFeel      (nullptr);
}

void VibeModeComponent::paint(juce::Graphics &) {}

void VibeModeComponent::resized()
{
    auto bounds = getLocalBounds().reduced(6);

    const int cellW  = bounds.getWidth() / 5;
    const int labelH = 22;

    auto feelCell = bounds.removeFromLeft(cellW).reduced(4, 0);
    feelLabel.setBounds(feelCell.removeFromTop(labelH));
    feelBox  .setBounds(feelCell.removeFromTop(28));

    auto compCell = bounds.removeFromLeft(cellW).reduced(4, 0);
    compressLabel .setBounds(compCell.removeFromTop(labelH));
    compressSlider.setBounds(compCell);

    auto reactCell = bounds.removeFromLeft(cellW).reduced(4, 0);
    reactLabel .setBounds(reactCell.removeFromTop(labelH));
    reactSlider.setBounds(reactCell);

    auto focusCell = bounds.removeFromLeft(cellW).reduced(4, 0);
    focusLabel.setBounds(focusCell.removeFromTop(labelH));
    focusBox  .setBounds(focusCell.removeFromTop(28));

    auto laCell = bounds.reduced(4, 0);
    lookaheadOnOffLabel .setBounds(laCell.removeFromTop(labelH));
    lookaheadOnOffButton.setBounds(laCell.removeFromTop(28).withSizeKeepingCentre(28, 28));
}

void VibeModeComponent::SliderSetup(juce::Slider &slider, juce::LookAndFeel &lf)
{
    slider.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour((juce::uint8)0, (juce::uint8)0, (juce::uint8)0, (juce::uint8)0));
    slider.setColour(juce::Slider::textBoxTextColourId,    juce::Colours::white);
    slider.setLookAndFeel(&lf);
    addAndMakeVisible(slider);
}

void VibeModeComponent::LabelSetup(juce::Label &label, const juce::String &text)
{
    label.setText             (text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour           (juce::Label::textColourId, juce::Colours::white);
    label.setFont             (juce::Font("futura", 16.0f, 0));
    addAndMakeVisible(label);
}

void VibeModeComponent::ComboSetup(juce::ComboBox &box, const juce::StringArray &items)
{
    box.addItemList(items, 1);
    box.setJustificationType(juce::Justification::centred);
    box.setColour(juce::ComboBox::backgroundColourId, juce::Colour((juce::uint8)53, (juce::uint8)59, (juce::uint8)60, (juce::uint8)255));
    box.setColour(juce::ComboBox::arrowColourId,      juce::Colours::white);
    box.setColour(juce::ComboBox::outlineColourId,    juce::Colours::white);
    box.setLookAndFeel(&boxLnF);
    addAndMakeVisible(box);
}
