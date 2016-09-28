#ifndef BGBACKGROUND_H
#define BGBACKGROUND_H

#include "ofMain.h"

class BGBackground {
	public:
		BGBackground();
		virtual ~BGBackground();

        void render(ofShader & mBackgroundShader, float width, float height);
        void update(float dt);
    private:

        ofImage mCellImage;
        float mTimeParameter;

};

#endif //BGBACKGROUND_H
