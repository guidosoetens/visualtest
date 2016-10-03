#ifndef BGENTRANCE_H
#define BGENTRANCE_H

#include "ofMain.h"

class BGEntrance {
	public:
		BGEntrance(ofVec2f, float);
		virtual ~BGEntrance();

        void render(ofShader & mEntranceShader);
        void update(float dt);
        void renderBack(ofShader & mEntranceShader);
    private:

        vector<ofMesh> mMeshes;
        vector<ofMesh> mBackMeshes;
        void renderMeshes(ofShader & mEntranceShader, vector<ofMesh> & meshes);

        ofVec2f mPosition;
        float mOrientation;
        ofVec2f addVertex(ofMesh & mesh, float angle, ofVec2f size, float y);
};

#endif //BGENTRANCE_H
