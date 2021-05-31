/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BitcrusherAudioProcessor::BitcrusherAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(mix = new juce::AudioParameterFloat("mix", // parameter ID,
                                                    "Mix", // parameter name
                                                     0.0,    // min value
                                                     100.0,  // max value
                                                     0.0));  // default value
    addParameter(downsampleAmt = new juce::AudioParameterInt("downampleAmt",
                                                    "Downsample",
                                                     1,
                                                     40,
                                                     1));
    addParameter(drive = new juce::AudioParameterFloat("drive",
                                                    "Drive",
                                                     0,
                                                     50,
                                                     0));
    addParameter(resolution = new juce::AudioParameterInt("resolution",
                                                    "Resolution",
                                                     1,
                                                     24,
                                                     1));
}

BitcrusherAudioProcessor::~BitcrusherAudioProcessor()
{
}

//==============================================================================
const juce::String BitcrusherAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BitcrusherAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BitcrusherAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BitcrusherAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BitcrusherAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BitcrusherAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BitcrusherAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BitcrusherAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BitcrusherAudioProcessor::getProgramName (int index)
{
    return {};
}

void BitcrusherAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BitcrusherAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void BitcrusherAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BitcrusherAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void BitcrusherAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int M = *downsampleAmt;

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    // ================= Drive ===============
    juce::dsp::Gain<float> newGain;
    newGain.setGainDecibels(*drive);
    buffer.applyGain(newGain.getGainDecibels() * (*mix) + 1);
    
    float dry, wet = 0.0;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        int j = -M; // equal to ((current index in channelData) // M) * getNumSamples(), where M is the
                    // downsampling factor
        
        auto* channelData = buffer.getWritePointer (channel);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            dry = channelData[i];
            // ============= Downsampling ===============
            // downsample the signal by a factor of M by replacing each M consecutive
            // samples in channelData the M'th sample. In other words, Throw away
            // whose indicies are not a multiple of M and replace it with
            // channelData[M]
            if (i % M == 0) j += M;
            wet = channelData[j];
            // ============= Resolution ================
            // represent each sample using 'resolution' bits rather than 32 bits. This assumes that
            // all of the samples in the channelData have exponents which are set to 0 (i.e the
            // sample values are all beteen -1 and 1)
            union {float f; int k;} num;
            num.f = wet;
            int s = 24 - *resolution;
            num.k = (num.k >> s) << s;
            wet = num.f;
            // ================= Mix ===============
            // channelData[i] is updated with a linear combination of the dry and wet signals
            channelData[i] = dry * (1.0 - (*mix)) + wet * (*mix);
            
            
        }
    }
    
}

//==============================================================================
bool BitcrusherAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BitcrusherAudioProcessor::createEditor()
{
    return new BitcrusherAudioProcessorEditor (*this);
}

//==============================================================================
void BitcrusherAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BitcrusherAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BitcrusherAudioProcessor();
}
