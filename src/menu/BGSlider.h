#ifndef BGSLIDER_H
#define BGSLIDER_H

#include "ofMain.h"

#define SLIDER_HEIGHT 20
#define SLIDER_WIDTH 150
#define BUTTON_WIDTH (SLIDER_HEIGHT)

class BGSlider;

class BGSliderValueChangedListener {
    public:
        virtual void valueChanged(BGSlider * slider) = 0;
};

class BGSlider {
	public:
		BGSlider();
		virtual ~BGSlider();

        void setup(string title, ofVec2f position, int* varRef, int minValue, int maxValue, BGSliderValueChangedListener* callbackListener ) {
            mTitle = title;
            mPosition = position;
            mVariable = varRef;
            mMinValue = minValue;
            mMaxValue = maxValue;
            mCallbackListener = callbackListener;
        };

        void render(ofTrueTypeFont & font);
        void update(float dt);

        void mouseDown(ofVec2f p);
        void mouseMove(ofVec2f p);
        void mouseUp(ofVec2f p);

        int getValue() {
            if(mVariable == NULL)
                return 0;
            return *mVariable;
        }

    private:
        string mTitle;
        ofVec2f mPosition;
        int* mVariable;
        int mMinValue;
        int mMaxValue;

        bool mSliding;

        BGSliderValueChangedListener* mCallbackListener;

};

#endif //BGCOLORPICKER_H
