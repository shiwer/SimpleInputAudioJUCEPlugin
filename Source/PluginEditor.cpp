/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AudioThumbnailComp.h"

//==============================================================================
SimpleAudioInputPluginAudioProcessorEditor::SimpleAudioInputPluginAudioProcessorEditor(SimpleAudioInputPluginAudioProcessor& p) :
    AudioProcessorEditor(&p),
    audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //thumbnailCache = juce::AudioThumbnailCache(1);
    thumbnail = new AudioThumbnailComp(audioProcessor.formatManager, audioProcessor.transportSource, audioProcessor.thumbnailCache, audioProcessor.currentlyLoadedFile);
    addAndMakeVisible(thumbnail);
    thumbnail->addChangeListener(this);

    addAndMakeVisible(startStopButton);
    startStopButton.setButtonText("Play/Stop");
    startStopButton.addListener(this);
    startStopButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff79ed7f));

    setOpaque(true);

    setSize(512, 220);
}

SimpleAudioInputPluginAudioProcessorEditor::~SimpleAudioInputPluginAudioProcessorEditor()
{
}

//==============================================================================
void SimpleAudioInputPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::grey);
 
}

void SimpleAudioInputPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    juce::Rectangle<int> r(getLocalBounds().reduced(4));

    juce::Rectangle<int> controls(r.removeFromBottom(32));

    startStopButton.setBounds(controls);

    r.removeFromBottom(6);
    thumbnail->setBounds(r.removeFromBottom(180));
    r.removeFromBottom(6);

}

void SimpleAudioInputPluginAudioProcessorEditor::buttonClicked (juce::Button* buttonThatWasClicked) 
{
    if (buttonThatWasClicked == &startStopButton)
    {
        if (audioProcessor.transportSource.isPlaying())
        {
            audioProcessor.transportSource.stop();
        }
        else
        {
            audioProcessor.transportSource.setPosition(0);
            audioProcessor.transportSource.start();
        }
    }
}

void SimpleAudioInputPluginAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source) 
{
    if (source == thumbnail)
    {
        audioProcessor.loadFileIntoTransport(thumbnail->getLastDroppedFile());
        thumbnail->setFile(thumbnail->getLastDroppedFile());
    }
}

