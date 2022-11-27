/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleAudioInputPluginAudioProcessor::SimpleAudioInputPluginAudioProcessor() :
    AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo())),
    thumbnailCache(1),
    readAheadThread("transport read ahead")
{
    formatManager.registerBasicFormats();
    readAheadThread.startThread(juce::Thread::Priority::high);
}
SimpleAudioInputPluginAudioProcessor::~SimpleAudioInputPluginAudioProcessor()
{
    transportSource.setSource(nullptr);
}

//==============================================================================
const juce::String SimpleAudioInputPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleAudioInputPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleAudioInputPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleAudioInputPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleAudioInputPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleAudioInputPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleAudioInputPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleAudioInputPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleAudioInputPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleAudioInputPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleAudioInputPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    transportSource.prepareToPlay(samplesPerBlock, sampleRate);
}

void SimpleAudioInputPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleAudioInputPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SimpleAudioInputPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    for(int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    transportSource.getNextAudioBlock(juce::AudioSourceChannelInfo(buffer));
}

//==============================================================================
bool SimpleAudioInputPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleAudioInputPluginAudioProcessor::createEditor()
{
    return new SimpleAudioInputPluginAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleAudioInputPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::XmlElement xml("plugin-settings");

    xml.setAttribute("audiofile", currentlyLoadedFile.getFullPathName());

    copyXmlToBinary(xml, destData);
}

void SimpleAudioInputPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
   std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName("plugin-settings"))
        {
            currentlyLoadedFile = juce::File::createFileWithoutCheckingPath(xmlState->getStringAttribute("audiofile"));
            if (currentlyLoadedFile.existsAsFile())
            {
                loadFileIntoTransport(currentlyLoadedFile);
            }
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleAudioInputPluginAudioProcessor();
}

void SimpleAudioInputPluginAudioProcessor::loadFileIntoTransport(const juce::File& audioFile)
{
    // unload the previous file source and delete it..
    transportSource.stop();
    transportSource.setSource(nullptr);
    currentAudioFileSource = nullptr;

    juce::AudioFormatReader* reader = formatManager.createReaderFor(audioFile);
    currentlyLoadedFile = audioFile;

    if (reader != nullptr)
    {
        currentAudioFileSource = new juce::AudioFormatReaderSource(reader, true);

        // ..and plug it into our transport source
        transportSource.setSource(
            currentAudioFileSource,
            32768,                   // tells it to buffer this many samples ahead
            &readAheadThread,        // this is the background thread to use for reading-ahead
            reader->sampleRate);     // allows for sample rate correction
    }
}

