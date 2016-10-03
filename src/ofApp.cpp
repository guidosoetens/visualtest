#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    ofDisableArbTex();

    mCover = false;

    reloadShaders();

    mBumpMap.loadImage("bumpMap1.png");
    mCellImage.loadImage("backgroundCell.png");
    mBubble.loadImage("bubble.png");

    mNetwork.setup(SCENE_WIDTH, SCENE_HEIGHT);
    mEntrances.push_back(BGEntrance(ofVec2f(250, 250), .3 * M_PI));
    mObstacles.push_back(BGObstacle(ofVec2f(200, 300), 100, 5));
    mObstacles.push_back(BGObstacle(ofVec2f(800, 300), 160, 8));

    mFont.loadFont("Courier Bold", 10);

    gettimeofday(&mLastSampledTime, NULL);
}

//--------------------------------------------------------------
void ofApp::update(){

    //sample elapsed time:

    struct timeval currtime;
    gettimeofday(&currtime, NULL);

    long seconds = currtime.tv_sec - mLastSampledTime.tv_sec;
    long useconds = currtime.tv_usec - mLastSampledTime.tv_usec;
    mLastSampledTime = currtime;

    float dt = round(seconds * 1000 + useconds / 1000.0) / 1000.0;

    //update game components:

    mGraphics.update(dt);
    mNetwork.update(dt);

    for(int i=0; i<mObstacles.size(); ++i)
        mObstacles[i].update(dt);

    for(int i=0; i<mEntrances.size(); ++i)
        mEntrances[i].update(dt);

    mBackground.update(dt);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofClear(250,200,150,255);

    mBackground.render(mBackgroundShader, mCellImage, mBubble, SCENE_WIDTH, SCENE_HEIGHT);

    for(int i=0; i<mEntrances.size(); ++i)
        mEntrances[i].renderBack(mEntranceShader);

    mNetwork.render(mGraphics, mEyeShader);

    for(int i=0; i<mObstacles.size(); ++i)
        mObstacles[i].render(mObstacleShader, mBumpMap, SCENE_WIDTH, SCENE_HEIGHT);

    for(int i=0; i<mEntrances.size(); ++i)
        mEntrances[i].render(mEntranceShader);

    if(mCover)
        ofClear(0);

    //render settings:
    ofSetColor(255, 255, 255);
    std::string str = std::string("[SPACE] lock vertices: ") + (mNetwork.LockVertices ? "YES" : "NO");
    mFont.drawString(str, 0, 15);
    str = std::string("[W] render wireframe: ") + (mGraphics.renderWireframe ? "YES" : "NO");
    mFont.drawString(str, 0, 30);
    str = std::string("[F] render flow: ") + (mGraphics.renderFlow ? "YES" : "NO");
    mFont.drawString(str, 0, 45);
    mFont.drawString("[R] reload shader", 0, 60);
    str = std::string("[D] depth test: ") + (mGraphics.depthTest ? "YES" : "NO");
    mFont.drawString(str, 0, 75);
    str = std::string("[C] cover: ") + (mCover ? "YES" : "NO");
    mFont.drawString(str, 0, 90);
}

void ofApp::reloadShaders() {
    
    mGraphics.reload();
    mEyeShader.load("shaders/eyeShader");
    mEntranceShader.load("shaders/entranceShader");
    mObstacleShader.load("shaders/obstacleShader");
    mBackgroundShader.load("shaders/backgroundShader2");
    mVoronoiShader.load("shaders/voronoiTestShader");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if(key == ' ')
        mNetwork.LockVertices = !mNetwork.LockVertices;
    else if(key == 'w')
        mGraphics.renderWireframe = !mGraphics.renderWireframe;
    if(key == 'f')
        mGraphics.renderFlow = !mGraphics.renderFlow;
    if(key == 'r')
        reloadShaders();
    if(key == 'd')
        mGraphics.depthTest = !mGraphics.depthTest;
    if(key == 'c')
        mCover = !mCover;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    mNetwork.mouseMove(ofVec2f(x,y));
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    mNetwork.mouseDown(ofVec2f(x,y));
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    mNetwork.mouseUp(ofVec2f(x,y));
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
