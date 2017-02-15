#ifndef BGIMAGEPICKER_H
#define BGIMAGEPICKER_H

#include "ofMain.h"

class BGImagePicker;

class BGImagePicker {
	public:
		BGImagePicker();
		virtual ~BGImagePicker();

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
};

#endif //BGCOLORPICKER_H
