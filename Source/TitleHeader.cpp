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
TitleHeader::TitleHeader() : backgroundColor( Colour( (uint8)125, (uint8)125,  (uint8)125 ) )
{
}

TitleHeader::~TitleHeader()
{
}

void TitleHeader::paint (juce::Graphics& g)
{
    
    g.fillAll(backgroundColor);
}

void TitleHeader::resized()
{
}

void TitleHeader::setBackgroundColor(Colour bg)
{
    backgroundColor = bg;
}
