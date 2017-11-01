#ifndef BGENTRANCEMESH_H
#define BGENTRANCEMESH_H

#include "ofMain.h"

class BGEntranceMesh {
	public:
		BGEntranceMesh(ofVec2f, float);
		virtual ~BGEntranceMesh();

        void render(ofShader & mEntranceMeshShader);
        void update(float dt);

    private:
        ofVec2f mPosition;
        float mOrientation;
        ofMesh mMesh;

        int sign(float f);
        void sampleSpline(ofVec2f p0, ofVec2f p1, ofVec2f p2, ofVec2f p3, float t, ofVec2f & p, ofVec2f & n);
        void pushTentacle(ofMesh& mesh, ofVec2f p0, ofVec2f p1, ofVec2f p2, ofVec2f p3);
        void mergeVertexInto(ofMesh& mesh, int src, int dst);
};

#endif //BGENTRANCE_H
