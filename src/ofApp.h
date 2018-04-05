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
#include "PixelSpullies.h"
#include "BGCog.h"
#include "BGFooObject.h"
#include "BGEye.h"
#include "BGTentacle.h"
#include "BGBlob.h"
#include "BGEntranceMesh.h"
#include "Uitprobeersel.h"

#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 768

class ofApp : public ofBaseApp{

	public:
	
		BGNetwork mNetwork;
		vector<BGEntrance> mEntrances;
		vector<BGObstacle> mObstacles;
		vector<BGAntenna> mAntennas;
		vector<BGCog> mCogs;
		vector<BGEye> mEyes;
		vector<BGTentacle> mTentacles;
		vector<BGBlob> mBlobs;
		vector<BGEntranceMesh> mEntranceMeshes;

		ofTrueTypeFont mFont;

		struct timeval mLastSampledTime;
		
		BGGraphics mGraphics;
		BGBackground mBackground;

		BGCellGenerator mCellGenerator;

		ofShader mEyeShader;
		ofShader mEntranceShader;
		ofShader mObstacleShader;
		ofShader mHoneyObstacleShader;
		ofShader mMechaObstacleShader;
		ofShader mBackgroundShader;
		ofShader mVoronoiShader;
		ofShader mRegularEntranceShader;
		ofShader mCogShader;
		ofShader mTentacleShader;
		ofShader mBlobShader;
		ofShader mEyeContainerShader;
		ofShader mEntranceMeshShader;

		ofImage mCellImage;
		ofImage mBumpMap;
		ofImage mBubble;
		ofImage mBumpMap2;
		ofImage mStringImage;

		ofImage mEndpointBackGray;
		ofImage mEndpointFrontGray;

		ofImage mEndpointBack;
		ofImage mEndpointFront;
		ofImage mEndpointFace;
		ofImage mMembrane;

		PixelSpullies mPixelSpullies;

		bool mCover;
		bool mDrawCog;
		bool mDrawTentacle;
		bool mDrawEye;
		bool mDrawBlob;
		bool mDrawEntranceMesh;
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
		bool mRenderText;

		BGFooObject mFooObject;

		Uitprobeersel mUitprobeersel;

};
