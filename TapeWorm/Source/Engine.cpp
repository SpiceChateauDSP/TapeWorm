/*
  ==============================================================================

    Engine.cpp
    Created: 19 Jul 2024 12:32:02am
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#include "Engine.h"

Engine::Engine (juce::AudioProcessorValueTreeState& vts) : apvts (vts) {
    tree.damping = apvts.getRawParameterValue ("Damping");
}

void Engine::prepare (const float new_sampleRate, const int new_bufferSize, const int new_numChannels) {
    /* Processing Variables */
    if (numChannels != new_numChannels)
        numChannels = new_numChannels;
    
    if (bufferSize != new_bufferSize)
        bufferSize = new_bufferSize;
    
    if (sampleRate != new_sampleRate) {
        sampleRate = new_sampleRate;
        samplePeriod = 1.f / sampleRate;
    }
    
    /* Processing Classes */
    dcBlocker.prepare (sampleRate, bufferSize);
    damping.prepare (sampleRate, bufferSize);
    
    delay.setSize (2 * sampleRate);
    delay.setDelayTime (0.003f);
    delay.setModRefreshRate (std::round (sampleRate / 50));
    delay.setModRate (5.f);
    delay.setModDepth (0.5f);
    delay.setModShape (0.f);
    delay.setModStereo (true);
    delay.setModInverted (false);
    delay.setRandomRefreshRate (std::round (sampleRate));
    delay.setRandomDepth (0.f);
    delay.setRandomSmooth (1.f);
    
    dcBlocker.setPotentiometer (1.f);
    damping.setPotentiometer (1.f);
    
    /* Parameter Variables */
    smooth_alpha = std::exp (-1 * std::log (9) / sampleRate * smoothTime_seconds);
    
    reset();
}

void Engine::reset () {
    updateParameters_forced ();
    
    dcBlocker.reset();
    damping.reset();
}

void Engine::processBuffer_inPlace (juce::AudioBuffer<float>& buffer) {
    /* Update any parameters that need to be set each buffer */
    updateParameters_buffer();
    
    /* Channel Loop */
    for (int channelIndex = 0; channelIndex < buffer.getNumChannels(); ++channelIndex) {
        auto* channelData = buffer.getWritePointer (channelIndex);
        
        /* Sample Loop */
        for (int sampleIndex = 0; sampleIndex < buffer.getNumSamples(); ++sampleIndex) {
            auto sample = channelData[sampleIndex];
            
            channelData[sampleIndex] = processSample (channelIndex, sample);
        }
    }
}

inline double Engine::processSample (const int channelIndex, const double sample) {
    /* Update any parameters that can be set every sample */
    updateParameters_sample ();
    
    /* Smooth any parameters that need it */
    smoothParameters (channelIndex);
    
    auto sample_dc = dcBlocker.processSample (channelIndex, sample);
    auto sample_delay = delay.processSample (channelIndex, sample_dc);
    auto sample_damp = damping.processSample (channelIndex, sample_delay);
    
    return 0.5 * (sample + sample_damp);
}

void Engine::updateParameters_sample () {
    auto value = tree.damping->load (std::memory_order_acquire);
    if (!juce::approximatelyEqual (local.damping, value))
        damping.setPotentiometer (value);
    
    /* The parent class should check for latencyFlag, and call getLatency() if the flag is true */
    latencyFlag = (cLatency != mLatency);
}

void Engine::updateParameters_buffer () {

}

void Engine::updateParameters_forced () {
    
}

void Engine::smoothParameters (const int channelIndex) {
    
}
