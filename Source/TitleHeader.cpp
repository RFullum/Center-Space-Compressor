/*
  ==============================================================================

    TitleHeader.cpp
    Created: 4 Feb 2021 11:47:30am
    Author:  Robert Fullum

  ==============================================================================
*/

#include <JuceHeader.h>
#include "TitleHeader.h"

//==============================================================================
TitleHeader::TitleHeader() : backgroundColor ( Colour( (uint8)125, (uint8)125,  (uint8)125 ) ),
                             textColor       ( Colour( (uint8)235, (uint8)52,   (uint8)222)  )
{
}

TitleHeader::~TitleHeader()
{
}

void TitleHeader::paint (juce::Graphics& g)
{
    g.fillAll(backgroundColor);
    
    auto totalArea = getLocalBounds();
    
    g.setColour ( textColor );
    g.setFont   ( Font("technoid", 65.0f, 0) );
    g.drawText  ( "FULLUMMUSIC", totalArea, Justification::right );
}

void TitleHeader::resized()
{
    
    
}

void TitleHeader::setBackgroundColor(Colour bg)
{
    backgroundColor = bg;
}
