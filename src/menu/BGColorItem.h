#ifndef BGCOLORITEM_H
#define BGCOLORITEM_H

#include "ofMain.h"
#include "BGUserControl.h"
#include "BGColorPicker.h"

class BGColorItem : public BGUserControl {
    public:
        BGColorItem(ofVec2f position, BGColorSetting* colorSetting, BGColorPicker* colorPicker) 
        :   mColorSetting(colorSetting)
        ,   mColorPicker(colorPicker)
        {
            mPosition = position;
        }
	    virtual ~BGColorItem();

        virtual void render(ofTrueTypeFont & font);
        virtual void update(float dt);

        virtual void mouseDown(ofVec2f p);
        virtual void mouseMove(ofVec2f p);
        virtual void mouseUp(ofVec2f p);
    private:
        BGColorSetting* mColorSetting;
        BGColorPicker* mColorPicker;

};

#endif //BGCOLORITEM_H
