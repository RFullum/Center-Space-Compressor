/*
  ==============================================================================

    TweakLayout.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "TweakLayout.h"

//==============================================================================

TweakLayout::TweakLayout()
{
    setOpaque(false);
}

TweakLayout::~TweakLayout() {}

void TweakLayout::resized()
{
    static constexpr int margin        = 50;
    static constexpr int labelH        = 15;
    static constexpr int componentSize = 141;
    static constexpr int slotH         = 30;
    
    auto bounds = getLocalBounds();
    
    bounds.removeFromBottom(margin);
    slot2Area      = bounds.removeFromBottom(slotH).withSizeKeepingCentre(componentSize, slotH);
    slot2LabelArea = bounds.removeFromBottom(labelH);
    
    bounds.removeFromBottom(20);
    slot1Area      = bounds.removeFromBottom(slotH).withSizeKeepingCentre(componentSize, slotH);
    slot1LabelArea = bounds.removeFromBottom(labelH);
    
    bounds.reduce(50, 15);
    const int rowH = bounds.proportionOfHeight(0.45f);
    auto topRow = bounds.removeFromTop(rowH);
    auto botRow = bounds.removeFromBottom(rowH);
    auto topL   = topRow.removeFromLeft(topRow.proportionOfWidth(0.5f));
    auto botL   = botRow.removeFromLeft(botRow.proportionOfWidth(0.5f));
    sliderLabel1Area = topL.removeFromTop(labelH);
    slider1Area      = topL;
    sliderLabel2Area = topRow.removeFromTop(labelH);
    slider2Area      = topRow;
    sliderLabel3Area = botL.removeFromTop(labelH);
    slider3Area      = botL;
    sliderLabel4Area = botRow.removeFromTop(labelH);
    slider4Area      = botRow;
}
