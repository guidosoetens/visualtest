#ifndef BGMENU_H
#define BGMENU_H

#include "ofMain.h"
#include "BGSlider.h"

class BGMenu : public BGSliderValueChangedListener {
	public:
		BGMenu();
		virtual ~BGMenu();

        void render(ofTrueTypeFont & font);
        void update(float dt);

        void mouseDown(ofVec2f p);
        void mouseMove(ofVec2f p);
        void mouseUp(ofVec2f p);

        virtual void valueChanged(BGSlider * slider);

    private:

        bool mIsOpen;
        vector<BGSlider> mSliders;
        vector<int> mSliderValues;
};

#endif //BGMENU_H
