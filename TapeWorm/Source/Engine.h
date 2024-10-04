/*
  ==============================================================================

    Engine.h
    Created: 19 Jul 2024 12:32:02am
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
//#include "Inductor First Order.h"

//class Engine {
//public:
//    Engine (juce::AudioProcessorValueTreeState& );
//    ~Engine () {};
//    
//    void prepare (int new_numChannels, int new_bufferSize, double new_sampleRate);
//    void reset ();
//    
//    void processBuffer (juce::AudioBuffer<float>& buffer);
//    void processBlock (juce::dsp::AudioBlock<float> block);
//    double processSample (int channel, double sample);
//    std::pair<double, double> processSample_stereo (std::pair<double, double> input);
//    
//    void updateParameters_sample (bool forceUpdate = false);
//    void updateParameters_buffer (bool forceUpdate = false);
//    
//    void smoothParameters (int channel);
//    
//private:
//    juce::AudioProcessorValueTreeState& apvts;
//    
//    Inductor_FirstOrder dcBlocker {apvts, HPF, false};
//    Inductor_FirstOrder dampingFilter {apvts, LPF, true};
//    
//    double Fs = 48000;
//    double Ts = 1 / Fs;
//    int numChannels = 2;
//    int bufferSize = 128;
//    
//    float smoothTime_seconds = 0.1f; // 100 ms
//    double smooth_alpha = std::exp (-1 * std::log (9) / (Fs * smoothTime_seconds));
//    
//    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Engine);
//};

class Engine : public HackAudio::GenericProcessor {
public:
    Engine (juce::AudioProcessorValueTreeState* vts) {
        fillParameters ();
        connectValueTreeParameters (vts);
    };
    ~Engine () {};
    
    double processSample (int channel, double sample) override;
    void fillParameters () override {
        gen_vars.parameters.add (&damping);
    };

private:
    void updateParameters_sample () override;
    
    HackAudio::Parameter damping { "Damping", [&] (float n) { return n; }};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Engine);
};
