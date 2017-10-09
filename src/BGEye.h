#ifndef BGEYE_H
#define BGEYE_H

#include "ofMain.h"

class BGEye {
	public:
		BGEye(ofVec2f, float);
		virtual ~BGEye();

        void render(ofShader & mEyeShader, ofShader & mContainerShader);
        void update(float dt);
    private:
        ofVec2f mPosition;
        float mOrientation;
        ofMesh mQuad;
        float mAnimParam;
        float mContainerAnimParam;

        ofImage mVeinsImage;
        ofImage mContainerImage;

        ofMesh mMesh;
};

#endif //BGEYE_H
