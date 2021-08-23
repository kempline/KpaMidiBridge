/*
  ==============================================================================

    This file contains the startup code for a PIP.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MidiBridge.h"

class Application    : public juce::JUCEApplication
{
public:
    //==============================================================================
    Application() = default;

    const juce::String getApplicationName() override       { return "KPA Midi-Bridge"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }

#if defined(JUCE_IOS)
    AudioDeviceManager   f_AudioDeviceManager;
    AudioSourcePlayer    f_FakeAudioSourcePlayer;
#endif
    
    void initialise (const juce::String&) override
    {
        mainWindow.reset (new MainWindow (getApplicationName(), new KpaMidiBridge, *this));
        #if defined(JUCE_IOS)
        f_AudioDeviceManager.initialiseWithDefaultDevices(0, 1);
        #endif
    }

    void shutdown() override                         {
        mainWindow = nullptr;
        #if defined(JUCE_IOS)
        f_AudioDeviceManager.removeAudioCallback(&f_FakeAudioSourcePlayer);
        #endif
    }
    void suspended() override
    {
        #if defined(JUCE_IOS)
        f_AudioDeviceManager.addAudioCallback(&f_FakeAudioSourcePlayer);
        #endif
    }

    void resumed() override
    {
        #if defined(JUCE_IOS)
        f_AudioDeviceManager.removeAudioCallback(&f_FakeAudioSourcePlayer);
        #endif
    }
    
private:
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (const juce::String& name, juce::Component* c, JUCEApplication& a)
            : DocumentWindow (name, juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                                .findColour (ResizableWindow::backgroundColourId),
                              juce::DocumentWindow::allButtons),
              app (a)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (c, true);

           #if JUCE_ANDROID || JUCE_IOS
            setFullScreen (true);
           #else
            setResizable (true, false);
            setResizeLimits (300, 250, 10000, 10000);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            app.systemRequestedQuit();
        }

    private:
        JUCEApplication& app;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
START_JUCE_APPLICATION (Application)
