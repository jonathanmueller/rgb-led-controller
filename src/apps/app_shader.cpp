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
    float x;
    float y;
    vec2<float> pos;
    int letterNumber;
}

REGISTER_APP(shader) {
    void setup() {}

    void loop() {
        Shader::time = (millis() - Shader::shaderStartTime) / 1000.0f;
        for (int i = 0; i < PixelCount; i++) {
            Shader::pos = pixelPositions[i];
            Shader::letterNumber = letterNumbers[i];
            Shader::x = Shader::pos.x;
            Shader::y = Shader::pos.y;
            strip.SetPixelColor(i, Shader::shaderFunction());
        }
        strip.Show();
    }
}