#include "led.h"
#include "eeprom_config.h"

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

#define REGISTER_SHADER_APP(app_name) \
    namespace App_##app_name { \
        RgbColor shader(); \
        void setup() { Shader::setShaderFunction(App_##app_name::shader); } \
    }; \
    const App &app_##app_name = []() { App app = { .name = #app_name, .setup = App_##app_name::setup, .loop = Shader::loop }; registerApp(app); return app; }(); \
    RgbColor App_##app_name::shader()
