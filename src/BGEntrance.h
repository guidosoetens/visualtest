#ifndef BGENTRANCE_H
#define BGENTRANCE_H

#include "ofMain.h"

class BGEntrance {
	public:
		BGEntrance(ofVec2f, float);
		virtual ~BGEntrance();

        void render();
        void update(float dt);
    private:

        vector<ofMesh> mMeshes;

        ofVec2f mPosition;
        float mOrientation;
        ofVec2f addVertex(ofMesh & mesh, float angle, ofVec2f size, float y);
};

#endif //BGENTRANCE_H
