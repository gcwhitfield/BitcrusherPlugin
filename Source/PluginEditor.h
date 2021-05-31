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
class BitcrusherAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                private juce::Slider::Listener
{
public:
    BitcrusherAudioProcessorEditor (BitcrusherAudioProcessor&);
    ~BitcrusherAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider *slider) override;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BitcrusherAudioProcessor& audioProcessor;

    juce::Slider driveSlider;
    juce::Label driveLabel;
    juce::Slider resolutionSlider;
    juce::Label resolutionLabel;
    juce::Slider downsamplingSlider;
    juce::Label downsamplingLabel;
    juce::Slider mixSlider;
    juce::Label mixLabel;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BitcrusherAudioProcessorEditor)
};
