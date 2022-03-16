#include "shader.h"

REGISTER_SHADER_APP(gradient) {
    static const RgbColor color1 = RgbColor(235, 121, 0); /* Orange */
    static const RgbColor color2 = RgbColor(0, 121, 235); /* Blue */

    float angle = Shader::time / 100.0f;

    float distanceToAngledLine = sin(angle) * Shader::normalizedPos.x - cos(angle) * Shader::normalizedPos.y;
    float progress = (tanh(distanceToAngledLine * 3) + 1) / 2;

    RgbColor blendedColor = RgbColor::LinearBlend(color1, color2, progress);

    return colorGamma.Correct(blendedColor);
}