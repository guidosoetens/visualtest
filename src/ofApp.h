#pragma once

#include "ofMain.h"
#include "BGTouchNode.h"
#include "BGGraphics.h"
#include "BGInternalNode.h"
#include <sys/time.h>

#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 768

#define TOUCHNODES_COUNT 5
#define INTERNALNODES_COUNT 4

class ofApp : public ofBaseApp{

	public:
	
        vector<BGNode*> allNodes;
		BGTouchNode touchNodes[TOUCHNODES_COUNT];
		BGInternalNode internalNodes[INTERNALNODES_COUNT];

		ofFbo mNetworkTarget;
		ofMesh mMesh;
		ofMesh mSquare;
		ofTrueTypeFont mFont;

		struct timeval mLastSampledTime;
		
		BGGraphics mGraphics;

		bool mLockVertices;
		float mTimeParameter;
		
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

};
