#pragma once

#ifndef StarShader_hpp
#define StarShader_hpp

#include <stdio.h>
#include <assert.h>
#include "PhongShader.h"


class StarShader : public PhongShader
{
public:
    StarShader(const std::string& AssetDirectory);
    virtual ~StarShader() {}
    virtual void activate(const BaseCamera& Cam) const;
    virtual void deactivate() const;
    void setFrequency(const float& f) { Frequency = f; };
    void setAmplitude(const float& a) { Amplitude = a; };
    void setTime(const float& t) { Time = t; };

private:
    GLint AmplitudeLoc;
    GLint FrequencyLoc;
    GLint TimeLoc;

    float Amplitude;
    float Frequency;
    float Time;
};


#endif /* StarShader_hpp */
