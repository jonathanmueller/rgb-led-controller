#include "shader.h"

REGISTER_SHADER_APP(gradient) {
    static HsbColor oppositeColor;
    
    SHADER_ONCE {
        appRespectsPrimaryColor = true;
        oppositeColor = primaryColor.getCurrentValue();
        oppositeColor.H = fract(oppositeColor.H + 0.5);
    }

    // static const RgbColor color1 = RgbColor(235, 121, 0); /* Orange */
    // static const RgbColor color2 = RgbColor(0, 121, 235); /* Blue */

    // static const RgbColor

    // float angle = Shader::time * 3.0f;
    float angle = Shader::time / 10.0f;

    float distanceToAngledLine = sin(angle) * Shader::normalizedPos.x - cos(angle) * Shader::normalizedPos.y;
    float progress = (tanh(distanceToAngledLine * 3) + 1) / 2;

    RgbColor blendedColor = RgbColor::LinearBlend(primaryColor, oppositeColor, progress);

    return blendedColor;
}