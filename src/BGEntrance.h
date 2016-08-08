#ifndef BGENTRANCE_H
#define BGENTRANCE_H

#include "ofMain.h"

class BGEntrance {
	public:
		BGEntrance(ofVec2f pos, float orientation) : mPosition(pos), mOrientation(orientation) {}
		virtual ~BGEntrance();

        void render();
        void update(float dt);
    private:
        ofVec2f mPosition;
        float mOrientation;
};

#endif //BGENTRANCE_H
