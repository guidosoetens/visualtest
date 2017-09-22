#ifndef BGCOG_H
#define BGCOG_H

#include "ofMain.h"

class BGCog {
	public:
		BGCog(ofVec2f, float);
		virtual ~BGCog();

        void render(ofShader & mCogShader);
        void update(float dt);
    private:

        ofImage mBumpImage;

        ofMesh mTeethMesh;
        ofMesh mCenterMesh;
        ofVec2f mPosition;
        float mOrientation;

        float mTimeAnimParam;
        float mCellsAnimParam;
};

#endif //BGCOG_H
