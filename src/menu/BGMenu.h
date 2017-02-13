#ifndef BGMENU_H
#define BGMENU_H

#include "ofMain.h"
#include "BGSlider.h"
#include "BGColorPicker.h"
#include "BGResources.h"

class BGMenu : public BGSliderValueChangedListener, public BGColorPickerClosedListener {
	public:
		BGMenu();
		virtual ~BGMenu();

        void render(ofTrueTypeFont & font);
        void update(float dt);

        void mouseDown(ofVec2f p);
        void mouseMove(ofVec2f p);
        void mouseUp(ofVec2f p);

        virtual void valueChanged(BGSlider * slider);
        virtual void colorPickerClosed(BGColorPicker * colorPicker);

    private:

        typedef struct {
            ofColor color;
            string name;
            ofVec2f position;
        } ColorPropertyButton;

        BGColorPicker mColorPicker;

        bool mIsOpen;
        BGSlider mStyleIndexSlider;
        vector<BGSlider> mSliders;
        vector<ColorPropertyButton> mColorButtons;
        vector<int> mSliderValues;
};

#endif //BGMENU_H
