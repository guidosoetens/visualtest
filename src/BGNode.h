#ifndef BGNODE_H
#define BGNODE_H

#include "ofMain.h"

//NOTE! Make sure: EDGE_WIDTH > 2 * BRIM_WIDTH
#define BRIM_WIDTH 10.0
#define EDGE_WIDTH 50.0
#define SPLINE_SAMPLES 15
#define SINGLE_SPLINE_SAMPLES 15

class BGNode {
	public:
		BGNode();
		virtual ~BGNode();

        virtual void render() = 0;
        virtual void update(float dt) = 0;

        ofVec2f position;
        int traversalId;

        float nodeRadius;

        void traversePushToMesh(ofMesh & mesh, float centerFactor, float offsetFactor);

    protected:

        float nodeDistance;
        std::vector<BGNode*> neighbours;
        
    private:
        float mCenterFactor;
        float mOffsetFactor;

        void traversePushToMesh(ofMesh & mesh, BGNode* parentNode, float centerFactor, float offsetFactor);
        void pushToMesh(ofMesh & mesh);

        void traverseDistributeDistance(float parentDistance, BGNode* parentNode);

        //add nodes to mesh, separated by number of neighbours (0, 1, or 2+)
        void pushSeparateToMesh(ofMesh & mesh, float nodeRadius);
        void pushSingleToMesh(ofMesh & mesh, float nodeRadius, BGNode* neighbor);
        void pushMultipleToMesh(ofMesh & mesh, float nodeRadius, std::vector<BGNode*> & neighbours);
        
        //center:
        int pushCenterToMesh(ofMesh & mesh);

        //spline sampling:
        void pushSplineToMesh(ofMesh & mesh, int splineSamples, ofVec2f a1, ofVec2f c, ofVec2f a2);
        //void pushSplineSampleToMesh(ofMesh & mesh, float t, int centerOffset, ofVec2f a1, ofVec2f c, ofVec2f a2);
        void pushSplineSampleToMesh(ofMesh & mesh, float t, ofVec2f a1, ofVec2f c, ofVec2f a2);
        void stitchPreviousSplineSamples(ofMesh & mesh);
        void stitchPreviousSplines(ofMesh & mesh, int splineCount, int splineSamples, float* distances, float centerDistance);

        //circle (arc)
        void pushFullCircletoMesh(ofMesh & mesh, float subRadius);
        void pushCircleArctoMesh(ofMesh & mesh, int centerOffset, float subRadius, float startAngle, float dAngle);
        void pushCircleArctoMesh(ofMesh & mesh, int centerOffset, float subRadius, float startAngle, float dAngle, bool includeBounds);
        
        //vertex:
        void pushVertex(ofMesh & mesh, float x, float y, float z, float nx, float ny, float nz, float d);
        void updateVertexAttributes(ofMesh & mesh, int idx, ofVec3f pos, ofVec3f normal, float d);
        void updateVertexAttributes(ofMesh & mesh, int idx, float x, float y, float z, float nx, float ny, float nz, float d);
        void sampleVertex(ofMesh & mesh, int idx, ofVec3f & pos, ofVec3f & normal);
};

#endif //BGNODE_H
