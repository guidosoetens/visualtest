#ifndef BGTEXTITEM_H
#define BGTEXTITEM_H

#include "ofMain.h"
#include "BGUserControl.h"
#include "BGColorPicker.h"

class BGTextItem : public BGUserControl {
    public:
        BGTextItem(ofVec2f position, string text) 
        :   mText(text)
        {
            mPosition = position;
        }
	    virtual ~BGTextItem();

        virtual void render(ofTrueTypeFont & font);
        virtual void update(float dt);

        virtual void mouseDown(ofVec2f p);
        virtual void mouseMove(ofVec2f p);
        virtual void mouseUp(ofVec2f p);
    private:
    
        string mText;

};

#endif //BGTEXTITEM_H
