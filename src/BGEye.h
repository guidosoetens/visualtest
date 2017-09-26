#ifndef BGEYE_H
#define BGEYE_H

#include "ofMain.h"

class BGEye {
	public:
		BGEye(ofVec2f, float);
		virtual ~BGEye();

        void render();
        void update(float dt);
    private:
        ofVec2f mPosition;
        float mOrientation;
};

#endif //BGEYE_H
