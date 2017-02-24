#ifndef BGIMAGEPICKER_H
#define BGIMAGEPICKER_H

#include "ofMain.h"
#include "BGResources.h"

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
        
        void open(BGImageSetting* setting);
        bool isOpen();
        void close();

        float getTotalHeight();

        float scrollOffset;
         
    private:

        float mTotalHeight;
        bool mIsOpen;
        BGImageSetting* mSetting;
};

#endif //BGCOLORPICKER_H
