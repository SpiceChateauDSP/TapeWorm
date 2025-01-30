/*
  ==============================================================================

    CubicDelay.h
    Created: 24 Jun 2024 9:40:48pm
    Author:  SpiceChateauDSP

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class CubicDelay {
public:
    CubicDelay () {}
    ~CubicDelay () {}
    
    void reset () {
        updateParameters_forced();
        
        delayLine[0].fill (0);
        delayLine[1].fill (0);
        theta[0] = theta[1] = 0;
    }
    
    inline double processSample (const int channelIndex, const double sample) {
        /* Smooth Parameters */
        smoothParameters_sample (channelIndex);
        
        /* Write sample to delay */
        writeIndex[channelIndex]++;
        if (writeIndex[channelIndex] > size - 1)
            writeIndex[channelIndex] = 0;
        
        delayLine[channelIndex].set (writeIndex[channelIndex], sample);
        
        /* Find delay time after updating LFOs */
        auto delayTime = getDelayTime (channelIndex);
        
        /* Get fractional delay times */
        int iDel = std::floor (delayTime);
        double frac = delayTime - iDel;
        
        /* Interpolate and return delayed signal */
        auto readPointers = findReadPointers (channelIndex, iDel);
        return interpolate (readPointers, channelIndex, frac);
    }
    
    inline double getDelayTime (int channelIndex) {
        /* Check if necessary to update LFO */
        countM[channelIndex]++;
        if (countM[channelIndex] > MAXCOUNT_M - 1) {
            countM[channelIndex] = 0;
            
            double rotation_sin = (isStereo) ? theta[channelIndex] + channelIndex * halfPi : theta[channelIndex];
            double rotation_tri = (isStereo) ? theta[channelIndex] + (1 - channelIndex) * halfPi : theta[channelIndex];
             
            auto p1 = amplitudeM_smooth[channelIndex] * sin_lfo (rotation_sin) + delayTime_samples;
            auto p2 = amplitudeM_smooth[channelIndex] * tri_lfo (rotation_tri) + delayTime_samples;
            
            position[channelIndex] = shape * (p2 - p1) + p1;

            /* Update Theta */
            theta[channelIndex] += phi;
            if (theta[channelIndex] > (2 * pi))
                theta[channelIndex] -= 2 * pi;
        
        }
        
        /* Smooth LFO */
        positionSmoothed[channelIndex] = position_alpha * (positionSmoothed[channelIndex] - position[channelIndex])
            + position[channelIndex];
        
        /* Check if necessary to update random */
        countR[channelIndex]++;
        if (countR[channelIndex] > MAXCOUNT_R - 1) {
            countR[channelIndex] = 0;
            
            randomPosition[channelIndex] = amplitudeR_smooth[channelIndex] * 2.f * (random.nextDouble() - 1.f);
        }
        
        /* Smooth random */
        randomSmoothed[channelIndex] = random_alpha * (randomSmoothed[channelIndex] - randomPosition[channelIndex])
            + randomPosition[channelIndex];
        
        return std::clamp (positionSmoothed[channelIndex] + randomSmoothed[channelIndex], (double) 1, (double) (size - 3));
    }
    
    inline double sin_lfo (const double t) {
        auto output = std::sin (t);
        return (isInverted) ? -output : output;
    }
    
    inline double tri_lfo (const double t) {
        auto output = (t < pi) ? -1 + t * twoOverPi : 1 - (t - pi) * twoOverPi;
        return (isInverted) ? -output : output;
    }
    
    inline std::array<int, 4> findReadPointers (const int channelIndex, int iDel) {
        auto r1 = writeIndex[channelIndex] - iDel + 1;
        if (r1 < 0)
            r1 = size + r1;
        else if (r1 > size - 1)
            r1 = r1 - size;
        
        auto r2 = writeIndex[channelIndex] - iDel;
        if (r2 < 0)
            r2 = size + r2;
        
        auto r3 = writeIndex[channelIndex] - iDel - 1;
        if (r3 < 0)
            r3 = size + r3;
        
        auto r4 = writeIndex[channelIndex] - iDel - 2;
        if (r4 < 0)
            r4 = size + r4;
        
        std::array<int, 4> readPointers { r1, r2, r3, r4 };
        return readPointers;
    }
    
    inline double interpolate (std::array<int, 4> readPointers, const int channelIndex, const double frac) {
        int r1 = readPointers[0];
        int r2 = readPointers[1];
        int r3 = readPointers[2];
        int r4 = readPointers[3];
        
        double frac2 = frac * frac;
        
        double a0 = delayLine[channelIndex][r4] - delayLine[channelIndex][r3] - delayLine[channelIndex][r1] + delayLine[channelIndex][r2];
        double a1 = delayLine[channelIndex][r1] - delayLine[channelIndex][r2] - a0;
        double a2 = delayLine[channelIndex][r3] - delayLine[channelIndex][r1];
        double a3 = delayLine[channelIndex][r2];
        
        return (a0 * frac * frac2 + a1 * frac2 + a2 * frac + a3);
    }
    
    inline void smoothParameters_sample (const int channelIndex) {
        if (!juce::approximatelyEqual (amplitudeM_smooth[channelIndex], amplitudeM))
            amplitudeM_smooth[channelIndex] = smooth_alpha * (amplitudeM_smooth[channelIndex] - amplitudeM) + amplitudeM;
        
        if (!juce::approximatelyEqual (amplitudeR_smooth[channelIndex], amplitudeR))
            amplitudeR_smooth[channelIndex] = smooth_alpha * (amplitudeR_smooth[channelIndex] - amplitudeR) + amplitudeR;
    }
    
    void updateParameters_forced () {
        amplitudeM_smooth[0] = amplitudeM_smooth[1] = amplitudeM;
        amplitudeR_smooth[0] = amplitudeR_smooth[1] = amplitudeR;
    }
    
    void setSize (const int newSize) {
        size = newSize;
        auto size_minus1 = size - 1;
        writeIndex[0] = writeIndex[1] = size_minus1;
        
        delayLine[0].resize (size);
        delayLine[1].resize (size);
        
        reset();
    }
    
    void setDelayTime (const double newDelayTime_seconds) {
        delayTime_seconds = newDelayTime_seconds;
        delayTime_samples = std::round (delayTime_seconds * sampleRate);
        
        DBG ("Delay Seconds: " << delayTime_seconds << ", Delay Samples: " << delayTime_samples);
        
        amplitudeM = (delayTime_samples * 0.7f) * depthM;
        amplitudeR = (delayTime_samples * 0.25f) * depthR;
    }
    
    void setModRefreshRate (const int newModRefreshRate_samples) {
        MAXCOUNT_M = newModRefreshRate_samples;
        countM[0] = countM[1] = MAXCOUNT_M - 1;
        phi = rate * samplePeriod * twoPi * (float) MAXCOUNT_M;
        
        DBG ("Mod Max: " << MAXCOUNT_M << ", count: " << countM[0]);
    }
    
    void setModRate (const double newModRate_hz) {
        rate = newModRate_hz;
        phi = rate * samplePeriod * twoPi * (float) MAXCOUNT_M;
        
        DBG ("Mod Rate: " << rate << ", Phi: " << phi);
    }
    
    void setModDepth (const double newModDepth) {
        depthM = newModDepth;
        amplitudeM = (delayTime_samples * 0.7f) * depthM;
        
        DBG ("Mod Depth: " << depthM << ", Mod Amp: " << amplitudeM);
    }
    
    void setModShape (const double newModShape) {
        shape = newModShape;
        
        DBG ("Mod Shape: " << shape);
    }
    
    void setModStereo (const bool newModStereo) {
        isStereo = newModStereo;
        
        DBG ("Stereo: " << (float) isStereo);
    }
            
    void setModInverted (const bool newModInverted) {
        isInverted = newModInverted;
        
        DBG ("Inverted: " << (float) isInverted);
    }
    
    void setRandomRefreshRate (const int newRandomRefreshRate_samples) {
        MAXCOUNT_R = newRandomRefreshRate_samples;
        countR[0] = countR[1] = MAXCOUNT_R - 1;
        
        DBG ("Random Max: " << MAXCOUNT_R << ", count: " << countR[0]);
    }
    
    void setRandomDepth (const double newRandomDepth) {
        depthR = newRandomDepth;
        amplitudeR = (delayTime_samples * 0.25f) * depthR;
        
        DBG ("Random Depth: " << depthR << ", Random Amp: " << amplitudeR);
    }
    
    void setRandomSmooth (const double newRandomSmooth) {
        random_alpha = (newRandomSmooth * 0.00029f) + 0.9997;
        
        DBG ("Random smooth: " << random_alpha);
    }
    
private:
    const double pi = juce::MathConstants<double>::pi;
    const double twoOverPi = 2 / pi;
    const double twoPi = juce::MathConstants<double>::twoPi;
    const double halfPi = juce::MathConstants<double>::halfPi;
    
    /* Process Variables */
    float sampleRate = 48000;
    double samplePeriod = 1 / sampleRate;
    
    /* Delay Variables */
    int size = 96000;
    std::array<juce::Array<double>, 2> delayLine;
    
    int writeIndex[2] { 0 };
    
    /* LFO Variables */
    bool isInverted = false, isStereo = false;
    double shape, rate;
    double delayTime_seconds = 1, delayTime_samples = 48000;
    double depthM = 0;
    double amplitudeM, amplitudeM_smooth[2] { 0 };
    double phi;
    double theta[2] { 0 };
    
    double position[2] = { 0 };
    double positionSmoothed[2] = { 0 };
    const float position_alpha = 0.9995f;
    
    /* Random Variables */
    juce::Random random { juce::Time::currentTimeMillis() };
    double randomPosition[2] { 0 };
    double randomSmoothed[2] { 0 };
    double random_alpha = 0.997;
    double depthR = 0;
    double amplitudeR, amplitudeR_smooth[2] { 0 };
    
    /* Refresh Rate */
    int MAXCOUNT_M = 64;
    int countM[2] { MAXCOUNT_M, MAXCOUNT_M };
    int MAXCOUNT_R = 64;
    int countR[2] { MAXCOUNT_R, MAXCOUNT_R };

    /* Smoothing */
    const float smoothTime_seconds = 0.1f; // 100 ms
    double smooth_alpha = std::exp (-1 * std::log (9) / (sampleRate * smoothTime_seconds));
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CubicDelay)
};
