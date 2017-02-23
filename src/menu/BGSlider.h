#ifndef BGSLIDER_H
#define BGSLIDER_H

#include "ofMain.h"
#include "BGResources.h"
#include "BGUserControl.h"

#define SLIDER_MARGIN_LEFT 100
#define SLIDER_MARGIN_RIGHT 80
#define BUTTON_WIDTH 20

class BGSlider;

class BGSliderValueChangedListener {
    public:
        virtual void valueChanged(BGSlider * slider) = 0;
};

class BGSlider : public BGUserControl {
	public:
		BGSlider(ofVec2f position, BGIntegerSetting* setting, int minValue, int maxValue, BGSliderValueChangedListener* callbackListener)
        :   mSliding(false)
        ,   mIntegerSetting(setting)
        ,   mFloatSetting(NULL)
        ,   mMinValue(minValue)
        ,   mMaxValue(maxValue)
        ,   mCallbackListener(callbackListener) 
        {
             mPosition = position;
        };

        BGSlider(ofVec2f position, BGFloatSetting* setting, float minValue, float maxValue, float step, BGSliderValueChangedListener* callbackListener)
        :   mSliding(false)
        ,   mIntegerSetting(NULL)
        ,   mFloatSetting(setting)
        ,   mMinValuef(minValue)
        ,   mMaxValuef(maxValue)
        ,   mStepf(step)
        ,   mCallbackListener(callbackListener) 
        {
             mPosition = position;
        };
        
		virtual ~BGSlider();

        virtual void render(ofTrueTypeFont & font);
        virtual void update(float dt);

        virtual void mouseDown(ofVec2f p);
        virtual void mouseMove(ofVec2f p);
        virtual void mouseUp(ofVec2f p);

    private:

        BGFloatSetting* mFloatSetting;
        float mMinValuef;
        float mMaxValuef;
        float mStepf;

        BGIntegerSetting* mIntegerSetting;
        int mMinValue;
        int mMaxValue;

        bool mSliding;
        BGSliderValueChangedListener* mCallbackListener;

};

#endif //BGCOLORPICKER_H
