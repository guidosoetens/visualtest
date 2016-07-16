#ifndef BGGRAPHICS_H
#define BGGRAPHICS_H

#include "ofMain.h"
#include "BGNode.h"

#define NETWORK_OFFSET 15.0

class BGGraphics {
	public:
		BGGraphics();
		virtual ~BGGraphics();
        
        void update(float dt);
        void reload();

        void drawSplineMesh(ofMesh & mesh);

        bool renderWireframe;
        bool renderFlow;
        bool depthTest;
        
        /*
        void drawSeparateNode(BGNode* node, float nodeOffset);
        void drawSingleConnectedNode(BGNode* node, ofVec2f edgePt, float nodeOffset, float edgePtOffset);
        void drawInternalNodeSegment(BGNode* node, ofVec2f edgePt1, ofVec2f edgePt2, float sourceOffset, ofVec2f offsetSource, ofVec2f offsetSink1, ofVec2f offsetSink2);
        */
        
        //void drawSplineMesh(ofMesh & mesh, ofVec2f flowSource, ofVec2f flowSink1, ofVec2f flowSink2);
        
    private:
        ofShader mBezierShader;
        float mTimeParameter;
        
        float dot(ofVec2f a, ofVec2f b);
        float cross(ofVec2f a, ofVec2f b);
        bool getIntersection(ofVec2f p, ofVec2f r, ofVec2f q, ofVec2f s, ofVec2f & result);
        ofVec2f getBarycentricCoords(ofVec2f p, ofVec2f a, ofVec2f b, ofVec2f c);
};

#endif //BGGRAPHICS_H
