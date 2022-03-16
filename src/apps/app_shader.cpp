#include "led.h"
#include "shader.h"
#include <Arduino.h>

namespace Shader {
    std::function<RgbColor()> shaderFunction;
    unsigned long shaderStartTime = 0;
    void setShaderFunction(std::function<RgbColor()> function) {
        Shader::shaderFunction = function;
        Shader::shaderStartTime = millis();
    }

    /* Shader variables */
    float time;
    vec2<float> pos;
    vec2<float> normalizedPos;
    int letterNumber;

    void loop() {
        Shader::time = (millis() - Shader::shaderStartTime) / 1000.0f;
        for (int i = 0; i < PixelCount; i++) {
            Shader::pos = PixelPositions[i];
            Shader::normalizedPos = NormalizedPixelPositions[i];
            Shader::letterNumber = PixelLetterNumbers[i];
            strip.SetPixelColor(i, Shader::shaderFunction());
        }
        strip.Show();
    }
}
