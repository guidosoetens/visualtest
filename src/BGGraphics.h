#ifndef BGGRAPHICS_H
#define BGGRAPHICS_H

#include "ofMain.h"

#define NETWORK_OFFSET 10.0

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
        float boundOffset;
        int drawMode;
        
        void pushSeparateNode(ofMesh& mesh, ofVec2f position, float nodeRadius);
        void pushSingleConnectedNode(ofMesh& mesh, ofVec2f position, float nodeRadius, ofVec2f edgePoint, bool isRoot);
        void pushDoubleConnectedNode(ofMesh& mesh, ofVec2f position, ofVec2f startEdgePoint, ofVec2f endEdgePoint);
        void pushTripleConnectedNode(ofMesh& mesh, ofVec2f position, ofVec2f startEdgePoint, ofVec2f endEdgePoint1, ofVec2f endEdgePoint2);

        void drawMesh(ofMesh & mesh, ofVec2f nodeLocation, float nodeRadius, float nodeDepth, bool isExternal, bool deform, ofVec2f surfaceNormal);

    private:
        ofShader mNetworkShader;
        float mTime;
        float mRevealParameter;

        void createMeshTemplates();
        ofVec2f calculateInternalTexOffset(float t, bool isSourceSpline, bool isSourceSegment, int offsetIndex);
        
        float dot(ofVec2f a, ofVec2f b);
        float cross(ofVec2f a, ofVec2f b);
        ofVec2f reflect(ofVec2f v, ofVec2f n);
        bool getIntersection(ofVec2f p, ofVec2f r, ofVec2f q, ofVec2f s, ofVec2f & result);
        ofVec2f getBarycentricCoords(ofVec2f p, ofVec2f a, ofVec2f b, ofVec2f c);

        void sampleSpline(ofVec2f a1, ofVec2f c, ofVec2f a2, float t, ofVec2f & pt, ofVec2f & normal);
        void pushVertex(ofMesh & mesh, float x, float y, float z, float nx, float ny, float nz, float offsetX, float offsetY);

        void pushCirclePart(ofMesh& mesh, ofVec2f position, float nodeRadius, float minAngle, float deltaAngle);
};

#endif //BGGRAPHICS_H
