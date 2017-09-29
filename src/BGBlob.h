#ifndef BGBLOB_H
#define BGBLOB_H

#include "ofMain.h"

class BGBlob {
	public:
		BGBlob(ofVec2f, float, float length = 150.0);
		virtual ~BGBlob();

        void render(ofShader & mBlobShader);
        void update(float dt);
    private:
        ofVec2f mPosition;
        float mOrientation;

        ofMesh mMesh;
        float mAnimParam;
        float mBaseLength;

        void sampleSpline(ofVec2f p0, ofVec2f p1, ofVec2f p2, ofVec2f p3, float t, ofVec2f & p, ofVec2f & n);
};

#endif //BGBLOB_H
