/*
  ==============================================================================

    Engine.cpp
    Created: 19 Jul 2024 12:32:02am
    Author:  Hack Audio (Computer 1)

  ==============================================================================
*/

#include "Engine.h"

Engine::Engine (juce::AudioProcessorValueTreeState& vts) : apvts (vts) {
    /* Connect ValueTree parameters to local pointer */
    P_vts = apvts.getRawParameterValue ("Cutoff");
}

void Engine::prepare (int new_numChannels, int new_bufferSize, double new_sampleRate) {
    if (numChannels != new_numChannels)
        numChannels = new_numChannels;
    
    if (bufferSize != new_bufferSize)
        bufferSize = new_bufferSize;
    
    if (Fs != new_sampleRate) {
        Fs = new_sampleRate;
        Ts = 1 / Fs;
    }
    
    smooth_alpha = std::exp (-1 * std::log (9) / Fs * smoothTime_seconds);
    
    reset();
}

void Engine::reset () {
    updateParameters_buffer (true);
    updateParameters_sample (true);
}

void Engine::processBuffer (juce::AudioBuffer<float>& buffer) {
    /* Update any per-buffer parameters */
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

void Engine::processBlock (juce::dsp::AudioBlock<float> block) {
    /* Update any per-buffer parameters */
    updateParameters_buffer();
    
    /* Channel Loop */
    for (int channelIndex = 0; channelIndex < block.getNumChannels(); ++channelIndex) {
        /* Sample Loop */
        for (int sampleIndex = 0; sampleIndex < block.getNumSamples(); ++sampleIndex) {
            auto sample = block.getSample (channelIndex, sampleIndex);
            
            block.setSample (channelIndex, sampleIndex, sample);
        }
    }
}

double Engine::processSample (int channel, double sample) {
    updateParameters_sample();
    smoothParameters (channel);
    
    double Il = y1[channel] / R;

    double L = (c * std::pow(N, 2) * S) / std::pow (l * (c + ((b * N)/l) * abs (Il)), 2);

    double b0 = 2 * L / (R * Ts + 2 * L);
    double b1 = -2 * L / (R * Ts + 2 * L);
    double a1 = (R * Ts - 2 * L) / (R * Ts + 2 * L);

    double output = sample * b0 + x1[channel] * b1 - y1[channel] * a1;
    
    y1[channel] = output;
    x1[channel] = sample;
    
    return output;
}

std::pair<double, double> Engine::processSample_stereo (double left_sample, double right_sample) {
    
    return std::make_pair (left_sample, right_sample);
}

void Engine::updateParameters_sample (bool forceUpdate) {
    if (std::abs (P_target - *P_vts) > 0.005 || forceUpdate)
        P_target = *P_vts;
}

void Engine::updateParameters_buffer (bool forceUpdate) {
    
}

void Engine::smoothParameters (int channel) {
    if (P_target != P_smooth[channel]) {
//        if (std::abs (P_target - P_smooth[channel]) > 0.01)
//            P_smooth[channel] = smooth_alpha * P_smooth[channel] + (1 - smooth_alpha) * P_target;
//        else
            P_smooth[channel] = P_target;
        
        R = P_smooth[channel] * 1975000 + 25000;
    }
}
