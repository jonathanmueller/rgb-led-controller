#include "led.h"
#include "util.h"

namespace Shader {
    void setShaderFunction(std::function<RgbColor()> function);

    extern bool isSetup;
    extern float time;
    extern float deltaTime;
    extern vec2<float> pos;
    extern vec2<float> normalizedPos;

    extern int letterNumber;
    extern void loop();
}

#define SHADER_ONCE if (Shader::isSetup)

#define REGISTER_SHADER_APP(name) \
    namespace App_##name { \
        RgbColor shader(); \
        void setup() { Shader::setShaderFunction(App_##name::shader); } \
    }; \
    const App &app_##name = []() { App app = std::make_tuple(App_##name::setup, Shader::loop); registerApp(#name, app); return app; }(); \
    RgbColor App_##name::shader()
