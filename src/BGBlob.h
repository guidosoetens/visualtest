#ifndef BGBLOB_H
#define BGBLOB_H

#include "ofMain.h"

class BGBlob {
	public:
		BGBlob(ofVec2f, float, float length = 250.0);
		virtual ~BGBlob();

        void render(ofShader & mBlobShader, ofShader & mEntranceShader);
        void update(float dt);
    private:

        void doOther(float dt);
        ofVec2f mPosition;
        float mOrientation;

        ofMesh mMesh;
        ofMesh mBorderMesh;
        float mAnimParam;
        float mBaseLength;
        float mCurrentLength;
        float mCurrentStretch;

        ofImage mFrontImage;
        ofImage mBackImage;

        void sampleSpline(ofVec2f p0, ofVec2f p1, ofVec2f p2, ofVec2f p3, float t, ofVec2f & p, ofVec2f & n);
        int sign(float f) {
            if(f < 0.0)
                return -1;
            else if(f > 0.0)
                return 1;
            return 0;
        }
};

#endif //BGBLOB_H
