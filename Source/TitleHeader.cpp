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
                             textColor       ( Colour( (uint8)235, (uint8)52,   (uint8)222)  ),
                             designColor     ( Colour( (uint8)125, (uint8)0,    (uint8)125)  )
{
}

TitleHeader::~TitleHeader()
{
}

void TitleHeader::paint (juce::Graphics& g)
{
    g.fillAll   ( backgroundColor );
    g.setColour ( textColor );
    
    g.setFont   ( Font("technoid", 60.0f, 0) );
    g.drawText  ( "FULLUMMUSIC", fullumMusicArea.reduced( 6, 0 ), Justification::right );
    
    g.setFont  ( Font("helvetica", 53.0f, 0) );
    g.drawText ( "CENTER", centerArea, Justification::verticallyCentred );
    g.drawText ( "SPACE",  spaceArea,  Justification::verticallyCentred );
    
    g.setColour( designColor );
    g.fillRect ( block1L  );
    g.fillRect ( block4L  );
    g.fillRect ( block7L  );
    g.fillRect ( block10L );
    g.fillRect ( block13L );
    g.fillRect ( block1R  );
    g.fillRect ( block4R  );
    g.fillRect ( block7R  );
    g.fillRect ( block10R );
    g.fillRect ( block13R );
    
    
    //g.setFont   ( Font("technoid", 30.0f, 0) );
    //g.drawText  ( "Center",     centerArea.reduced     ( 10, 0 ), Justification::left );
    //g.drawText  ( "Duck",       duckArea.reduced       ( 10, 0 ), Justification::left );
    //g.drawText  ( "Compressor", compressorArea.reduced ( 12, 0 ), Justification::left );
}

void TitleHeader::resized()
{
    auto totalArea = getLocalBounds();
    
    fullumMusicArea = totalArea.removeFromRight( totalArea.getWidth() * 0.5f );
    
    Rectangle<int> pluginTitleArea = totalArea;
    Rectangle<int> titlePadLeft    = totalArea.removeFromLeft ( 8.0f );
    Rectangle<int> titleLeftArea   = totalArea.removeFromLeft ( pluginTitleArea.getWidth() * 0.5f );
    Rectangle<int> titleRightArea  = totalArea;
    
    //Rectangle<int> designPadL = titleLeftArea.removeFromBottom  ( 5 );
    //Rectangle<int> designPadR = titleRightArea.removeFromBottom ( 5 );
    Rectangle<int> titleDesignL = titleLeftArea.removeFromRight ( titleLeftArea.getWidth() * 0.2f  );
    Rectangle<int> titleDesignR = titleRightArea.removeFromLeft ( titleRightArea.getWidth() * 0.2f );
    
    centerArea.setBounds ( titleLeftArea.getX(),  titleLeftArea.getY(),  titleLeftArea.getWidth(),  titleLeftArea.getHeight()  );
    spaceArea.setBounds  ( titleRightArea.getX(), titleRightArea.getY(), titleRightArea.getWidth(), titleRightArea.getHeight() );
    
    int designPadTop = 15;
    int designPadBottom = 23;
    
    Rectangle<int> titleDesignLPadTop = titleDesignL.removeFromTop( designPadTop );
    Rectangle<int> titleDesignRPadTop = titleDesignR.removeFromTop( designPadTop );
    Rectangle<int> titleDesignLPadBottom = titleDesignL.removeFromBottom( designPadBottom );
    Rectangle<int> titleDesignRPadBottom = titleDesignR.removeFromBottom( designPadBottom );
    
    float blockWidth = titleDesignL.getWidth() * 0.125f;
    float blockHeight = titleDesignL.getHeight() / 9.0f;
    
    Rectangle<int> section1L   = titleDesignL.removeFromLeft   ( blockWidth  );
    Rectangle<int> section2La  = titleDesignL.removeFromTop    ( blockHeight );
    Rectangle<int> section2Lb  = titleDesignL.removeFromBottom ( blockHeight );
    Rectangle<int> section3L   = titleDesignL.removeFromLeft   ( blockWidth  );
    Rectangle<int> section4L   = titleDesignL.removeFromLeft   ( blockWidth  );
    Rectangle<int> section5La  = titleDesignL.removeFromTop    ( blockHeight );
    Rectangle<int> section5Lb  = titleDesignL.removeFromBottom ( blockHeight );
    Rectangle<int> section6L   = titleDesignL.removeFromLeft   ( blockWidth  );
    Rectangle<int> section7L   = titleDesignL.removeFromLeft   ( blockWidth  );
    Rectangle<int> section8La  = titleDesignL.removeFromTop    ( blockHeight );
    Rectangle<int> section8Lb  = titleDesignL.removeFromBottom ( blockHeight );
    Rectangle<int> section9L   = titleDesignL.removeFromLeft   ( blockWidth  );
    Rectangle<int> section10L  = titleDesignL.removeFromLeft   ( blockWidth  );
    Rectangle<int> section11La = titleDesignL.removeFromTop    ( blockHeight );
    Rectangle<int> section11Lb = titleDesignL.removeFromBottom ( blockHeight );
    Rectangle<int> section12L  = titleDesignL.removeFromLeft   ( blockWidth  );
    Rectangle<int> section13L  = titleDesignL;
    
    block1L.setBounds  ( section1L.getX(),  section1L.getY(),  section1L.getWidth(),  section1L.getHeight()  );
    block4L.setBounds  ( section4L.getX(),  section4L.getY(),  section4L.getWidth(),  section4L.getHeight()  );
    block7L.setBounds  ( section7L.getX(),  section7L.getY(),  section7L.getWidth(),  section7L.getHeight()  );
    block10L.setBounds ( section10L.getX(), section10L.getY(), section10L.getWidth(), section10L.getHeight() );
    block13L.setBounds ( section13L.getX(), section13L.getY(), section13L.getWidth(), section13L.getHeight() );
    
    
    Rectangle<int> section1R   = titleDesignR.removeFromRight  ( blockWidth  );
    Rectangle<int> section2Ra  = titleDesignR.removeFromTop    ( blockHeight );
    Rectangle<int> section2Rb  = titleDesignR.removeFromBottom ( blockHeight );
    Rectangle<int> section3R   = titleDesignR.removeFromRight  ( blockWidth  );
    Rectangle<int> section4R   = titleDesignR.removeFromRight  ( blockWidth  );
    Rectangle<int> section5Ra  = titleDesignR.removeFromTop    ( blockHeight );
    Rectangle<int> section5Rb  = titleDesignR.removeFromBottom ( blockHeight );
    Rectangle<int> section6R   = titleDesignR.removeFromRight  ( blockWidth  );
    Rectangle<int> section7R   = titleDesignR.removeFromRight  ( blockWidth  );
    Rectangle<int> section8Ra  = titleDesignR.removeFromTop    ( blockHeight );
    Rectangle<int> section8Rb  = titleDesignR.removeFromBottom ( blockHeight );
    Rectangle<int> section9R   = titleDesignR.removeFromRight  ( blockWidth  );
    Rectangle<int> section10R  = titleDesignR.removeFromRight  ( blockWidth  );
    Rectangle<int> section11Ra = titleDesignR.removeFromTop    ( blockHeight );
    Rectangle<int> section11Rb = titleDesignR.removeFromBottom ( blockHeight );
    Rectangle<int> section12R  = titleDesignR.removeFromRight  ( blockWidth  );
    Rectangle<int> section13R  = titleDesignR;
    
    block1R.setBounds  ( section1R.getX(),  section1R.getY(),  section1R.getWidth(),  section1R.getHeight()  );
    block4R.setBounds  ( section4R.getX(),  section4R.getY(),  section4R.getWidth(),  section4R.getHeight()  );
    block7R.setBounds  ( section7R.getX(),  section7R.getY(),  section7R.getWidth(),  section7R.getHeight()  );
    block10R.setBounds ( section10R.getX(), section10R.getY(), section10R.getWidth(), section10R.getHeight() );
    block13R.setBounds ( section13R.getX(), section13R.getY(), section13R.getWidth(), section13R.getHeight() );
    
    //float titleDivisions = pluginTitleArea.getHeight() * 0.33f;
    
    //centerArea     = pluginTitleArea.removeFromTop( titleDivisions );
    //duckArea       = pluginTitleArea.removeFromTop( titleDivisions );
    //compressorArea = pluginTitleArea;
    
}

void TitleHeader::setBackgroundColor(Colour& bg)
{
    backgroundColor = bg;
}

void TitleHeader::setTextColor(Colour& textC)
{
    textColor = textC;
}

void TitleHeader::setDesignColor(Colour& designC)
{
    designColor = designC;
}


//==============================================================================
//==============================================================================
//==============================================================================



void TitleFooter::paint(juce::Graphics& g)
{
    g.fillAll   (backgroundColor);
    g.setColour ( textColor );
    
    g.setFont  ( Font("Helvetica", 14, 0) );
    g.drawText ( "Version 1.0", versionArea.reduced( 12, 6 ), Justification::left );
    g.drawText ( "www.FullumMusic.com", urlArea.reduced( 12, 6 ), Justification::right );
}


void TitleFooter::resized()
{
    auto totalArea = getLocalBounds();
    
    versionArea = totalArea.removeFromLeft( totalArea.getWidth() * 0.5f );
    urlArea     = totalArea;
}



