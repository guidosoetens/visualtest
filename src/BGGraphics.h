#ifndef BGGRAPHICS_H
#define BGGRAPHICS_H

#include "ofMain.h"

#define NETWORK_OFFSET 15.0

class BGGraphics {
	public:
		BGGraphics();
		virtual ~BGGraphics();
        
        void update(float dt);
        void reload();

        bool renderWireframe;
        bool renderFlow;
        bool depthTest;
        float maxDepth;
        
        void renderSeparateNode(ofVec2f position, float nodeRadius, float depth);
        void renderSingleConnectedNode(ofVec2f position, float nodeRadius, ofVec2f edgePoint, float depth);
        void renderDoubleConnectedNode(ofVec2f position, ofVec2f startEdgePoint, ofVec2f endEdgePoint, float depth);
        void renderTripleConnectedNode(ofVec2f position, ofVec2f startEdgePoint, ofVec2f endEdgePoint1, ofVec2f endEdgePoint2, float depth);

    private:
        ofShader mNetworkShader;
        float mTimeParameter;

        void createMeshTemplates();
        ofVec2f calculateInternalTexOffset(float t, bool isSourceSpline, bool isSourceSegment, int offsetIndex);
        
        float dot(ofVec2f a, ofVec2f b);
        float cross(ofVec2f a, ofVec2f b);
        ofVec2f reflect(ofVec2f v, ofVec2f n);
        bool getIntersection(ofVec2f p, ofVec2f r, ofVec2f q, ofVec2f s, ofVec2f & result);
        ofVec2f getBarycentricCoords(ofVec2f p, ofVec2f a, ofVec2f b, ofVec2f c);

        void sampleSpline(ofVec2f a1, ofVec2f c, ofVec2f a2, float t, ofVec2f & pt, ofVec2f & normal);
        void pushVertex(ofMesh & mesh, float x, float y, float z, float nx, float ny, float nz, float offsetX, float offsetY);
        void drawMesh(ofMesh & mesh);
        void drawMesh(ofMesh & mesh, bool triangulateFlowTexture);

        void renderCirclePart(ofVec2f position, float nodeRadius, float minAngle, float deltaAngle, float depth);
};

#endif //BGGRAPHICS_H
