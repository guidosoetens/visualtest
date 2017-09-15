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
        ofMesh mMesh;
        ofVec2f mPosition;
        float mOrientation;
};

#endif //BGCOG_H
