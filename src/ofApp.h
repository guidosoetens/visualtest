#pragma once

#include "ofMain.h"
#include "BGNetwork.h"
#include "BGGraphics.h"
#include "BGEntrance.h"
#include "BGObstacle.h"
#include "BGBackground.h"
#include "BGAntenna.h"
#include "BGCellGenerator.h"
#include <sys/time.h>
#include "BGResources.h"
#include "BGMenu.h"

#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 768

class ofApp : public ofBaseApp{

	public:
	
		BGNetwork mNetwork;
		vector<BGEntrance> mEntrances;
		vector<BGObstacle> mObstacles;
		vector<BGAntenna> mAntennas;

		ofTrueTypeFont mFont;

		struct timeval mLastSampledTime;
		
		BGGraphics mGraphics;
		BGBackground mBackground;

		BGCellGenerator mCellGenerator;

		ofShader mEyeShader;
		ofShader mEntranceShader;
		ofShader mObstacleShader;
		ofShader mBackgroundShader;
		ofShader mVoronoiShader;

		ofImage mCellImage;
		ofImage mBumpMap;
		ofImage mBubble;
		ofImage mBumpMap2;
		ofImage mStringImage;

		ofImage mEndpointBack;
		ofImage mEndpointFront;
		ofImage mEndpointFace;
		ofImage mMembrane;

		bool mCover;
		BGMenu mMenu;
		
		void reloadShaders();

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
		void exit();

	private:
		bool mIsScrolling;
		ofVec2f mPreviousScrollPosition;

};
