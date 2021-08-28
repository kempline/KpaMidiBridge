/*
  ==============================================================================

    MainWindowViewPort.h
    Created: 28 Aug 2021 11:44:27am
    Author:  Sparenberg, Heiko

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MidiBridge.h"

class MainWindowViewPort : public juce::Viewport {
public:
    MainWindowViewPort() {
        setSize (732, 520);
        kpaMidiBridge.setSize (732, 1000);
        addAndMakeVisible(kpaMidiBridge);
        setViewedComponent(&kpaMidiBridge, false);
    }
    ~MainWindowViewPort() {
        
    }
    void resized() override
    {
        auto localBounds = getLocalBounds();
        auto requestedHeight = kpaMidiBridge.estimataHeight(localBounds);
        printf("Width: %d\n", localBounds.getWidth());
        //kpaMidiBridge.setSize(500, 700);
    }
private:
    KpaMidiBridge kpaMidiBridge;
};
