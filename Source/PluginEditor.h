/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
//==============================================================================
/**
*/
class SimpleAudioInputPluginAudioProcessorEditor  : 
    public juce::AudioProcessorEditor, 
    private juce::Button::Listener,
    private juce::ChangeListener
{
public:
    SimpleAudioInputPluginAudioProcessorEditor (SimpleAudioInputPluginAudioProcessor&);
    ~SimpleAudioInputPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleAudioInputPluginAudioProcessor& audioProcessor;
    juce::ScopedPointer<AudioThumbnailComp> thumbnail;

    juce::TextButton startStopButton;

    void buttonClicked(juce::Button* buttonThatWasClicked) override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleAudioInputPluginAudioProcessorEditor)
};
