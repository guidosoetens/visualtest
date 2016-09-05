#pragma once

#include "ofMain.h"
#include <sys/time.h>

#define SCENE_WIDTH 1024
#define SCENE_HEIGHT 768

#define TOUCHNODES_COUNT 6
#define INTERNALNODES_COUNT 3

class ofScreenApp : public ofBaseApp{

	public:

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

    private:
		ofTrueTypeFont mFont;
		ofTrueTypeFont mLevelButtonFont;
		ofTrueTypeFont mSymbolFont;

		ofShader mLoaderShader;
        ofShader mScreenShader;
        ofImage mBackgroundImage;
        ofFbo mFrontGuiTarget;
		float mTimeParameter;

		void drawLoader(float x, float y, float width, float height, float cornerRadius);
		void drawLevelButton(string txt, ofVec2f loc, float width);
		void drawButton(string txt, ofVec2f loc, float width);
        void drawMenuElements();
		void drawHexagon(ofVec2f pt, float radius, float baseWidth, float angleOffset, float lineThickness, float lineOpacity, float fillOpacity);
        struct timeval mLastSampledTime;

};
