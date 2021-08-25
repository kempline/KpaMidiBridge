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
        messageTypeComboBox.addItem("Program Change (PC)", 1);
        messageTypeComboBox.addItem("Control Change (CC)", 2);
        messageTypeComboBox.addItem("Midi Machine Control (MMC)", 3);
        messageTypeComboBox.onChange = [this] { onMessageTypeChange(); };
        
        addAndMakeVisible (param1Label);
        addAndMakeVisible (param1ComboBox);
        param1Label.setVisible(false);
        param1ComboBox.setVisible(false);
        
        addAndMakeVisible (param2Label);
        addAndMakeVisible (param2ComboBox);
        param2Label.setVisible(false);
        param2ComboBox.setVisible(false);
        
        addAndMakeVisible (midiChannelLabel);
        addAndMakeVisible (midiChannelComboBox);
        midiChannelLabel.setVisible(false);
        midiChannelComboBox.setVisible(false);
        
        for(uint8 i=0; i<16; i++) {
            midiChannelComboBox.addItem(String(i+1), i+1);
        }
    }
    void resized() override {
        effectButtonLabel.setBounds (10, 10, 300, 20);
        
        messageTypeLabel.setBounds (10, 30, 100, 20);
        messageTypeComboBox.setBounds (100, 30, 200, 20);
        
        midiChannelLabel.setBounds (10, 55, 100, 20);
        midiChannelComboBox.setBounds (100, 55, 200, 20);
        
        param1Label.setBounds (10, 80, 100, 20);
        param1ComboBox.setBounds (100, 80, 200, 20);
        
        param2Label.setBounds (10, 105, 100, 20);
        param2ComboBox.setBounds (100, 105, 200, 20);
        
        this->setColour (juce::Label::textColourId, juce::Colours::orange);
    }
    
    void onMessageTypeChange() {
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
    
private:
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
