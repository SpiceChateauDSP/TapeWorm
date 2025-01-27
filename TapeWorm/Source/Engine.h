/*
  ==============================================================================

    Engine.h
    Created: 19 Jul 2024 12:32:02am
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Inductor First Order.h"
#include "CubicDelay.h"

class Engine {
public:
    Engine (juce::AudioProcessorValueTreeState& vts);
    ~Engine () {};
    
    void prepare (const float new_sampleRate, const int new_bufferSize, const int new_numChannels);
    void reset ();
    
    /* Processing */
    void processBuffer_inPlace (juce::AudioBuffer<float>& buffer);
    inline double processSample (const int channelIndex, const double sample);
    
    /* Latency */
    const bool checkLatency () { return latencyFlag; };
    const int getLatencySamples () {
        mLatency = cLatency;
        latencyFlag = false;
        return cLatency;
    };

private:
    /* Process Variables */
    float sampleRate = 48000.f;
    double samplePeriod = 1 / sampleRate;
    int bufferSize = 128;
    int numChannels = 2;
    
    /* Latency */
    int cLatency = 0, mLatency = 0;
    bool latencyFlag = false;
    
    /* Parameters */
    juce::AudioProcessorValueTreeState& apvts;
    void updateParameters_sample ();
    void updateParameters_buffer ();
    void updateParameters_forced ();
    void smoothParameters (const int channelIndex);
    
    struct {
        std::atomic<float>* damping = nullptr;
    } tree;
    
    struct {
        float damping = -1.f;
    } local;

    double smoothTime_seconds = 0.1f; // 100 ms smoothing
    double smooth_alpha = 1.f;
    
    /* Filters */
    Inductor_FirstOrder dcBlocker { Inductor_FirstOrder::FilterType::HPF };
    Inductor_FirstOrder damping { Inductor_FirstOrder::FilterType::LPF };
    
    /* Delay */
    CubicDelay delay;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Engine);
};
