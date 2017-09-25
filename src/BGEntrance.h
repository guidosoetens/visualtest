#ifndef BGENTRANCE_H
#define BGENTRANCE_H

#include "ofMain.h"

class BGEntrance {
	public:
		BGEntrance(ofVec2f, float);
		virtual ~BGEntrance();

        void render(ofShader & mEntranceShader);
        void update(float dt);
        void renderBack(ofShader & mEntranceShader);
    private:
        float mWobbleParam;
        ofImage mEntanceImage;
        ofVec2f mPosition;
        float mOrientation;
};

#endif //BGENTRANCE_H
