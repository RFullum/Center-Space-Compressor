/*
  ==============================================================================

    OtherLookAndFeel.cpp
    Created: 4 Feb 2021 10:59:27am
    Author:  Robert Fullum

  ==============================================================================
*/

#include "OtherLookAndFeel.h"


/// Constructor
OtherLookAndFeel::OtherLookAndFeel() :
    dialColor( Colour( (uint8)125, (uint8)125,  (uint8)125 ) ),
    tickColor( Colour( (uint8)0,   (uint8)0,    (uint8)0   ) ),
    backColor( Colour( (uint8)255, (uint8)255,  (uint8)255 ) )
{}

// Destructor
OtherLookAndFeel::~OtherLookAndFeel()  {}

/// Draws rotary slider as round knob with outer ring and circular tick
void OtherLookAndFeel::drawRotarySlider(Graphics &g, int x, int y, int width, int height, float sliderPos,
                                        float rotaryStartAngle, float rotaryEndAngle, Slider &slider)
{
    // Dial Outer Ring & Tick Variables
    float reducer  = 0.75f;
    float diameter = jmin(width * reducer, height * reducer);   // Lesser of two creates squre & circle from rect & ellipse
    float radius   = diameter * 0.5f;
    float centerX  = x + (width / 2.0f);     // Finds center offset by x value to actually center it
    float centerY  = y + (height / 2.0f);    // Ditto for centerY
    float radiusX  = centerX - radius;
    float radiusY  = centerY - radius;
    /*
     - The difference between rotaryEndAngle and rotaryStartAngle is the total distance travelable in radians.
     - Multiplying that by slider position says what percentage of the way around that distance you are.
     - Plus the initial rotaryStartAngle to account for the space between 0 and the rotaryStartAngle.
     */
    float tickAngle = rotaryStartAngle + ( sliderPos * (rotaryEndAngle - rotaryStartAngle) );
    float tickWidth = diameter * 0.15f;     // Ticks proportional to dial size
    
    // Dial Mid Ring (Offsets the radius so it centers over Outer Ring)
    float diameter2 = diameter * 0.9f;
    float radiusX2  = radiusX + ( (diameter - diameter2) * 0.5f );
    float radiusY2  = radiusY + ( (diameter - diameter2) * 0.5f );
    
    // Dial Inner (Offsets the radius so it centers over Outer Ring)
    float diameter3 = diameter2 * 0.9f;
    float radiusX3  = radiusX + ( (diameter - diameter3) * 0.5f );
    float radiusY3  = radiusY + ( (diameter - diameter3) * 0.5f );
    
    // Rectangles the Dial Rings will go inside
    Rectangle<float> dialArea  ( radiusX,  radiusY,  diameter,  diameter  );
    Rectangle<float> dialArea2 ( radiusX2, radiusY2, diameter2, diameter2 );
    Rectangle<float> dialArea3 ( radiusX3, radiusY3, diameter3, diameter3 );

    g.setColour   ( backColor );
    g.fillEllipse ( dialArea  );
    
    g.setColour   ( tickColor );
    g.fillEllipse ( dialArea2 );
    
    g.setColour   ( dialColor );
    g.fillEllipse ( dialArea3 );
    
    // The radius of the inner dial area to put the tick directly on its edge
    float radius3 = diameter3 * 0.5f;
    
    Path dialTick;      // Drawable path to draw the dial tick
    dialTick.addEllipse ( 0.0f, -radius3 + 5.0f, tickWidth, tickWidth );
    
    g.setColour ( tickColor );
    g.fillPath  ( dialTick, AffineTransform::rotation( tickAngle ).translated( centerX, centerY ) );
}


/// Sets font for Slider Text Box
Font OtherLookAndFeel::getLabelFont (Label& label)
{
    //return label.getFont();
    return Font ( "futura", 17.0f, 0 );
}

void OtherLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                        float sliderPos,
                                        float minSliderPos,
                                        float maxSliderPos,
                                        const Slider::SliderStyle style, Slider& slider)
{
    if (slider.isBar())
    {
        g.setColour (slider.findColour (Slider::trackColourId));
        g.fillRect (slider.isHorizontal() ? Rectangle<float> (static_cast<float> (x), (float) y + 0.5f, sliderPos - (float) x, (float) height - 1.0f)
                                          : Rectangle<float> ((float) x + 0.5f, sliderPos, (float) width - 1.0f, (float) y + ((float) height - sliderPos)));
    }
    else
    {
        auto isTwoVal   = (style == Slider::SliderStyle::TwoValueVertical   || style == Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

        auto trackWidth = jmin (6.0f, slider.isHorizontal() ? (float) height * 0.25f : (float) width * 0.25f);

        Point<float> startPoint (slider.isHorizontal() ? (float) x : (float) x + (float) width * 0.5f,
                                 slider.isHorizontal() ? (float) y + (float) height * 0.5f : (float) (height + y));

        Point<float> endPoint (slider.isHorizontal() ? (float) (width + x) : startPoint.x,
                               slider.isHorizontal() ? startPoint.y : (float) y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        if (isTwoVal || isThreeVal)
        {
            minPoint = { slider.isHorizontal() ? minSliderPos : (float) width * 0.5f,
                         slider.isHorizontal() ? (float) height * 0.5f : minSliderPos };

            if (isThreeVal)
                thumbPoint = { slider.isHorizontal() ? sliderPos : (float) width * 0.5f,
                               slider.isHorizontal() ? (float) height * 0.5f : sliderPos };

            maxPoint = { slider.isHorizontal() ? maxSliderPos : (float) width * 0.5f,
                         slider.isHorizontal() ? (float) height * 0.5f : maxSliderPos };
        }
        else
        {
            auto kx = slider.isHorizontal() ? sliderPos : ((float) x + (float) width * 0.5f);
            auto ky = slider.isHorizontal() ? ((float) y + (float) height * 0.5f) : sliderPos;

            minPoint = startPoint;
            maxPoint = { kx, ky };
        }

        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (isThreeVal ? thumbPoint : maxPoint);
        g.setColour (slider.findColour (Slider::trackColourId));
        g.strokePath (valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

        if (! isTwoVal)
        {
            g.setColour (slider.findColour (Slider::thumbColourId));
            g.fillRect( Rectangle<float> (static_cast<float> (trackWidth), 6.0f).withCentre (isThreeVal ? thumbPoint : maxPoint));
        }

        if (isTwoVal || isThreeVal)
        {
            auto sr = jmin (trackWidth, (slider.isHorizontal() ? (float) height : (float) width) * 0.4f);
            auto pointerColour = slider.findColour (Slider::thumbColourId);

            if (slider.isHorizontal())
            {
                drawPointer (g, minSliderPos - sr,
                             jmax (0.0f, (float) y + (float) height * 0.5f - trackWidth * 2.0f),
                             trackWidth * 2.0f, pointerColour, 2);

                drawPointer (g, maxSliderPos - trackWidth,
                             jmin ((float) (y + height) - trackWidth * 2.0f, (float) y + (float) height * 0.5f),
                             trackWidth * 2.0f, pointerColour, 4);
            }
            else
            {
                drawPointer (g, jmax (0.0f, (float) x + (float) width * 0.5f - trackWidth * 2.0f),
                             minSliderPos - trackWidth,
                             trackWidth * 2.0f, pointerColour, 1);

                drawPointer (g, jmin ((float) (x + width) - trackWidth * 2.0f, (float) x + (float) width * 0.5f), maxSliderPos - sr,
                             trackWidth * 2.0f, pointerColour, 3);
            }
        }
    }
}

/// Sets the color of the dial (knob)
void OtherLookAndFeel::setDialColor(Colour& dialC)
{
    dialColor = dialC;
}

/// Sets the colorof the tick on the dial
void OtherLookAndFeel::setTickColor(Colour& tickC)
{
    tickColor = tickC;
}

/// Sets the colorof the tick on the dial
void OtherLookAndFeel::setBackColor(Colour& backC)
{
    backColor = backC;
}



// ==================================================================================================
// ==================================================================================================

BoxLookAndFeel::BoxLookAndFeel()
{}

BoxLookAndFeel::~BoxLookAndFeel()
{}


void BoxLookAndFeel::drawComboBox (Graphics& g, int width, int height, bool, int, int, int, int, ComboBox& box)
{
    auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour (box.findColour (ComboBox::backgroundColourId));
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);
    
    g.setColour( Colours::white );
    g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 3.0f);

    Rectangle<int> arrowZone (width - 30, 0, 20, height);
    Path path;
    path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
    path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
    path.lineTo ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);

    g.setColour (box.findColour (ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
    g.strokePath (path, PathStrokeType (3.0f));
}

/// Overrides to make minimum ComboBox Font size 18
Font BoxLookAndFeel::getComboBoxFont (ComboBox& box)
{
    return { jmin (18.0f, (float) box.getHeight() * 0.85f) };
}
