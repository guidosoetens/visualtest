#ifndef BGBACKGROUND_H
#define BGBACKGROUND_H

#include "ofMain.h"

class BGBackground {
	public:
		BGBackground();
		virtual ~BGBackground();

        void render(ofShader & mBackgroundShader, ofImage & mCellImage, ofImage & mBubbleImage, ofImage & mMembraneImage, float width, float height);
        void update(float dt);
    private:

        float mTimeParameter;

};

#endif //BGBACKGROUND_H
