#ifndef BGIMAGEITEM_H
#define BGIMAGEITEM_H

#include "ofMain.h"
#include "BGUserControl.h"
#include "BGImagePicker.h"

#define IMAGE_ITEM_HEIGHT 50

class BGImageItem : public BGUserControl {
    public:
        BGImageItem(ofVec2f position, BGImageSetting* imageSetting, BGImagePicker* imagePicker) 
        :   mImageSetting(imageSetting)
        ,   mImagePicker(imagePicker)
        {
            mPosition = position;
        }
	    virtual ~BGImageItem();

        virtual void render(ofTrueTypeFont & font);
        virtual void update(float dt);

        virtual void mouseDown(ofVec2f p);
        virtual void mouseMove(ofVec2f p);
        virtual void mouseUp(ofVec2f p);
    private:
        BGImageSetting* mImageSetting;
        BGImagePicker* mImagePicker;

};

#endif //BGIMAGEITEM_H
