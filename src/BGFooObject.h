#ifndef FOOOBJECT_H
#define FOOOBJECT_H

#include "ofMain.h"

class BGFooObject {
	public:
		BGFooObject();
		virtual ~BGFooObject();

        void render();
        void update(float dt);
    private:
        ofShader mShader;
        ofImage mImage;
        ofImage mPerlinImage;
        float mTimeParam;
};

#endif //FOOOBJECT_H
