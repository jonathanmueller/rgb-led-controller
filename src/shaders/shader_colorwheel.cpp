#include "shader.h"

REGISTER_SHADER_APP(colorwheel) {
    double intTime;
    float time5 = modf(Shader::time / 100.0f, &intTime);

    float brightness = 0.5 - (sin(3 * Shader::time + Shader::x) + 1) / 8;

    return colorGamma.Correct(RgbColor(HslColor(time5, 1.0f, brightness)));
}