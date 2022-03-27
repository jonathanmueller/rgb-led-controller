#include "shader.h"

REGISTER_SHADER_APP(colorwave) {
    double intTime;
    float time = modf(Shader::time / 100.0f, &intTime);

    float brightness = 0.5 - (sin(Shader::time + Shader::normalizedPos.x * 2) + 1) / 8;

    return colorGamma.Correct(RgbColor(HslColor(time, 1.0f, brightness)));
}