#ifndef BGNODE_H
#define BGNODE_H

#include "ofMain.h"

#define HALF_EDGE_WIDTH 15.0
#define SPLINE_SAMPLES 15

class BGNode {
	public:
		BGNode();
		virtual ~BGNode();

        virtual void render() = 0;
        virtual void update(float dt) = 0;

        ofVec2f position;
        int traversalId;

        float nodeRadius;

        void traversePushToMesh(ofMesh & mesh);

    protected:

        float nodeDistance;
        std::vector<BGNode*> neighbours;
        
    private:

        void traversePushToMesh(ofMesh & mesh, BGNode* parentNode);
        void pushToMesh(ofMesh & mesh);

        void traverseDistributeDistance(float parentDistance, BGNode* parentNode);

        //add nodes to mesh, separated by number of neighbours (0, 1, or 2+)
        void pushSeparateToMesh(ofMesh & mesh, float nodeRadius);
        void pushSingleToMesh(ofMesh & mesh, float nodeRadius, BGNode* neighbor);
        void pushMultipleToMesh(ofMesh & mesh, float nodeRadius, std::vector<BGNode*> & neighbours);
        
        //center:
        int pushCenterToMesh(ofMesh & mesh);

        //spline sampling:
        void pushSplineToMesh(ofMesh & mesh, int oppositeDeltaIdx1, int oppositeDeltaIdx2, int splineSamples, ofVec2f a1, ofVec2f c, ofVec2f a2, float d1, float dCenter, float d2);
        void pushSplineSampleToMesh(ofMesh & mesh, float t, int centerOffset, ofVec2f a1, ofVec2f c, ofVec2f a2, float d);
        void pushSplineSampleToMesh(ofMesh & mesh, float t, ofVec2f a1, ofVec2f c, ofVec2f a2, float d);

        //circle (arc)
        void pushFullCircletoMesh(ofMesh & mesh, float subRadius);
        void pushCircleArctoMesh(ofMesh & mesh, int centerOffset, float subRadius, float startAngle, float dAngle);
        
        //vertex:
        void pushVertex(ofMesh & mesh, float x, float y, float z, float nx, float ny, float nz, float d);
};

#endif //BGNODE_H
