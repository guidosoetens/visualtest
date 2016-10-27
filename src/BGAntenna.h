#ifndef BGANTENNA_H
#define BGANTENNA_H

#include "ofMain.h"

class BGAntenna {
	public:
    	BGAntenna(ofVec2f pos, float angle);
		virtual ~BGAntenna();

        void render();
        void update(float dt);

    private:
        ofVec2f mPosition;
        float mAngle;
        float mTimeParameter;
        int mRandomRoot;
};

#endif