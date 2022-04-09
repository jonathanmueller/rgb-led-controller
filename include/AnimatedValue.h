#pragma once


template<typename T>
struct AnimatedValue {
    T targetValue;
    bool isAnimating = false;
    T animationStartValue;
    unsigned int animationStartTime = 0;
    unsigned int animationDuration = 0;

    AnimatedValue() : targetValue(), animationStartValue() {}
    AnimatedValue(const T& value) : targetValue(value), animationStartValue(value) {}

    operator T() { return getCurrentValue(); }

    void animateTo(const T &target, unsigned int duration) {
        if (duration > 0) {
            if (target != targetValue) {
                animationStartValue = getCurrentValue();
                if (!isAnimating) {
                    animationStartTime = millis();
                } else {
                    animationStartTime = 2 * millis() - duration - animationStartTime;
                }
                isAnimating = true;
                animationDuration = duration;
            }
        } else {
            isAnimating = false;
            animationStartValue = target;
            animationDuration = 0;
        }
        targetValue = target;
    }

    T getCurrentValue() {
        if (isAnimating) {
            unsigned long timeSinceAnimationStart = millis() - animationStartTime;
            if (timeSinceAnimationStart >= animationDuration) {
                isAnimating = false;
            }

            float factor = min(max((float)timeSinceAnimationStart / animationDuration, 0.0f), 1.0f);
            return targetValue * factor + animationStartValue * (1-factor);
        }

        return targetValue;
    }
};