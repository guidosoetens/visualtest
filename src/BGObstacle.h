#ifndef BGOBSTACLE_H
#define BGOBSTACLE_H

#include "ofMain.h"

class BGObstacle {
	public:
		BGObstacle(ofVec2f pos, float rad, int reps, bool stretch);
		virtual ~BGObstacle();

        void render(ofShader & mObstacleShader, ofImage & mBumpMap, int width, int height);
        void update(float dt);
    private:
    
        float mObstacleTimeParameter;
        ofMesh mMesh;
        ofVec2f mPosition;
};

#endif //BGOBSTACLE_H
