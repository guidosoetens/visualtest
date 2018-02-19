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

        float globalScale;
        bool darkenBrim;
        void overrideColor(ofFloatColor color) {
            mHasOverrideColor = true;
            mColor = color;
        };

    private:
        float mFlowParam;
        float mWobbleParam;
        ofImage mFrontImage;
        ofImage mBackImage;
        ofImage m3DImage;
        ofImage mMaskImage;
        ofImage mSpotsImage;
        ofVec2f mPosition;

        ofImage mNormalImage;
        ofImage mOutlineImage;

        float mOrientation;
        bool mHasMask;

        bool mHasOverrideColor;
        ofFloatColor mColor;

        void prepareDraw(ofShader & mEntranceShader);
        void endDraw(ofShader & mEntranceShader);
};

#endif //BGENTRANCE_H
