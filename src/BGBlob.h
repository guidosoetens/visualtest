#ifndef BGBLOB_H
#define BGBLOB_H

#include "ofMain.h"

class BGBlob {
	public:
		BGBlob(ofVec2f, float);
		virtual ~BGBlob();

        void render();
        void update(float dt);
    private:
        ofVec2f mPosition;
        float mOrientation;
};

#endif //BGBLOB_H
