/*
  ==============================================================================

    EffectButtonSettings.h
    Created: 25 Aug 2021 9:27:01pm
    Author:  Sparenberg, Heiko

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class EffectButtonSettings : public juce::GroupComponent
{
public:
    EffectButtonSettings(String EffectButtonLabel) : label(EffectButtonLabel) {
        //addAndMakeVisible (effectButtonLabel);
        //effectButtonLabel.setColour (juce::Label::textColourId, juce::Colours::orange);
        //effectButtonLabel.setFont (22.0f);
        //effectButtonLabel.setJustificationType (juce::Justification::right);
        this->setText(label);
        this->setTextLabelPosition(juce::Justification::right);
        this->setTextLabelPosition(juce::Justification::left);
        this->setColour (juce::GroupComponent::textColourId, juce::Colours::orange);
        this->setColour (juce::GroupComponent::outlineColourId, juce::Colours::orange);
        
        addAndMakeVisible (messageTypeLabel);
        addAndMakeVisible (messageTypeComboBox);
		messageTypeComboBox.addItem("None", 99);
        messageTypeComboBox.addItem("Program Change (PC)", 1);
        messageTypeComboBox.addItem("Control Change (CC)", 2);
        messageTypeComboBox.addItem("Midi Machine Control (MMC)", 3);
        messageTypeComboBox.onChange = [this] { onMessageTypeChange(); };
        
        addAndMakeVisible (param1Label);
        addAndMakeVisible (param1ComboBox);
        param1Label.setVisible(false);
        param1ComboBox.setVisible(false);
		param1ComboBox.onChange = [this] { onParam1ComboBoxChange(); };
        
        addAndMakeVisible (param2Label);
        addAndMakeVisible (param2ComboBox);
        param2Label.setVisible(false);
        param2ComboBox.setVisible(false);
		param2ComboBox.onChange = [this] { onParam2ComboBoxChange(); };
        
        addAndMakeVisible (midiChannelLabel);
        addAndMakeVisible (midiChannelComboBox);
        midiChannelLabel.setVisible(false);
        midiChannelComboBox.setVisible(false);
		midiChannelComboBox.onChange = [this] { onMidiChannelComboBoxChange(); };
        
        for(uint8 i=0; i<16; i++) {
            midiChannelComboBox.addItem(String(i+1), i+1);
        }
    }
	
	void setApplicationProperties(ApplicationProperties *pApplicationProperties) {
		applicationProperties = pApplicationProperties;
	}
	
	void restoreUiSettings() {
		auto messageTypeSelection = applicationProperties->getUserSettings()->getIntValue(label + "-messageType");
		if(0 != messageTypeSelection) {
			messageTypeComboBox.setSelectedId(messageTypeSelection);
			// onMessageTypeChange();
		}
	}
	
	void restoreUiSettingsEx() {
		auto param1Selection = applicationProperties->getUserSettings()->getIntValue(label + "-param1");
		if(0 != param1Selection) {
			param1ComboBox.setSelectedId(param1Selection);
		}
		
		auto param2Selection = applicationProperties->getUserSettings()->getIntValue(label + "-param2");
		if(0 != param2Selection) {
			param2ComboBox.setSelectedId(param2Selection);
		}
		
		auto midiChannelSelection = applicationProperties->getUserSettings()->getIntValue(label + "-midiChannel");
		if(0 != midiChannelSelection) {
			midiChannelComboBox.setSelectedId(midiChannelSelection);
		}
	}
	
    void resized() override {
		auto margin = 10;
		
		auto area = getLocalBounds();
		auto quarterHeight = area.getHeight() / 4;
		
		Rectangle<int> messageTypeComboBoxArea =  area.removeFromTop(quarterHeight);
		
		
		Rectangle<int> messageTypeLabelArea =  messageTypeComboBoxArea.removeFromLeft(area.getWidth() / 2);
		messageTypeLabelArea.removeFromLeft(margin);
		messageTypeLabelArea.removeFromTop(margin);
		messageTypeLabelArea.removeFromBottom(margin/2);
		messageTypeLabelArea.removeFromRight(margin);
		
		messageTypeComboBoxArea.removeFromLeft(margin);
		messageTypeComboBoxArea.removeFromTop(margin);
		messageTypeComboBoxArea.removeFromBottom(margin/2);
		messageTypeComboBoxArea.removeFromRight(margin);
		
		Rectangle<int> midiChannelComboBoxArea =  area.removeFromTop(quarterHeight);
		Rectangle<int> midiChannelLabelArea =  midiChannelComboBoxArea.removeFromLeft(area.getWidth() / 2);
		midiChannelLabelArea.removeFromLeft(margin);
		midiChannelLabelArea.removeFromTop(margin/2);
		midiChannelLabelArea.removeFromBottom(margin/2);
		midiChannelLabelArea.removeFromRight(margin);
		
		midiChannelComboBoxArea.removeFromLeft(margin);
		midiChannelComboBoxArea.removeFromTop(margin/2);
		midiChannelComboBoxArea.removeFromBottom(margin/2);
		midiChannelComboBoxArea.removeFromRight(margin);
		
		Rectangle<int> param1ComboBoxArea =  area.removeFromTop(quarterHeight);
		Rectangle<int> param1LabelArea =  param1ComboBoxArea.removeFromLeft(area.getWidth() / 2);
		param1LabelArea.removeFromLeft(margin);
		param1LabelArea.removeFromTop(margin/2);
		param1LabelArea.removeFromBottom(margin/2);
		param1LabelArea.removeFromRight(margin);
		
		param1ComboBoxArea.removeFromLeft(margin);
		param1ComboBoxArea.removeFromTop(margin/2);
		param1ComboBoxArea.removeFromBottom(margin/2);
		param1ComboBoxArea.removeFromRight(margin);
		
		Rectangle<int> param2ComboBoxArea =  area.removeFromTop(quarterHeight);
		Rectangle<int> param2LabelArea =  param2ComboBoxArea.removeFromLeft(area.getWidth() / 2);
		param2LabelArea.removeFromLeft(margin);
		param2LabelArea.removeFromTop(margin/2);
		param2LabelArea.removeFromBottom(margin/2);
		param2LabelArea.removeFromRight(margin);
		
		param2ComboBoxArea.removeFromLeft(margin);
		param2ComboBoxArea.removeFromTop(margin/2);
		param2ComboBoxArea.removeFromBottom(margin/2);
		param2ComboBoxArea.removeFromRight(margin);
		
		messageTypeLabel.setBounds (messageTypeLabelArea);
        messageTypeComboBox.setBounds (messageTypeComboBoxArea);
        
        midiChannelLabel.setBounds (midiChannelLabelArea);
        midiChannelComboBox.setBounds (midiChannelComboBoxArea);
        
        param1Label.setBounds (param1LabelArea);
        param1ComboBox.setBounds (param1ComboBoxArea);
        
        param2Label.setBounds (param2LabelArea);
        param2ComboBox.setBounds (param2ComboBoxArea);
        
        this->setColour (juce::Label::textColourId, juce::Colours::orange);
		
		if(applicationProperties)
			restoreUiSettings();
    }
	
	void onMidiChannelComboBoxChange() {
		printf("onMidiChannelComboBoxChange for %s, value=%d\n", label.toStdString().c_str(), midiChannelComboBox.getSelectedId());
		if(applicationProperties) {
			applicationProperties->getUserSettings()->setValue(label + "-midiChannel", midiChannelComboBox.getSelectedId());
			applicationProperties->saveIfNeeded();
		}
	}
	
	void onParam2ComboBoxChange() {
		printf("onParam2ComboBoxChange for %s, value=%d\n", label.toStdString().c_str(), param2ComboBox.getSelectedId());
		if(applicationProperties) {
			applicationProperties->getUserSettings()->setValue(label + "-param2", param2ComboBox.getSelectedId());
			applicationProperties->saveIfNeeded();
		}
	}
	
	void onParam1ComboBoxChange() {
		printf("onParam1ComboBoxChange for %s, value=%d\n", label.toStdString().c_str(), param1ComboBox.getSelectedId());
		if(applicationProperties) {
			applicationProperties->getUserSettings()->setValue(label + "-param1", param1ComboBox.getSelectedId());
			applicationProperties->saveIfNeeded();
		}
	}
	
    void onMessageTypeChange() {
		printf("onMessageTypeChange for %s, value=%d\n", label.toStdString().c_str(), messageTypeComboBox.getSelectedId());
		if(applicationProperties) {
			applicationProperties->getUserSettings()->setValue(label + "-messageType", messageTypeComboBox.getSelectedId());
			applicationProperties->saveIfNeeded();
		}
        
		param1ComboBox.clear();
        param2ComboBox.clear();
        
        switch (messageTypeComboBox.getSelectedId()) {
            case 1:
                param1Label.setVisible(true);
                param1ComboBox.setVisible(true);
                for(uint8 i=0; i<127; i++) {
                    param1ComboBox.addItem(String(i+1), i+1);
                }
                
                param1Label.setText("Program: ", NotificationType::dontSendNotification);
                param2Label.setVisible(false);
                param2ComboBox.setVisible(false);
                midiChannelLabel.setVisible(true);
                midiChannelComboBox.setVisible(true);
				restoreUiSettingsEx();
                break;
            case 2:
                param1Label.setVisible(true);
                param1ComboBox.setVisible(true);
                param1Label.setText("Controller: ", NotificationType::dontSendNotification);
                param2Label.setVisible(true);
                param2Label.setText("Value: ", NotificationType::dontSendNotification);
                param2ComboBox.setVisible(true);
                for(uint8 i=0; i<127; i++) {
                    param1ComboBox.addItem(String(i+1), i+1);
                    param2ComboBox.addItem(String(i+1), i+1);
                }
                midiChannelLabel.setVisible(true);
                midiChannelComboBox.setVisible(true);
				restoreUiSettingsEx();
                break;
            case 3:
				param1ComboBox.setVisible(true);
                param1Label.setVisible(true);
                param1Label.setText("Control: ", NotificationType::dontSendNotification);
                param2Label.setVisible(false);
                param2ComboBox.setVisible(false);
                param1ComboBox.addItem("Stop", 1);
                param1ComboBox.addItem("Play", 2);
                param1ComboBox.addItem("Deferred Play (play after no longer busy)", 3);
                param1ComboBox.addItem("Fast Forward", 4);
                param1ComboBox.addItem("Rewind", 5);
                param1ComboBox.addItem("Record Strobe (AKA [[Punch in/out|Punch In]])", 6);
                param1ComboBox.addItem("Record Exit (AKA [[Punch out (music)|Punch out]]", 7);
                param1ComboBox.addItem("Record Pause", 8);
                param1ComboBox.addItem("Pause (pause playback)", 9);
                midiChannelLabel.setVisible(true);
                midiChannelComboBox.setVisible(true);
				restoreUiSettingsEx();
                break;
            default:
                param1Label.setVisible(false);
                param1ComboBox.setVisible(false);
                
                param2Label.setVisible(false);
                param2ComboBox.setVisible(false);
                
                midiChannelLabel.setVisible(false);
                midiChannelComboBox.setVisible(false);
                break;
        }
    }
	MidiMessage getMidiMessage() {
		
		switch(messageTypeComboBox.getSelectedItemIndex()) {
			case 1: {
				
				MidiMessage m = MidiMessage::programChange(
														   midiChannelComboBox.getSelectedItemIndex(),
														   param1ComboBox.getSelectedItemIndex());
				return m;
			}
			case 2: {
				MidiMessage m = MidiMessage::MidiMessage::controllerEvent(
														   midiChannelComboBox.getSelectedItemIndex(),
														   param1ComboBox.getSelectedItemIndex(),
														   param2ComboBox.getSelectedItemIndex());
				return m;
			}
				
			case 99:
			default: {
				return MidiMessage::MidiMessage::midiContinue();
				break;
			}
				
		}
		
	}
	
private:
	ApplicationProperties *applicationProperties = NULL;
    String label;
    
    Label effectButtonLabel   { "Effect Button Label",  label + "\t"};
    
    Label messageTypeLabel   { "Message Type Label",  "Type: " };
    ComboBox messageTypeComboBox;
    
    Label param1Label   { "Parameter 1 Label",  "Parameter 1: " };
    ComboBox param1ComboBox;
    
    Label param2Label   { "Parameter 2 Label",  "Parameter 2: " };
    ComboBox param2ComboBox;
    
    Label midiChannelLabel   { "Midi Channel Label",  "Midi Channel: " };
    ComboBox midiChannelComboBox;
    
    //Label midiInputLabel2    { "Test Label2",  "Test Label:" + label };
};
