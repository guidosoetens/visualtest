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
		ofTrueTypeFont mReadableFont;

		ofShader mLoaderShader;
        ofShader mScreenShader;
        ofImage mBackgroundImage;
		ofImage mSettingsImage;
		ofImage mStarImage;
        ofFbo mFrontGuiTarget;
		float mTimeParameter;

		int mCurrMenuIdx;
		int mColorIdx;
		bool mShowMenu;
		float mZoomFactor;

		void drawLoader(float x, float y, float width, float height, float cornerRadius);
		void drawLevelButton(string txt, ofVec2f loc, float width, int numStars);
		void drawButton(string txt, ofVec2f loc, float width, bool flip = true);
		void drawCheckbox(string txt, ofVec2f loc);

        void drawLevelSelect();
		void drawBrim(bool drawLeft, float bottomHeight, float bottomLoaderWidth = 500.0f);
		void drawNotification();
		void drawWinLevel(bool extended);
		void drawWinSpeedrun();
		void drawPause();

		void drawStar(float innerRadius, float outerRadius, float borderRadius);
		void drawHexagon(ofVec2f pt, float radius, float baseWidth, float angleOffset, float lineThickness, float lineOpacity, float fillOpacity);
        struct timeval mLastSampledTime;

};
