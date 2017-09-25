#ifndef BGENTRANCE_H
#define BGENTRANCE_H

#include "ofMain.h"

class BGEntrance {
	public:
		BGEntrance(ofVec2f, float, bool);
		virtual ~BGEntrance();

        void render(ofShader & mEntranceShader);
        void update(float dt);
        void renderBack(ofShader & mEntranceShader);
    private:
        float mWobbleParam;
        ofImage mFrontImage;
        ofImage mBackImage;
        ofImage mMaskImage;
        ofVec2f mPosition;
        float mOrientation;
        bool mHasMask;

        void prepareDraw(ofShader & mEntranceShader);
        void endDraw(ofShader & mEntranceShader);
};

#endif //BGENTRANCE_H
