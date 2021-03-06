#include "led.h"
#include "shader.h"
#include <Arduino.h>

namespace Shader {
    std::function<RgbColor()> shaderFunction;

    unsigned long shaderStartTime = 0;

    /* Shader variables */
    bool isSetup;
    float time;
    float deltaTime;
    vec2<float> pos;
    vec2<float> normalizedPos;
    int letterNumber;
    unsigned int pixelIndex;
    unsigned long lastExecutionTime = 0;

    void setShaderFunction(std::function<RgbColor()> function) {
        Shader::shaderFunction = function;
        Shader::shaderStartTime = millis();
        Shader::lastExecutionTime = Shader::shaderStartTime;
    }

    void loop() {
        unsigned long time = millis();

        Shader::time = (time - Shader::shaderStartTime) / 1000.0f;
        Shader::deltaTime = max(1UL, time - Shader::lastExecutionTime) / 1000.0f;

        Shader::isSetup = true;
        Shader::shaderFunction();
        Shader::isSetup = false;

        for (unsigned int i = 0; i < PixelCount; i++) {
            Shader::pixelIndex = i;
            Shader::pos = PixelPositions[i];
            Shader::normalizedPos = NormalizedPixelPositions[i];
            Shader::letterNumber = PixelLetterNumbers[i];
            strip.SetPixelColor(i, Shader::shaderFunction());
        }
        strip.Show();

        Shader::lastExecutionTime = time;
    }
}
