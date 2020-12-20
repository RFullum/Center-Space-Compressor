/*
  ==============================================================================

    Background.cpp
    Created: 20 Dec 2020 7:26:12pm
    Author:  Robert Fullum

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Background.h"

//==============================================================================
Background::Background()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

Background::~Background()
{
}

void Background::paint (juce::Graphics& g)
{
    auto& random = Random::getSystemRandom();
    
    Colour colour(random.nextInt(Range<int>(0, 255)), random.nextInt(Range<int>(0, 255)), random.nextInt(Range<int>(0, 255)) );
    g.fillAll (colour);   // clear the background

}

void Background::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
