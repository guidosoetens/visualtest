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
        
        void renderSeparateNode(ofVec2f position, float nodeRadius);
        void renderSingleConnectedNode(ofVec2f position, float nodeRadius, ofVec2f edgePt);
        void renderInternalNodePart(ofVec2f position, ofVec2f edgePt1, ofVec2f edgePt2);
        
    private:
        ofShader mBezierShader;
        float mTimeParameter;
        
        float dot(ofVec2f a, ofVec2f b);
        float cross(ofVec2f a, ofVec2f b);
        bool getIntersection(ofVec2f p, ofVec2f r, ofVec2f q, ofVec2f s, ofVec2f & result);
        ofVec2f getBarycentricCoords(ofVec2f p, ofVec2f a, ofVec2f b, ofVec2f c);
};

#endif //BGGRAPHICS_H
