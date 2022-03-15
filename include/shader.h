#include "led.h"

namespace Shader {
    void setShaderFunction(std::function<RgbColor()> function);
    extern float time;
    extern float x;
    extern float y;
    extern vec2<float> pos;
    extern int letterNumber;
}

namespace App_shader {
    void setup();
    void loop();
}

#define REGISTER_SHADER_APP(name) \
    namespace App_##name { \
        RgbColor shader(); \
        void setup() { Shader::setShaderFunction(App_##name::shader); } \
        void loop() { App_shader::loop(); } \
    }; \
    const App &app_##name = []() { App app = std::make_tuple(App_##name::setup, App_##name::loop); registerApp(#name, app); return app; }(); \
    RgbColor App_##name::shader()
