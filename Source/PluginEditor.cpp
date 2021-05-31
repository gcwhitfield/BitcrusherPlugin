/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BitcrusherAudioProcessorEditor::BitcrusherAudioProcessorEditor (BitcrusherAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 300);
    
    // add listeners to sliders
    driveSlider.addListener(this);
    mixSlider.addListener(this);
    downsamplingSlider.addListener(this);
    resolutionSlider.addListener(this);
    
    // set default values to sliders
    driveSlider.setSliderStyle(juce::Slider::Rotary);
    driveSlider.setRange(0.0, 50.0, 1);
    driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    driveSlider.setPopupDisplayEnabled(true, false, this);
    driveSlider.setTextValueSuffix("dB");
    driveSlider.setValue(0);
    mixSlider.setSliderStyle(juce::Slider::Rotary);
    mixSlider.setRange(0, 100, 1);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    mixSlider.setPopupDisplayEnabled(true, false, this);
    mixSlider.setTextValueSuffix("%");
    mixSlider.setValue(0);
    downsamplingSlider.setSliderStyle(juce::Slider::Rotary);
    downsamplingSlider.setRange(1, 40, 1);
    downsamplingSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    downsamplingSlider.setPopupDisplayEnabled(true, false, this);
    downsamplingSlider.setTextValueSuffix("x");
    downsamplingSlider.setValue(1);
    resolutionSlider.setSliderStyle(juce::Slider::Rotary);
    resolutionSlider.setRange(1, 24, 1);
    resolutionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    resolutionSlider.setPopupDisplayEnabled(true, false, this);
    resolutionSlider.setTextValueSuffix("-bit");
    resolutionSlider.setValue(1);
    addAndMakeVisible(&driveSlider);
    addAndMakeVisible(&mixSlider);
    addAndMakeVisible(&downsamplingSlider);
    addAndMakeVisible(&resolutionSlider);
    
    // add labels to sliders
    addAndMakeVisible(driveLabel);
    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.attachToComponent(&driveSlider,  false);
    addAndMakeVisible(resolutionLabel);
    resolutionLabel.setText("Resolution", juce::dontSendNotification);
    resolutionLabel.attachToComponent(&resolutionSlider, false);
    addAndMakeVisible(mixLabel);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, false);
    addAndMakeVisible(downsamplingLabel);
    downsamplingLabel.setText("Downsample", juce::dontSendNotification);
    downsamplingLabel.attachToComponent(&downsamplingSlider, false);
}

BitcrusherAudioProcessorEditor::~BitcrusherAudioProcessorEditor()
{
}

//==============================================================================
void BitcrusherAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void BitcrusherAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    //sets the positions and size of the slider with arguments (x, y, width, height)
    driveSlider.setBounds(100, 100, 75, 75);
    mixSlider.setBounds(200, 100, 75, 75);
    downsamplingSlider.setBounds(300, 100, 75, 75);
    resolutionSlider.setBounds(400, 100, 75, 75);
}

void BitcrusherAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    *(audioProcessor.downsampleAmt) = (int)downsamplingSlider.getValue();
    *(audioProcessor.mix) = mixSlider.getValue() / 100.0;
    *(audioProcessor.drive) = driveSlider.getValue();
    *(audioProcessor.resolution) = (int)resolutionSlider.getValue();
}
