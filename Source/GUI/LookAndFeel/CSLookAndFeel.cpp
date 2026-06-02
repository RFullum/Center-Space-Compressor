/*
  ==============================================================================

     CSLookAndFeel.cpp

  ==============================================================================
*/

#include "CSLookAndFeel.h"

//==============================================================================

CSLookAndFeel::CSLookAndFeel(const Palette::Theme &themeRef)
: theme(themeRef)
{
    setColour(juce::ComboBox::backgroundColourId,     theme.structure);
    setColour(juce::ComboBox::textColourId,           theme.textPrimary);
    setColour(juce::ComboBox::outlineColourId,        theme.textSecondary.withAlpha(0.35f));
    setColour(juce::ComboBox::arrowColourId,          theme.textSecondary);
    setColour(juce::ComboBox::focusedOutlineColourId, theme.primaryAccent);
    setColour(juce::ComboBox::buttonColourId,         juce::Colours::transparentBlack);

    // PopupMenu — alpha < 1.0 flips isOpaque() to false so JUCE's MenuWindow
    // skips its corner-clobbering fillAll. Stay under ~0.998f (byte 255 would
    // round opaque).
    setColour(juce::PopupMenu::backgroundColourId,            theme.background.withAlpha(0.99f));
    setColour(juce::PopupMenu::textColourId,                  theme.textPrimary);
    setColour(juce::PopupMenu::headerTextColourId,            theme.textSecondary);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, theme.primaryAccent.withAlpha(0.20f));
    setColour(juce::PopupMenu::highlightedTextColourId,       theme.textPrimary);

    setColour(juce::TooltipWindow::backgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::TooltipWindow::textColourId,       theme.textPrimary);
    setColour(juce::TooltipWindow::outlineColourId,    juce::Colours::transparentBlack);

    setColour(juce::AlertWindow::backgroundColourId, theme.background);
    setColour(juce::AlertWindow::textColourId,       theme.textPrimary);
    setColour(juce::AlertWindow::outlineColourId,    juce::Colours::transparentBlack);

    setColour(juce::TextEditor::backgroundColourId,      theme.structure);
    setColour(juce::TextEditor::textColourId,            theme.textPrimary);
    setColour(juce::TextEditor::outlineColourId,         juce::Colours::transparentBlack);
    setColour(juce::TextEditor::focusedOutlineColourId,  theme.primaryAccent);
    setColour(juce::TextEditor::highlightColourId,       theme.primaryAccent.withAlpha(0.35f));
    setColour(juce::TextEditor::highlightedTextColourId, theme.textPrimary);
    setColour(juce::TextEditor::shadowColourId,          juce::Colours::transparentBlack);

    setColour(juce::TextButton::buttonColourId,   theme.structure);
    setColour(juce::TextButton::buttonOnColourId, theme.primaryAccent.withAlpha(0.25f));
    setColour(juce::TextButton::textColourOffId,  theme.textPrimary);
    setColour(juce::TextButton::textColourOnId,   theme.textPrimary);
}

//==============================================================================
// Rotary sliders

void CSLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height
                                     , float sliderPos, float rotaryStartAngle, float rotaryEndAngle
                                     , juce::Slider &slider)
{
    const float boundsSize = (float) juce::jmin(width, height);
    const float cx         = (float) x + (float) width  * 0.5f;
    const float cy         = (float) y + (float) height * 0.5f;

    const float arcRadius  = boundsSize * 0.42f;
    const float arcThick   = juce::jmax(2.0f, boundsSize * 0.08f);
    const float curAngle   = juce::jmap(sliderPos, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);
    const auto fillColor   = slider.findColour(juce::Slider::rotarySliderFillColourId);

    juce::Path bgArc;
    bgArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(trackBackground);
    g.strokePath(bgArc, juce::PathStrokeType(arcThick, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path valueArc;
    valueArc.addCentredArc(cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, curAngle, true);
    g.setColour(fillColor);
    g.strokePath(valueArc, juce::PathStrokeType(arcThick, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // "snapAt50": radial tick at centre, brightens within 1.5% of dead centre.
    // Visual cue only, no actual snapping.
    const bool snapAt50 = (bool) slider.getProperties().getWithDefault("snapAt50", false);

    if (snapAt50)
    {
        const float centreAngle = juce::jmap(0.5f, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);
        const bool  atCentre    = std::abs(sliderPos - 0.5f) < 0.015f;

        const auto  thumbColor  = slider.findColour(juce::Slider::thumbColourId);
        g.setColour(atCentre ? thumbColor : thumbColor.withAlpha(0.35f));

        const float r1 = arcRadius - arcThick * 0.5f - 1.0f;
        const float r2 = arcRadius + arcThick * 0.5f + 1.0f;

        const float x1 = cx + std::sin(centreAngle) * r1;
        const float y1 = cy - std::cos(centreAngle) * r1;
        const float x2 = cx + std::sin(centreAngle) * r2;
        const float y2 = cy - std::cos(centreAngle) * r2;

        g.drawLine(x1, y1, x2, y2, 1.5f);
    }
}

void CSLookAndFeel::SetTrackBackground(juce::Colour color)
{
    trackBackground = color;
}


void CSLookAndFeel::drawComboBox(juce::Graphics &g, int width, int height
                                 , bool /*isButtonDown*/
                                 , int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/
                                 , juce::ComboBox &box)
{
    const auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float) width, (float) height);

    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(bounds, popupCornerRadius);

    const auto outline = box.hasKeyboardFocus(true)
                             ? box.findColour(juce::ComboBox::focusedOutlineColourId)
                             : box.findColour(juce::ComboBox::outlineColourId);
    g.setColour(outline);
    g.drawRoundedRectangle(bounds.reduced(0.5f), popupCornerRadius, 1.0f);

    const float arrowSize = (float) juce::jmin(8, height / 3);
    const float arrowX    = (float) width - arrowSize - 10.0f;
    const float arrowY    = ((float) height - arrowSize * 0.5f) * 0.5f;

    juce::Path arrow;
    arrow.addTriangle(arrowX
                      , arrowY
                      , arrowX + arrowSize
                      , arrowY
                      , arrowX + arrowSize * 0.5f
                      , arrowY + arrowSize * 0.5f);
    g.setColour(box.findColour(juce::ComboBox::arrowColourId));
    g.fillPath(arrow);
}

juce::Font CSLookAndFeel::getComboBoxFont(juce::ComboBox &)
{
    return juce::Font(juce::FontOptions("Helvetica", 13.0f, 0));
}

void CSLookAndFeel::positionComboBoxText(juce::ComboBox &box, juce::Label &label)
{
    label.setBounds(8
                    , 1
                    , box.getWidth() - 24
                    , box.getHeight() - 2);
    label.setFont(getComboBoxFont(box));
    label.setColour(juce::Label::textColourId, box.findColour(juce::ComboBox::textColourId));
}

void CSLookAndFeel::drawPopupMenuBackground(juce::Graphics &g, int width, int height)
{
    const auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float) width, (float) height);

    g.setColour(theme.background);
    g.fillRoundedRectangle(bounds, popupCornerRadius);
}

void CSLookAndFeel::drawPopupMenuItem(juce::Graphics               &g
                                      , const juce::Rectangle<int> &area
                                      , bool                        isSeparator
                                      , bool                        isActive
                                      , bool                        isHighlighted
                                      , bool                        isTicked
                                      , bool                        hasSubMenu
                                      , const juce::String         &text
                                      , const juce::String         &shortcutKeyText
                                      , const juce::Drawable       *icon
                                      , const juce::Colour         *textColourOverride)
{
    if (isSeparator)
    {
        auto line = area.toFloat().reduced((float) popupItemPaddingX * 0.5f, 0.0f);
        line.setY(area.toFloat().getCentreY() - 0.5f);
        line.setHeight(1.0f);

        g.setColour(theme.structure);
        g.fillRect(line);
        return;
    }

    if (isHighlighted && isActive)
    {
        g.setColour(findColour(juce::PopupMenu::highlightedBackgroundColourId));
        g.fillRoundedRectangle(area.toFloat().reduced(3.0f, 1.0f), popupCornerRadius * 0.75f);
    }

    const auto baseText = textColourOverride != nullptr
                              ? *textColourOverride
                              : findColour(isHighlighted
                                               ? juce::PopupMenu::highlightedTextColourId
                                               : juce::PopupMenu::textColourId);
    
    const auto textColour = isActive ? baseText : baseText.withMultipliedAlpha(0.4f);

    auto contentArea = area.reduced(popupItemPaddingX, 0);

    if (isTicked)
    {
        const int  tickSize = juce::jmin(contentArea.getHeight() - 6, 12);
        const auto tickArea = contentArea.removeFromLeft(tickSize + 6)
                                         .withSizeKeepingCentre(tickSize, tickSize)
                                         .toFloat();

        g.setColour(theme.primaryAccent);
        juce::Path tick;
        tick.startNewSubPath((float)tickArea.getX(),                              (float)tickArea.getCentreY());
        tick.lineTo         ((float)tickArea.getX() + tickArea.getWidth() * 0.4f, (float)tickArea.getBottom());
        tick.lineTo         ((float)tickArea.getRight(),                          (float)tickArea.getY());
        g.strokePath(tick, juce::PathStrokeType(1.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    else if (icon != nullptr)
    {
        const auto iconArea = contentArea.removeFromLeft(contentArea.getHeight()).reduced(3);
        icon->drawWithin(g, iconArea.toFloat(), juce::RectanglePlacement::centred, 1.0f);
    }

    if (hasSubMenu)
    {
        const int  arrowSize = juce::jmin(contentArea.getHeight() / 3, 6);
        const auto arrowArea = contentArea.removeFromRight(arrowSize + 6)
                                          .withSizeKeepingCentre(arrowSize, arrowSize)
                                          .toFloat();

        juce::Path arrow;
        arrow.addTriangle(arrowArea.getX()
                          , arrowArea.getY()
                          , arrowArea.getX()
                          , arrowArea.getBottom()
                          , arrowArea.getRight()
                          , arrowArea.getCentreY());

        g.setColour(textColour.withAlpha(0.7f));
        g.fillPath(arrow);
    }

    if (shortcutKeyText.isNotEmpty())
    {
        auto       shortcutFont  = getPopupMenuFont().withHeight(getPopupMenuFont().getHeight() * 0.85f);
        const int  shortcutWidth = juce::GlyphArrangement::getStringWidthInt(shortcutFont, shortcutKeyText) + 8;
        auto       shortcutArea  = contentArea.removeFromRight(shortcutWidth);

        g.setFont(shortcutFont);
        g.setColour(textColour.withMultipliedAlpha(0.55f));
        g.drawText(shortcutKeyText, shortcutArea, juce::Justification::centredRight, true);
    }

    g.setFont(getPopupMenuFont());
    g.setColour(textColour);
    g.drawText(text, contentArea, juce::Justification::centredLeft, true);
}

juce::Font CSLookAndFeel::getPopupMenuFont()
{
    return juce::Font(juce::FontOptions("Helvetica", 13.0f, 0))
           .withExtraKerningFactor(0.04f);
}

void CSLookAndFeel::getIdealPopupMenuItemSize(const juce::String &text
                                              , bool              isSeparator
                                              , int               standardMenuItemHeight
                                              , int              &idealWidth
                                              , int              &idealHeight)
{
    if (isSeparator)
    {
        idealWidth  = 50;
        idealHeight = popupSeparatorH;
        return;
    }

    auto font   = getPopupMenuFont();
    idealWidth  = juce::GlyphArrangement::getStringWidthInt(font, text) + popupItemPaddingX * 2 + 24;
    idealHeight = standardMenuItemHeight > 0 ? standardMenuItemHeight : popupItemHeight;
}

void CSLookAndFeel::drawTooltip(juce::Graphics &g, const juce::String &text, int width, int height)
{
    const auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float) width, (float) height);

    g.setColour(theme.background);
    g.fillRoundedRectangle(bounds, popupCornerRadius);

    g.setColour(theme.primaryAccent.withAlpha(0.5f));
    g.drawRoundedRectangle(bounds.reduced(0.5f), popupCornerRadius, 1.0f);

    juce::AttributedString s;
    s.append(text, getPopupMenuFont(), theme.textPrimary);
    s.setJustification(juce::Justification::centred);

    juce::TextLayout layout;
    layout.createLayout(s, (float) width - 12.0f);
    layout.draw(g, bounds.reduced(6.0f, 4.0f));
}

juce::Rectangle<int> CSLookAndFeel::getTooltipBounds(const juce::String   &tipText
                                                    , juce::Point<int>     screenPos
                                                    , juce::Rectangle<int> parentArea)
{
    juce::AttributedString s;
    s.append(tipText, getPopupMenuFont());

    juce::TextLayout layout;
    layout.createLayout(s, 400.0f);

    const int w = (int) std::ceil(layout.getWidth())  + 16;
    const int h = (int) std::ceil(layout.getHeight()) + 10;

    return juce::Rectangle<int>(screenPos.x > parentArea.getCentreX() ? screenPos.x - (w + 12) : screenPos.x + 24,
                                screenPos.y > parentArea.getCentreY() ? screenPos.y - (h + 6)  : screenPos.y + 6,
                                w, h).constrainedWithin(parentArea);
}

void CSLookAndFeel::drawAlertBox(juce::Graphics              &g
                                 , juce::AlertWindow         &alert
                                 , const juce::Rectangle<int> &textArea
                                 , juce::TextLayout          &textLayout)
{
    const auto bounds = alert.getLocalBounds().toFloat();

    g.setColour(theme.background);
    g.fillRoundedRectangle(bounds, popupCornerRadius);

    // Recolour title runs in place: title font is taller than the body, so any
    // run above the body-font height is part of the title.
    const float bodyFontHeight = getAlertWindowMessageFont().getHeight();

    for (int line = 0; line < textLayout.getNumLines(); ++line)
        for (auto *run : textLayout.getLine(line).runs)
            if (run != nullptr && run->font.getHeight() > bodyFontHeight + 0.5f)
                run->colour = theme.primaryAccent;

    textLayout.draw(g, textArea.toFloat());
}

juce::Font CSLookAndFeel::getAlertWindowFont()
{
    return juce::Font(juce::FontOptions("Helvetica", 13.0f, 0));
}

juce::Font CSLookAndFeel::getAlertWindowTitleFont()
{
    return juce::Font(juce::FontOptions("Helvetica", 16.0f, juce::Font::bold))
           .withExtraKerningFactor(0.04f);
}

juce::Font CSLookAndFeel::getAlertWindowMessageFont()
{
    return juce::Font(juce::FontOptions("Helvetica", 13.0f, 0));
}
