#ifndef BGCOLORPICKER_H
#define BGCOLORPICKER_H

#include "ofMain.h"
#include "BGSlider.h"

class BGColorPicker;

class BGColorPickerClosedListener {
    public:
        virtual void colorPickerClosed(BGColorPicker * colorPicker) = 0;
};

class BGColorPicker : public BGSliderValueChangedListener {
	public:
		BGColorPicker();
		virtual ~BGColorPicker();

        void render(ofTrueTypeFont & font);
        void update(float dt);

        void mouseDown(ofVec2f p);
        void mouseMove(ofVec2f p);
        void mouseUp(ofVec2f p);

         virtual void valueChanged(BGSlider * slider);

         void open(ofColor* color);
         ofColor getColor();
         bool isOpen();


    private:

        ofColor* mColor;
        
        int mRed, mGreen, mBlue, mHue, mSaturation, mValue;
        bool mIsOpen;
        vector<BGSlider> mSliders;
};

#endif //BGCOLORPICKER_H
