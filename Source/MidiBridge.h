
#pragma once
#include "Kpa.h"
#include "EffectButtonSettings.h"

#define BAND_HELPER_NEXT_SONG_CC 83
#define BAND_HELPER_MIDI_CHANNEL 3

//==============================================================================
struct MidiDeviceListEntry : ReferenceCountedObject
{
    MidiDeviceListEntry (MidiDeviceInfo info) : deviceInfo (info) {}

    MidiDeviceInfo deviceInfo;
    std::unique_ptr<MidiInput> inDevice;
    std::unique_ptr<MidiOutput> outDevice;

    using Ptr = ReferenceCountedObjectPtr<MidiDeviceListEntry>;
};


//==============================================================================
class KpaMidiBridge  : public Component,
                  private Timer,
                  private MidiKeyboardState::Listener,
                  private MidiInputCallback,
                  private AsyncUpdater
{
public:
    //==============================================================================
    KpaMidiBridge()
        : I("I"), II("II"), III("III"), IIII("IIII"),
          midiKeyboard       (keyboardState, MidiKeyboardComponent::horizontalKeyboard),
          midiInputSelector  (new MidiDeviceListBox ("Midi Input Selector",  *this, true)),
          midiOutputSelector (new MidiDeviceListBox ("Midi Output Selector", *this, false)),
          startTime (juce::Time::getMillisecondCounterHiRes() * 0.001)
    {
        addLabelAndSetStyle (midiInputLabel);
        addLabelAndSetStyle (midiOutputLabel);
        addLabelAndSetStyle (incomingMidiLabel);
        addLabelAndSetStyle (outgoingMidiLabel);

        midiKeyboard.setName ("MIDI Keyboard");
        addAndMakeVisible (midiKeyboard);

        midiMonitor.setMultiLine (true);
        midiMonitor.setReturnKeyStartsNewLine (false);
        midiMonitor.setReadOnly (true);
        midiMonitor.setScrollbarsShown (true);
        midiMonitor.setCaretVisible (false);
        midiMonitor.setPopupMenuEnabled (false);
        midiMonitor.setText ({});
        addAndMakeVisible (midiMonitor);

        if (! BluetoothMidiDevicePairingDialogue::isAvailable())
            pairButton.setEnabled (false);

        addAndMakeVisible (pairButton);
        pairButton.onClick = []
        {
            RuntimePermissions::request (RuntimePermissions::bluetoothMidi,
                                         [] (bool wasGranted)
                                         {
                                             if (wasGranted)
                                                 BluetoothMidiDevicePairingDialogue::open();
                                         });
        };
        keyboardState.addListener (this);

        addAndMakeVisible (midiInputSelector .get());
        addAndMakeVisible (midiOutputSelector.get());
        
		initProperties();
		effectButtonSettings.add(&I);
		effectButtonSettings.add(&II);
		effectButtonSettings.add(&III);
		effectButtonSettings.add(&IIII);
		
		for(auto effectButton: effectButtonSettings) {
			effectButton->setApplicationProperties(&applicationProperties);
			addAndMakeVisible (*effectButton);
		}

        midiIn->start();
		
        setSize (732, 520);

        startTimer (500);
    }

    ~KpaMidiBridge() override
    {
        stopTimer();
        midiInputs .clear();
        midiOutputs.clear();
        keyboardState.removeListener (this);

        midiInputSelector .reset();
        midiOutputSelector.reset();
    }

	void initProperties()
	{
		PropertiesFile::Options options;
		options.applicationName = ProjectInfo::projectName;
		options.filenameSuffix = ".settings";
		options.osxLibrarySubFolder = "Application Support";
		options.folderName = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getChildFile(ProjectInfo::projectName).getFullPathName();
		options.storageFormat = PropertiesFile::storeAsXML;
		
		applicationProperties.setStorageParameters(options);
	}
	
    //==============================================================================
    void timerCallback() override
    {
        updateDeviceList (true);
        updateDeviceList (false);
    }

    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
        MidiMessage m (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
        m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
        sendToOutputs (m);
        
        m = MidiMessage(MidiMessage::controllerEvent(BAND_HELPER_MIDI_CHANNEL+1, BAND_HELPER_NEXT_SONG_CC, 127));
        m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
        sendToOutputs(m);
    }

    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
        MidiMessage m (MidiMessage::noteOff (midiChannel, midiNoteNumber, velocity));
        m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
        sendToOutputs (m);
    }

    void paint (Graphics&) override {}

    int estimataHeight(Rectangle<int> parentsBounds) {
        auto minimumWidthForEffectSettings = 360;
        auto minimumHeightForEffectSettings = 160;
        
        auto heightMidiInComboBox = 20;
        auto heightBleButton = 20;
        
        auto estimatedHeight = 2 * minimumHeightForEffectSettings + heightMidiInComboBox + heightBleButton;
        if(parentsBounds.getWidth() < 2 * minimumWidthForEffectSettings) {
            estimatedHeight = 4 * minimumHeightForEffectSettings + heightMidiInComboBox + heightBleButton;
        }
        printf("Estimated Height: %d\n", estimatedHeight);
        return estimatedHeight;
    }
    
    void resized() override
    {
        auto localBounds = getLocalBounds();
        
        
        auto margin = 10;

        midiInputLabel.setBounds (margin, margin,
                                  (getWidth() / 2) - (2 * margin), 24);

        midiOutputLabel.setBounds ((getWidth() / 2) + margin, margin,
                                   (getWidth() / 2) - (2 * margin), 24);

        midiInputSelector->setBounds (margin, (2 * margin) + 24,
                                      (getWidth() / 2) - (2 * margin),
                                      (getHeight() / 2) - ((4 * margin) + 24 + 24));

        midiOutputSelector->setBounds ((getWidth() / 2) + margin, (2 * margin) + 24,
                                       (getWidth() / 2) - (2 * margin),
                                       (getHeight() / 2) - ((4 * margin) + 24 + 24));

        pairButton.setBounds (margin, (getHeight() / 2) - (margin + 24),
                              getWidth() - (2 * margin), 24);

        /*outgoingMidiLabel.setBounds (margin, getHeight() / 2, getWidth() - (2 * margin), 24);
        */
        
        /*
        midiKeyboard.setBounds (margin, (getHeight() / 2) + (24 + margin), getWidth() - (2 * margin), 64);
         */
        
        I.setBounds (margin, getHeight() / 2, getWidth()/2 - (2 * margin), 160);
        
        II.setBounds (margin + getWidth() / 2, getHeight() / 2, getWidth() / 2 - (2 * margin), 160);
        
        III.setBounds (margin, getHeight() / 2 + 160, getWidth() /2 - (2 * margin), 160);
        
        IIII.setBounds (margin + getWidth() / 2, getHeight() / 2 + 160, getWidth() /2 - (2 * margin), 160);
         /*
         incomingMidiLabel.setBounds (margin, (getHeight() / 2) + (24 + (2 * margin) + 64),
                                     getWidth() - (2 * margin), 24);

          */
        
        /*
        auto y = (getHeight() / 2) + ((2 * 24) + (3 * margin) + 64);
        midiMonitor.setBounds (margin, y,
                               getWidth() - (2 * margin), getHeight() - y - margin);
        
        */
    }

    void openDevice (bool isInput, int index)
    {
        if (isInput)
        {
            jassert (midiInputs[index]->inDevice.get() == nullptr);
            midiInputs[index]->inDevice = MidiInput::openDevice (midiInputs[index]->deviceInfo.identifier, this);

            if (midiInputs[index]->inDevice.get() == nullptr)
            {
                DBG ("MidiDemo::openDevice: open input device for index = " << index << " failed!");
                return;
            }

            midiInputs[index]->inDevice->start();
        }
        else
        {
            jassert (midiOutputs[index]->outDevice.get() == nullptr);
            midiOutputs[index]->outDevice = MidiOutput::openDevice (midiOutputs[index]->deviceInfo.identifier);

            if (midiOutputs[index]->outDevice.get() == nullptr)
            {
                DBG ("MidiDemo::openDevice: open output device for index = " << index << " failed!");
            }
        }
    }

    void closeDevice (bool isInput, int index)
    {
        if (isInput)
        {
            jassert (midiInputs[index]->inDevice.get() != nullptr);
            midiInputs[index]->inDevice->stop();
            midiInputs[index]->inDevice.reset();
        }
        else
        {
            jassert (midiOutputs[index]->outDevice.get() != nullptr);
            midiOutputs[index]->outDevice.reset();
        }
    }

    int getNumMidiInputs() const noexcept
    {
        return midiInputs.size();
    }

    int getNumMidiOutputs() const noexcept
    {
        return midiOutputs.size();
    }

    ReferenceCountedObjectPtr<MidiDeviceListEntry> getMidiDevice (int index, bool isInput) const noexcept
    {
        return isInput ? midiInputs[index] : midiOutputs[index];
    }

private:
    //==============================================================================
    struct MidiDeviceListBox : public ListBox,
                               private ListBoxModel
    {
        MidiDeviceListBox (const String& name,
                           KpaMidiBridge& contentComponent,
                           bool isInputDeviceList)
            : ListBox (name, this),
              parent (contentComponent),
              isInput (isInputDeviceList)
        {
            setOutlineThickness (1);
            setMultipleSelectionEnabled (true);
            setClickingTogglesRowSelection (true);
        }

        //==============================================================================
        int getNumRows() override
        {
            return isInput ? parent.getNumMidiInputs()
                           : parent.getNumMidiOutputs();
        }

        void paintListBoxItem (int rowNumber, Graphics& g,
                               int width, int height, bool rowIsSelected) override
        {
            auto textColour = getLookAndFeel().findColour (ListBox::textColourId);

            if (rowIsSelected)
                g.fillAll (textColour.interpolatedWith (getLookAndFeel().findColour (ListBox::backgroundColourId), 0.5));


            g.setColour (textColour);
            g.setFont ((float) height * 0.7f);

            if (isInput)
            {
                if (rowNumber < parent.getNumMidiInputs())
                    g.drawText (parent.getMidiDevice (rowNumber, true)->deviceInfo.name,
                                5, 0, width, height,
                                Justification::centredLeft, true);
            }
            else
            {
                if (rowNumber < parent.getNumMidiOutputs())
                    g.drawText (parent.getMidiDevice (rowNumber, false)->deviceInfo.name,
                                5, 0, width, height,
                                Justification::centredLeft, true);
            }
        }

        //==============================================================================
        void selectedRowsChanged (int) override
        {
            auto newSelectedItems = getSelectedRows();
            if (newSelectedItems != lastSelectedItems)
            {
                for (auto i = 0; i < lastSelectedItems.size(); ++i)
                {
                    if (! newSelectedItems.contains (lastSelectedItems[i]))
                        parent.closeDevice (isInput, lastSelectedItems[i]);
                }

                for (auto i = 0; i < newSelectedItems.size(); ++i)
                {
                    if (! lastSelectedItems.contains (newSelectedItems[i]))
                        parent.openDevice (isInput, newSelectedItems[i]);
                }

                lastSelectedItems = newSelectedItems;
            }
        }

        //==============================================================================
        void syncSelectedItemsWithDeviceList (const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices)
        {
            SparseSet<int> selectedRows;
            for (auto i = 0; i < midiDevices.size(); ++i)
                if (midiDevices[i]->inDevice.get() != nullptr || midiDevices[i]->outDevice.get() != nullptr)
                    selectedRows.addRange (Range<int> (i, i + 1));

            lastSelectedItems = selectedRows;
            updateContent();
            setSelectedRows (selectedRows, dontSendNotification);
        }

    private:
        //==============================================================================
        KpaMidiBridge& parent;
        bool isInput;
        SparseSet<int> lastSelectedItems;
    };

    //==============================================================================
    void onEffectButtonClicked(uint8 buttonNo, uint8 state) {
        printf("Effect button %d clicked, value: %d\n", buttonNo, state);
		if(0 == state) {
			for(const auto effectButton: effectButtonSettings) {
				effectButton->setColour (juce::GroupComponent::textColourId, juce::Colours::orange);
				effectButton->setColour (juce::GroupComponent::outlineColourId, juce::Colours::orange);
			}
			return;
		}
		if(buttonNo >= 0 && buttonNo <=3) {
			effectButtonSettings[buttonNo]->setColour (juce::GroupComponent::textColourId, juce::Colours::red);
			effectButtonSettings[buttonNo]->setColour (juce::GroupComponent::outlineColourId, juce::Colours::red);
			
			MidiMessage m = effectButtonSettings[buttonNo]->getMidiMessage();
			if(!m.isMidiContinue()) {
				m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001 - startTime);
				midiOut->sendMessageNow(m);
			}
		}
    }
    
    void handleIncomingMidiMessage (MidiInput* /*source*/, const MidiMessage& message) override
    {
        if(message.isMidiClock())
            return;
        
        if(message.getRawDataSize() == 1)
            if(message.getRawData()[0] == 0xfe)
                return;
        
        // This is called on the MIDI thread
        const ScopedLock sl (midiMonitorLock);
        incomingMessages.add (message);
        triggerAsyncUpdate();
        
        if(message.isSysEx()) {
            int size = message.getSysExDataSize();
            const uint8* array = message.getSysExData();
            
            for(int i=0; i<message.getSysExDataSize(); i++) {
                printf("%d ", array[i]);
            }
            printf("\n");
            
            if(size > KPA_INDEX_FCT_CODE+1) {
                // Check for Kemper messages starting with 0xF0 0x00 0x20 0x33
                if(array[0] == 0x00 && array[1] == 0x20 && array[2] == 0x33) {
                  // Serial.print("Function Code: "); Serial.println(array[KPA_INDEX_FCT_CODE]);
                  switch(array[KPA_INDEX_FCT_CODE]) {
                    case(KPA_FCT_EXTENDED_PARAMETER_CHANGE):
                      uint8_t ex_address[4]; // address
                      uint8_t ex_value[4];  // value
                      if(Kpa::convert5to4byte(&array[7], 5, ex_address) && Kpa::convert5to4byte(&array[12], 5, ex_value) ) {
                        if(ex_address[0] == 0 && ex_address[1] == 0 && ex_address[2] == 0x51) { // remote/stage effekt button pressed
                          onEffectButtonClicked(ex_address[3], ex_value[3]);
                        }
                      }
                      break;
                    case(KPA_FCT_STRING_PARAMETER_CHANGE):
                    case(KPA_FCT_EXTENDED_STRING_PARAMETER_CHANGE):
                    case(KPA_FCT_SINGLE_PARAMETER_CHANGE):
                    case(KPA_FCT_MULTI_PARAMETER_CHANGE):
                    case(KPA_FCT_BLOB):
                    case(KPA_FCT_REQUEST_SINGLE_PARAMETER_VALUE):
                    case(KPA_FCT_REQUEST_MULTI_PARAMETER_VALUE):
                    case(KPA_FCT_REQUEST_STRING_PARAMETER):
                    case(KPA_FCT_REQUEST_EXTENDED_STRING_PARAMETER):
                    case(KPA_FCT_REQUEST_PARAMETER_VALUE_AS_RENDERED_STRING):
                    case(KPA_FCT_RESPONSE_PARAMETER_VALUE_AS_RENDERED_STRING):
                    case(KPA_FCT_RESERVED_1):
                    case(KPA_FCT_RESERVED_2):
                    case(KPA_FCT_RESERVED_3):
                    default:
                      break;
                  }
                }
              }
        }
    }

    void handleAsyncUpdate() override
    {
        // This is called on the message loop
        Array<MidiMessage> messages;

        {
            const ScopedLock sl (midiMonitorLock);
            messages.swapWith (incomingMessages);
        }

        String messageText;

        for (auto& m : messages)
            messageText << m.getDescription() << "\n";

        midiMonitor.insertTextAtCaret (messageText);
    }

    void sendToOutputs (const MidiMessage& msg)
    {
        for (auto midiOutput : midiOutputs)
            if (midiOutput->outDevice.get() != nullptr)
                midiOutput->outDevice->sendMessageNow (msg);
    }

    //==============================================================================
    bool hasDeviceListChanged (const Array<MidiDeviceInfo>& availableDevices, bool isInputDevice)
    {
        ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
                                                                                : midiOutputs;

        if (availableDevices.size() != midiDevices.size())
            return true;

        for (auto i = 0; i < availableDevices.size(); ++i)
            if (availableDevices[i] != midiDevices[i]->deviceInfo)
                return true;

        return false;
    }

    ReferenceCountedObjectPtr<MidiDeviceListEntry> findDevice (MidiDeviceInfo device, bool isInputDevice) const
    {
        const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
                                                                                      : midiOutputs;

        for (auto& d : midiDevices)
            if (d->deviceInfo == device)
                return d;

        return nullptr;
    }

    void closeUnpluggedDevices (const Array<MidiDeviceInfo>& currentlyPluggedInDevices, bool isInputDevice)
    {
        ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
                                                                                : midiOutputs;

        for (auto i = midiDevices.size(); --i >= 0;)
        {
            auto& d = *midiDevices[i];

            if (! currentlyPluggedInDevices.contains (d.deviceInfo))
            {
                if (isInputDevice ? d.inDevice .get() != nullptr
                                  : d.outDevice.get() != nullptr)
                    closeDevice (isInputDevice, i);

                midiDevices.remove (i);
            }
        }
    }

    void updateDeviceList (bool isInputDeviceList)
    {
        auto availableDevices = isInputDeviceList ? MidiInput::getAvailableDevices()
                                                  : MidiOutput::getAvailableDevices();

        if (hasDeviceListChanged (availableDevices, isInputDeviceList))
        {

            ReferenceCountedArray<MidiDeviceListEntry>& midiDevices
                = isInputDeviceList ? midiInputs : midiOutputs;

            closeUnpluggedDevices (availableDevices, isInputDeviceList);

            ReferenceCountedArray<MidiDeviceListEntry> newDeviceList;

            // add all currently plugged-in devices to the device list
            for (auto& newDevice : availableDevices)
            {
                
                MidiDeviceListEntry::Ptr entry = findDevice (newDevice, isInputDeviceList);

                if (entry == nullptr)
                    entry = new MidiDeviceListEntry (newDevice);

                /*
                if(0 == entry->deviceInfo.name.compare("KpaMidiBridge"))
                    continue;
                */
                newDeviceList.add (entry);
            }

            // actually update the device list
            midiDevices = newDeviceList;

            // update the selection status of the combo-box
            if (auto* midiSelector = isInputDeviceList ? midiInputSelector.get() : midiOutputSelector.get())
                midiSelector->syncSelectedItemsWithDeviceList (midiDevices);
        }
    }

    //==============================================================================
    void addLabelAndSetStyle (Label& label)
    {
        label.setFont (Font (15.00f, Font::plain));
        label.setJustificationType (Justification::centredLeft);
        label.setEditable (false, false, false);
        label.setColour (TextEditor::textColourId, Colours::black);
        label.setColour (TextEditor::backgroundColourId, Colour (0x00000000));

        addAndMakeVisible (label);
    }

    //==============================================================================
	ApplicationProperties applicationProperties;
    Label midiInputLabel    { "Midi Input Label",  "MIDI Input:" };
    ComboBox midiInputComboBox;
    
    Label midiOutputLabel   { "Midi Output Label", "MIDI Output:" };
    Label incomingMidiLabel { "Incoming Midi Label", "Received MIDI messages:" };
    Label outgoingMidiLabel { "Outgoing Midi Label", "Play the keyboard to send MIDI messages..." };
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent midiKeyboard;
    TextEditor midiMonitor  { "MIDI Monitor" };
    TextButton pairButton   { "MIDI Bluetooth devices..." };
    
    std::unique_ptr<MidiInput> midiIn =  MidiInput::createNewDevice    ("KpaMidiBridge", this);
    std::unique_ptr<MidiOutput> midiOut =  MidiOutput::createNewDevice    ("KpaMidiBridge");

    std::unique_ptr<MidiDeviceListBox> midiInputSelector, midiOutputSelector;
    ReferenceCountedArray<MidiDeviceListEntry> midiInputs, midiOutputs;

    CriticalSection midiMonitorLock;
    Array<MidiMessage> incomingMessages;
    
    EffectButtonSettings I, II, III, IIII;
	Array<EffectButtonSettings*> effectButtonSettings;
    
    double startTime;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KpaMidiBridge)
};
