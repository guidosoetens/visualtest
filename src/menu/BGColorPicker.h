#ifndef BGCOLORPICKER_H
#define BGCOLORPICKER_H

#include "ofMain.h"
#include "BGSlider.h"

class BGColorPicker {
	public:
		BGColorPicker();
		virtual ~BGColorPicker();

        void render();
        void update(float dt);

        void mouseDown(ofVec2f p);
        void mouseMove(ofVec2f p);
        void mouseUp(ofVec2f p);
    private:

        vector<BGSlider> mSliders;
};

#endif //BGCOLORPICKER_H
