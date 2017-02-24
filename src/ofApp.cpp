#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofDisableArbTex();

    mCover = true;
    mRenderText = false;

    reloadShaders();

    bgResources.reload();

    mBumpMap.loadImage("bumpMap1.png");
    mCellImage.loadImage("backgroundCell.png");
    //mBubble.loadImage("blobs.jpg");
    mBubble.loadImage("spots.jpg");
    mCellGenerator.copyToImage(mBumpMap2);

    mEndpointBack.loadImage("endpoint_back.png");
	mEndpointFront.loadImage("endpoint_front.png");
	mEndpointFace.loadImage("face.png");
    mMembrane.loadImage("membrain.png");
    mStringImage.loadImage("scribbles.jpg");

    mNetwork.setup(SCENE_WIDTH, SCENE_HEIGHT);
    //mEntrances.push_back(BGEntrance(ofVec2f(250, 250), .3 * M_PI));
    mObstacles.push_back(BGObstacle(ofVec2f(200, 300), 100, 5));
    mObstacles.push_back(BGObstacle(ofVec2f(800, 300), 160, 8));

    //mAntennas.push_back(BGAntenna(ofVec2f(260, 240), -M_PI / 5.0));

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

    for(int i=0; i<mAntennas.size(); ++i)
        mAntennas[i].update(dt);

    mBackground.update(dt);

    mCellGenerator.update(dt);

    mMenu.update(dt);
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofSetColor(255);

    float entrance_angle = 53;
    float entrance_scale = .35;
    
    ofClear(250,200,150,255);

    mBackground.render(mBackgroundShader, mCellImage, mBubble, mMembrane, mStringImage, SCENE_WIDTH, SCENE_HEIGHT);

    /*
    ofSetColor(255,100,0,50);

    ofPushMatrix();
    ofTranslate(SCENE_WIDTH/2, SCENE_HEIGHT/2);
    ofScale(1.6, 1.6);
    ofTranslate(-mMembrane.width/2, -mMembrane.height/2);
    mMembrane.draw(0,0);
    ofPopMatrix();

    ofSetColor(255);
    */

    for(int i=0; i<mEntrances.size(); ++i)
        mEntrances[i].renderBack(mEntranceShader);

    //draw entrances:
    {
        ofPushMatrix();
        ofTranslate(285, 235);
        
        ofPushMatrix();
        ofScale(entrance_scale, entrance_scale);
        ofRotate(entrance_angle);
        ofTranslate(-mEndpointBack.width/2, -mEndpointBack.height/2);
        mEndpointBack.draw(0, 0);
        ofPopMatrix();

        ofPopMatrix();
    }

    mNetwork.render(mGraphics, mEyeShader);

    for(int i=0; i<mObstacles.size(); ++i)
        mObstacles[i].render(mObstacleShader, mBumpMap2, SCENE_WIDTH, SCENE_HEIGHT);

    for(int i=0; i<mEntrances.size(); ++i)
        mEntrances[i].render(mEntranceShader);

    for(int i=0; i<mAntennas.size(); ++i)
        mAntennas[i].render();

    //draw entrances:
    {
        ofPushMatrix();
        ofTranslate(285, 235);
        
        ofPushMatrix();
        ofScale(entrance_scale, entrance_scale);
        ofRotate(entrance_angle);
        ofTranslate(-mEndpointFront.width/2, -mEndpointFront.height/2);
        mEndpointFront.draw(0, 0);
        ofPopMatrix();

        float faceScale = entrance_scale * 1.15;

        ofPushMatrix();
        ofScale(faceScale, faceScale);
        ofRotate(entrance_angle);
        ofTranslate(0,60);
        ofTranslate(-mEndpointFace.width/2, -mEndpointFace.height/2);
        mEndpointFace.draw(0, 0);
        ofPopMatrix();

        ofPopMatrix();
    }

    if(mCover) {
        ofClear(0);
        mCellGenerator.draw();
    }

    if(mRenderText) {
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

    mMenu.render(mFont);
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
    if(key == 's')
        bgResources.save();
    if(key == 357) //up
        mMenu.mouseScrolled(ofVec2f(0,0), 1);
    if(key == 359) //down
        mMenu.mouseScrolled(ofVec2f(0,0), -1);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    mRenderText = x > 800;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    if(mIsScrolling) {
        ofVec2f p(x, y);
        mMenu.mouseScrolled(p, y - mPreviousScrollPosition.y);
        mPreviousScrollPosition = p;
    }
    else {
        mNetwork.mouseMove(ofVec2f(x,y));
        mMenu.mouseMove(ofVec2f(x,y));
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    mIsScrolling = button != 0;
    if(mIsScrolling) {
        mPreviousScrollPosition = ofVec2f(x, y);
    }
    else {
        mNetwork.mouseDown(ofVec2f(x,y));
        mMenu.mouseDown(ofVec2f(x,y));
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    mNetwork.mouseUp(ofVec2f(x,y));
    mMenu.mouseUp(ofVec2f(x,y));
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

void ofApp::exit() {
    bgResources.save();
}
