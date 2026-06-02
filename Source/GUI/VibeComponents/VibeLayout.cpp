/*
  ==============================================================================

    VibeLayout.cpp
    Author:  Robert Fullum

  ==============================================================================
*/

#include "VibeLayout.h"

//==============================================================================

VibeLayout::VibeLayout()
{
    setOpaque(false);
}

VibeLayout::~VibeLayout() {}

void VibeLayout::resized()
{
    static constexpr int margin        = 50;
    static constexpr int labelH        = 15;
    static constexpr int componentSize = 141;
    static constexpr int slotH         = 30;
    
    auto bounds = getLocalBounds();
    bounds.removeFromTop(margin);
    
    sliderLabelArea = bounds.removeFromTop(labelH);
    sliderArea      = bounds.removeFromTop(componentSize);
    
    bounds.removeFromBottom(margin);
    slot2Area      = bounds.removeFromBottom(slotH).withSizeKeepingCentre(componentSize, slotH);
    slot2LabelArea = bounds.removeFromBottom(labelH);
    
    bounds.removeFromBottom(20);
    slot1Area      = bounds.removeFromBottom(slotH).withSizeKeepingCentre(componentSize, slotH);
    slot1LabelArea = bounds.removeFromBottom(labelH);
}
