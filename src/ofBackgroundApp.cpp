#include "ofBackgroundApp.h"

/*
#include <sstream>
#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()
        */
//--------------------------------------------------------------
void ofBackgroundApp::setup(){
    
    ofDisableArbTex();

    gettimeofday(&mLastSampledTime, NULL);

    mBackgroundShader.load("shaders/backgroundShader2");

    mTimeParameter = 0;
}

//--------------------------------------------------------------
void ofBackgroundApp::update(){

    struct timeval currtime;
    gettimeofday(&currtime, NULL);

    long seconds = currtime.tv_sec - mLastSampledTime.tv_sec;
    long useconds = currtime.tv_usec - mLastSampledTime.tv_usec;
    float dt = round(seconds * 1000 + useconds / 1000.0) / 1000.0;
    mLastSampledTime = currtime;

    mTimeParameter = fmodf(mTimeParameter + .01 * dt, 1.0);
}


//--------------------------------------------------------------
void ofBackgroundApp::draw(){

    mBackgroundShader.begin();
    mBackgroundShader.setUniform2f("uResolution", SCENE_WIDTH, SCENE_HEIGHT);
    mBackgroundShader.setUniform1f("uTime", mTimeParameter);

    ofMesh quad;
    quad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    quad.addVertex(ofVec3f(0,0,0));
    quad.addVertex(ofVec3f(SCENE_WIDTH,0,0));
    quad.addVertex(ofVec3f(0,SCENE_HEIGHT,0));
    quad.addVertex(ofVec3f(SCENE_WIDTH,SCENE_HEIGHT,0));

    quad.addTexCoord(ofVec2f(0,0));
    quad.addTexCoord(ofVec2f(1,0));
    quad.addTexCoord(ofVec2f(0,1));
    quad.addTexCoord(ofVec2f(1,1));
    quad.draw();

    mBackgroundShader.end();
}

//--------------------------------------------------------------
void ofBackgroundApp::keyPressed(int key){

    if(key == ' ') {
        mBackgroundShader.load("shaders/backgroundShader2");
    }

    /*
    int totalOptions = 6;

    if(key == OF_KEY_LEFT) {
        mCurrMenuIdx--;
        if(mCurrMenuIdx < 0)
            mCurrMenuIdx = totalOptions - 1;
    }
    else if(key == OF_KEY_RIGHT) {
        mCurrMenuIdx = (mCurrMenuIdx + 1) % totalOptions;
    }

    totalOptions = 4;
    if(key == OF_KEY_DOWN) {
        mColorIdx--;
        if(mColorIdx < 0)
            mColorIdx = totalOptions - 1;
    }
    else if(key == OF_KEY_UP) {
        mColorIdx = (mColorIdx + 1) % totalOptions;
    }

    if(key == ' ')
        mShowMenu = !mShowMenu;
    */
}

//--------------------------------------------------------------
void ofBackgroundApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofBackgroundApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofBackgroundApp::mouseDragged(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofBackgroundApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofBackgroundApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofBackgroundApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofBackgroundApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofBackgroundApp::dragEvent(ofDragInfo dragInfo){

}
