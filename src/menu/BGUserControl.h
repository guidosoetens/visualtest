#ifndef BGUSERCONTROL_H
#define BGUSERCONTROL_H

#include "ofMain.h"
#include "BGResources.h"

#define CONTROL_HEIGHT 20
#define CONTROL_WIDTH 250

void drawCenteredText(ofTrueTypeFont & font, string text);
void drawTextLeft(ofTrueTypeFont & font, string text);

class BGUserControl {
	public:
		BGUserControl();
		virtual ~BGUserControl();

        virtual void render(ofTrueTypeFont & font);
        virtual void update(float dt);

        virtual void mouseDown(ofVec2f p);
        virtual void mouseMove(ofVec2f p);
        virtual void mouseUp(ofVec2f p);

        ofVec2f mPosition;
    protected:
        

};

#endif //BGUSERCONTROL_H
