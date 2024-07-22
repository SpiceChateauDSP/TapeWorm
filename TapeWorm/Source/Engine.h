/*
  ==============================================================================

    Engine.h
    Created: 19 Jul 2024 12:32:02am
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Engine {
public:
    Engine (juce::AudioProcessorValueTreeState& );
    ~Engine () {};
    
    void prepare (int new_numChannels, int new_bufferSize, double new_sampleRate);
    void reset ();
    
    void processBuffer (juce::AudioBuffer<float>& buffer);
    void processBlock (juce::dsp::AudioBlock<float> block);
    double processSample (int channel, double sample);
    std::pair<double, double> processSample_stereo (double left_sample, double right_sample);
    
    void updateParameters_sample (bool forceUpdate = false);
    void updateParameters_buffer (bool forceUpdate = false);
    
    void smoothParameters (int channel);
    
private:
    juce::AudioProcessorValueTreeState& apvts;
    
    double Fs = 48000;
    double Ts = 1 / Fs;
    int numChannels = 2;
    int bufferSize = 128;
    
    const double Ui = 400;
    const double Uo = 4 * juce::MathConstants<double>::pi * 10e-7;
    const double Bsat = 0.15;
    const double N = 1000;
    const double S = 1;
    const double l = 2;
    
    const double u = Ui * Uo;
    const double b = (1 - sqrt (1/Ui))/ Bsat;
    const double c = 1 / u;
    
    std::atomic<float>* P_vts;
    float P_target = -1.f;
    double P_smooth[2] {0};
    double R = 25000;
    
    double x1[2] {0};
    double y1[2] {0};
    
    float smoothTime_seconds = 0.1f; // 100 ms
    double smooth_alpha = std::exp (-1 * std::log (9) / (Fs * smoothTime_seconds));
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Engine);
};
