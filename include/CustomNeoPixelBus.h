
#pragma once

#include "NeoPixelBus.h"

template<typename T_COLOR_FEATURE, typename T_METHOD, typename T_GAMMA_METHOD = NeoGammaTableMethod>
class CustomNeoPixelBus : public NeoPixelBus<T_COLOR_FEATURE, T_METHOD> {
public:
    CustomNeoPixelBus(uint16_t countPixels, const typename T_COLOR_FEATURE::ColorObject::SettingsObject& settingsObject) :
        NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(countPixels),
        _brightness(1.0f),
        _currentLimit(0),
        _settingsObject(settingsObject)
    {
        _pixelCopy = static_cast<uint8_t*>(malloc(this->PixelsSize()));
    }

    ~CustomNeoPixelBus() {
        free(_pixelCopy);
    }

    void SetBrightness(float brightness) {
        brightness = min(max(brightness, 0.0f), 1.0f);
        if (_brightness != brightness) {
            _brightness = brightness;
            this->Dirty();
        }
    }

    float GetBrightness() const {
        return _brightness;
    }

    void SetCurrentLimit(uint16_t currentLimit) {
        if (_currentLimit != currentLimit) {
            _currentLimit = currentLimit;
            this->Dirty();
        }
    }

    uint16_t GetCurrentLimit() const {
        return _currentLimit;
    }


    void Show() {
        if (!this->IsDirty()) {
            return;
        }

        uint8_t* pixelData = this->Pixels();

        /* Save current pixel data */
        memcpy(_pixelCopy, pixelData, this->PixelsSize());

        /* Adapt pixel data according to settings */
        AdaptPixelData();

        /* Call super implementation to send out data */
        NeoPixelBus<T_COLOR_FEATURE, T_METHOD>::Show();

        /* Restore pixel data */
        memcpy(pixelData, _pixelCopy, this->PixelsSize());
    }

    uint16_t CalcTotalMilliAmpere(const typename T_COLOR_FEATURE::ColorObject::SettingsObject& _ignored) {
        return currentDraw;
    }


protected:
    uint8_t* _pixelCopy;
    uint16_t currentDraw = 0;

    float _brightness;
    uint16_t _currentLimit;
    typename T_COLOR_FEATURE::ColorObject::SettingsObject _settingsObject;

    NeoGamma<T_GAMMA_METHOD> gamma;

    void AdaptPixelData() {
        uint8_t* pixelData = this->Pixels();

        /* Adapt pixel brightness and apply gamma */
        for (uint16_t i = 0; i < this->PixelsSize(); i++) {
            pixelData[i] = T_GAMMA_METHOD::Correct(pixelData[i] * _brightness);
        }

        /* Call super method to calculate current usage */
        uint16_t unlimitedCurrentDraw = NeoPixelBus<T_COLOR_FEATURE, T_METHOD>::CalcTotalMilliAmpere(_settingsObject);

        /* Limit maximum current */
        if (_currentLimit > 0) {
            if (unlimitedCurrentDraw > _currentLimit) {
                float currentLimitFactor = (float)_currentLimit / unlimitedCurrentDraw;
                uint8_t* pixelData = this->Pixels();
                for (uint16_t i = 0; i < this->PixelsSize(); i++) {
                    pixelData[i] *= currentLimitFactor;
                }
            }
            currentDraw = NeoPixelBus<T_COLOR_FEATURE, T_METHOD>::CalcTotalMilliAmpere(_settingsObject);
        } else {
            currentDraw = unlimitedCurrentDraw;
        }

    }
};


