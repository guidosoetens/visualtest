#ifndef BGTENTACLE_H
#define BGTENTACLE_H

#include "ofMain.h"
#include "BGEntrance.h"

class BGTentacle {
	public:
		BGTentacle(ofVec2f, float);
		virtual ~BGTentacle();

        void render(ofShader & mTentacleShader, ofShader & mEntranceShader);
        void update(float dt);
    private:
        ofVec2f mPosition;
        float mOrientation;
        float mTimeParameter;
        ofMesh mMesh;

        BGEntrance mEntrance;
};

#endif //BGTENTACLE_H
