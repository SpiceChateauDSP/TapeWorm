/*
  ==============================================================================

    Engine.cpp
    Created: 19 Jul 2024 12:32:02am
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#include "Engine.h"

//Engine::Engine (juce::AudioProcessorValueTreeState& vts) : apvts (vts) {
//}
//
//void Engine::prepare (int new_numChannels, int new_bufferSize, double new_sampleRate) {
//    if (numChannels != new_numChannels)
//        numChannels = new_numChannels;
//    
//    if (bufferSize != new_bufferSize)
//        bufferSize = new_bufferSize;
//    
//    if (Fs != new_sampleRate) {
//        Fs = new_sampleRate;
//        Ts = 1 / Fs;
//    }
//    
//    dcBlocker.prepare (numChannels, bufferSize, Fs);
//    dampingFilter.prepare (numChannels, bufferSize, Fs);
//    
//    smooth_alpha = std::exp (-1 * std::log (9) / Fs * smoothTime_seconds);
//    
//    reset();
//}
//
//void Engine::reset () {
//    updateParameters_buffer (true);
//    updateParameters_sample (true);
//    
//    dcBlocker.reset();
//    dampingFilter.reset();
//}
//
//void Engine::processBuffer (juce::AudioBuffer<float>& buffer) {
//    /* Update any per-buffer parameters */
//    updateParameters_buffer();
//    
//    dcBlocker.updateParameters_buffer();
//    dampingFilter.updateParameters_buffer();
//    
//    /* Channel Loop */
//    for (int channelIndex = 0; channelIndex < buffer.getNumChannels(); ++channelIndex) {
//        auto* channelData = buffer.getWritePointer (channelIndex);
//        
//        /* Sample Loop */
//        for (int sampleIndex = 0; sampleIndex < buffer.getNumSamples(); ++sampleIndex) {
//            auto sample = channelData[sampleIndex];
//            
//            channelData[sampleIndex] = processSample (channelIndex, sample);
//        }
//    }
//}
//
//void Engine::processBlock (juce::dsp::AudioBlock<float> block) {
//    /* Update any per-buffer parameters */
//    updateParameters_buffer();
//    
//    dcBlocker.updateParameters_buffer();
//    dampingFilter.updateParameters_buffer();
//    
//    /* Channel Loop */
//    for (int channelIndex = 0; channelIndex < block.getNumChannels(); ++channelIndex) {
//        /* Sample Loop */
//        for (int sampleIndex = 0; sampleIndex < block.getNumSamples(); ++sampleIndex) {
//            auto sample = block.getSample (channelIndex, sampleIndex);
//            
//            block.setSample (channelIndex, sampleIndex, sample);
//        }
//    }
//}
//
//double Engine::processSample (int channel, double sample) {
//    updateParameters_sample();
//    smoothParameters (channel);
//    
//    auto sample_hpf = dcBlocker.processSample (channel, sample);
//    auto output = dampingFilter.processSample (channel, sample_hpf);
//    
//    return output;
//}
//
//std::pair<double, double> Engine::processSample_stereo (std::pair<double, double> input) {
//    auto output = input;
//    return output;
//}
//
//void Engine::updateParameters_sample (bool forceUpdate) {
//
//}
//
//void Engine::updateParameters_buffer (bool forceUpdate) {
//    
//}
//
//void Engine::smoothParameters (int channel) {
//    
//}


double Engine::processSample (int channel, double sample) {
    updateParameters_sample ();
    smoothParameters (channel);
    
//    auto sample_hpf = dcBlocker.processSample (channel, sample);
//    auto output = dampingFilter.processSample (channel, sample_hpf);
    
    return sample;
}

void Engine::updateParameters_sample () {
    damping.update();
    
    /* Nothing here should change the latency, but this is how it might look if you need it.
     The parent class should check for latencyFlag, and call getLatency() if the flag is true */
    gen_vars.latencyFlag = (gen_vars.cLatency != gen_vars.mLatency);
}
