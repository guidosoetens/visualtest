#ifndef BGMENU_H
#define BGMENU_H

#include "ofMain.h"
#include "BGSlider.h"
#include "BGColorPicker.h"
#include "BGImagePicker.h"
#include "BGResources.h"

#define MENU_OUT_MARGIN 20
#define MENU_INNER_MARGIN 5
#define MENU_WIDTH (2 * MENU_INNER_MARGIN + CONTROL_WIDTH)
#define SUB_PANEL_HEIGHT 400
#define CONTROLS_TOP_OFFSET 60
#define MENU_HEADER_HEIGHT 50

#define BTN_PREV_RECT ofVec4f(MENU_OUT_MARGIN + MENU_WIDTH - MENU_INNER_MARGIN - 80, MENU_OUT_MARGIN + MENU_INNER_MARGIN, 25, 25)
#define BTN_NEXT_RECT ofVec4f(MENU_OUT_MARGIN + MENU_WIDTH - MENU_INNER_MARGIN - 25, MENU_OUT_MARGIN + MENU_INNER_MARGIN, 25, 25)
#define FOOZ 1

class BGMenu : public BGSliderValueChangedListener {
	public:
		BGMenu();
		virtual ~BGMenu();

        void render(ofTrueTypeFont & font);
        void update(float dt);

        void mouseDown(ofVec2f p);
        void mouseMove(ofVec2f p);
        void mouseUp(ofVec2f p);
        void mouseScrolled(ofVec2f p, float scrollY);

        virtual void valueChanged(BGSlider * slider);

    private:

        BGColorPicker mColorPicker;
        BGImagePicker mImagePicker;

        ofImage mArrowImage;
        ofImage mCrossImage;
        ofImage mCheckImage;

        float mScrollValue;
        ofFbo mPanelTarget;
        int mControlsHeight;

        //main page:
        bool mIsOpen;
        bool mScrolling;
        ofVec2f mPreviousScrollPosition;
        vector<BGUserControl*> mUserControls[NUM_STYLES];

        void renderButton(ofVec4f bounds, ofImage & image, bool isFlipped);
};

#endif //BGMENU_H
