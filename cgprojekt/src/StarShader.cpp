#include "StarShader.h"
#include <string>

StarShader::StarShader(const std::string& AssetDirectory) : PhongShader(false), Amplitude(1.0f), Frequency(10.0)
{
    std::string VSFile = AssetDirectory + "vsstar.glsl";
    std::string FSFile = AssetDirectory + "fsphong.glsl";
    if (!load(VSFile.c_str(), FSFile.c_str()))
        throw std::exception();
    PhongShader::assignLocations();
    specularColor(Color(0, 0, 0));

    FrequencyLoc = getParameterID("Frequency");
    AmplitudeLoc = getParameterID("Amplitude");
    TimeLoc = getParameterID("Time");
}

void StarShader::activate(const BaseCamera& Cam) const
{
    PhongShader::activate(Cam);

    setParameter(FrequencyLoc, Frequency);
    setParameter(AmplitudeLoc, Amplitude);
    setParameter(TimeLoc, Time);
}

void StarShader::deactivate() const
{
    PhongShader::deactivate();
}
