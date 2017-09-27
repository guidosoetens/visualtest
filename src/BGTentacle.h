#ifndef BGTENTACLE_H
#define BGTENTACLE_H

#include "ofMain.h"

class BGTentacle {
	public:
		BGTentacle(ofVec2f, float);
		virtual ~BGTentacle();

        void render();
        void update(float dt);
    private:
        ofVec2f mPosition;
        float mOrientation;
        float mTimeParameter;
        ofMesh mMesh;
        ofShader mTentacleShader;
};

#endif //BGTENTACLE_H