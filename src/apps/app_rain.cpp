#include "led.h"

REGISTER_APP(rain) {
    // NeoPixelFunLoop
    // This example will move a trail of light around a series of pixels.  
    // A ring formation of pixels looks best.  
    // The trail will have a slowly fading tail.
    // 
    // This will demonstrate the use of the NeoPixelAnimator.
    // It shows the advanced use an animation to control the modification and 
    // starting of other animations.
    // It also shows the normal use of animating colors.
    // It also demonstrates the ability to share an animation channel rather than
    // hard code them to pixels.
    //

    const uint16_t AnimCount = PixelCount / 5 * 2 + 1; // we only need enough animations for the tail and one extra

    const uint16_t PixelFadeDuration = 300; // third of a second
    const uint16_t NextPixelMoveDuration = 1000 / PixelCount; // how fast we move through the pixels

    // what is stored for state is specific to the need, in this case, the colors and
    // the pixel to ainmate;
    // basically what ever you need inside the animation update function
    struct MyAnimationState
    {
        RgbColor StartingColor;
        RgbColor EndingColor;
        uint16_t IndexPixel; // which pixel this animation is effecting
    };

    NeoPixelAnimator animations(AnimCount); // NeoPixel animation management object
    MyAnimationState animationState[AnimCount];
    uint16_t frontPixel = 0;  // the front of the loop
    RgbColor frontColor;  // the color at the front of the loop


    void FadeOutAnimUpdate(const AnimationParam & param)
    {
        // this gets called for each animation on every time step
        // progress will start at 0.0 and end at 1.0
        // we use the blend function on the RgbColor to mix
        // color based on the progress given to us in the animation
        RgbColor updatedColor = RgbColor::LinearBlend(
            animationState[param.index].StartingColor,
            animationState[param.index].EndingColor,
            param.progress);
        // apply the color to the strip
        strip.SetPixelColor(animationState[param.index].IndexPixel,
            colorGamma.Correct(updatedColor));
    }

    void LoopAnimUpdate(const AnimationParam & param)
    {
        // wait for this animation to complete,
        // we are using it as a timer of sorts
        if (param.state == AnimationState_Completed)
        {
            // done, time to restart this position tracking animation/timer
            animations.RestartAnimation(param.index);

            // pick the next pixel inline to start animating
            // 
            frontPixel = (frontPixel + 1) % PixelCount; // increment and wrap
            if (frontPixel == 0)
            {
                // we looped, lets pick a new front color
                frontColor = HslColor(random(360) / 360.0f, 1.0f, 0.25f);
                markAppCycle();
            }

            uint16_t indexAnim;
            // do we have an animation available to use to animate the next front pixel?
            // if you see skipping, then either you are going to fast or need to increase
            // the number of animation channels
            if (animations.NextAvailableAnimation(&indexAnim, 1))
            {
                animationState[indexAnim].StartingColor = frontColor;
                animationState[indexAnim].EndingColor = RgbColor(0, 0, 0);
                animationState[indexAnim].IndexPixel = frontPixel;

                animations.StartAnimation(indexAnim, PixelFadeDuration, FadeOutAnimUpdate);
            }
        }
    }

    void setup() {
        fill(0);

        // we use the index 0 animation to time how often we move to the next
        // pixel in the strip
        animations.StartAnimation(0, NextPixelMoveDuration, LoopAnimUpdate);
        strip.Show();
    }

    void loop() {
        animations.UpdateAnimations();
        strip.Show();
    }

}