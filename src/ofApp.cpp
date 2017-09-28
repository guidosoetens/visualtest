#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofDisableArbTex();

    mCover = false;
    mRenderText = false;
    mDrawCog = false;
    mDrawTentacle = false;
    mDrawEye = false;
    mDrawBlob = false;

    reloadShaders();

    bgResources.reload();

    mPixelSpullies.setup();

    mBumpMap.loadImage("bumpMap1.png");
    mCellImage.loadImage("backgroundCell.png");
    //mBubble.loadImage("blobs.jpg");
    mBubble.loadImage("spots.jpg");
    mCellGenerator.copyToImage(mBumpMap2);

    // mEndpointBack.loadImage("endpoint_back.png");
	// mEndpointFront.loadImage("endpoint.png");
    // mEndpointFace.loadImage("face.png");
    mEndpointBack.loadImage("back_base.png");
	mEndpointFront.loadImage("front_base.png");
    mEndpointFace.loadImage("mask.png");
    mEndpointBackGray.loadImage("back_gray.png");
    mEndpointFrontGray.loadImage("front_gray.png");

    mMembrane.loadImage("membrain.png");
    mStringImage.loadImage("scribbles.jpg");

    mNetwork.setup(SCENE_WIDTH, SCENE_HEIGHT);

    mEntrances.push_back(BGEntrance(ofVec2f(330, 200), .3 * M_PI, true));
    mEntrances.push_back(BGEntrance(ofVec2f(640, 435), -0.57 * M_PI, false));

    mObstacles.push_back(BGObstacle(ofVec2f(200, 300), 160, 5, false));
    mObstacles.push_back(BGObstacle(ofVec2f(800, 300), 160, 8, true));

    mEyes.push_back(BGEye(ofVec2f(800, 300), .3 * M_PI));
    mTentacles.push_back(BGTentacle(ofVec2f(750, 600), .7 * M_PI));
    mTentacles.push_back(BGTentacle(ofVec2f(680, 220), -.85 * M_PI));
    //mBlobs.push_back(BGBlob(ofVec2f(160, 440), .6 * M_PI));
    mBlobs.push_back(BGBlob(ofVec2f(320, 400), .2 * M_PI));

    vector<BGEye> mEyes;
    vector<BGTentacle> mTentacles;
    vector<BGBlob> mBlobs;

    //mCogs.push_back(BGCog(ofVec2f(1024 / 2.0, 768 / 2.0), -.3 * M_PI));
    mCogs.push_back(BGCog(ofVec2f(400, 600), -.3 * M_PI));
    //mCogs.push_back(BGCog(ofVec2f(850, 200), -.6 * M_PI));

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

    for(int i=0; i<mCogs.size(); ++i)
        mCogs[i].update(dt);
    
    for(int i=0; i<mEyes.size(); ++i)
        mEyes[i].update(dt);

    for(int i=0; i<mTentacles.size(); ++i)
        mTentacles[i].update(dt);

    for(int i=0; i<mBlobs.size(); ++i)
        mBlobs[i].update(dt);
 
    mBackground.update(dt);

    mCellGenerator.update(dt);

    mMenu.update(dt);

    mFooObject.update(dt);
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofSetColor(255);

    float entrance_angle = 53;
    float entrance_scale = .2;
    ofVec2f entrance_pos(330, 200);
    
    ofClear(250,200,150,255);

    mBackground.render(mBackgroundShader, mCellImage, mBubble, mMembrane, mStringImage, SCENE_WIDTH, SCENE_HEIGHT);

    for(int i=0; i<mEntrances.size(); ++i)
        mEntrances[i].renderBack(mRegularEntranceShader);

    mNetwork.render(mGraphics);

    ofShader* obsShader = &mHoneyObstacleShader;
    for(int i=0; i<mObstacles.size(); ++i)
        mObstacles[i].render(*obsShader, mBumpMap2, SCENE_WIDTH, SCENE_HEIGHT);


    for(int i=0; i<mEntrances.size(); ++i)
        mEntrances[i].render(mRegularEntranceShader);

    for(int i=0; i<mAntennas.size(); ++i)
        mAntennas[i].render();

    if(mDrawCog) {
        for(int i=0; i<mCogs.size(); ++i)
            mCogs[i].render(mCogShader);
    }

    if(mDrawTentacle) {
        for(int i=0; i<mTentacles.size(); ++i)
            mTentacles[i].render(mTentacleShader);
    }

    if(mDrawEye) {
        for(int i=0; i<mEyes.size(); ++i)
            mEyes[i].render(mEyeShader);
    }

    if(mDrawBlob) {
        for(int i=0; i<mBlobs.size(); ++i)
            mBlobs[i].render(mBlobShader);
    }

    if(mCover) {
        ofClear(100);
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
        str = std::string("[Q] cog: ") + (mDrawCog ? "YES" : "NO");
        mFont.drawString(str, 0, 105);
        str = std::string("[T] tentacle: ") + (mDrawTentacle ? "YES" : "NO");
        mFont.drawString(str, 0, 120);
        str = std::string("[B] blob: ") + (mDrawBlob ? "YES" : "NO");
        mFont.drawString(str, 0, 135);
        str = std::string("[E] eye: ") + (mDrawEye ? "YES" : "NO");
        mFont.drawString(str, 0, 150);
    }

    mMenu.render(mFont);

    //mPixelSpullies.render();

    //mFooObject.render();
}

void ofApp::reloadShaders() {
    
    mGraphics.reload();
    mEntranceShader.load("shaders/entranceShader");
    mObstacleShader.load("shaders/honeyObstacleShader");
    //mObstacleShader.load("shaders/obstacleShader");
    //mBackgroundShader.load("shaders/backgroundShader2");
    mBackgroundShader.load("shaders/patternBGShader");
    mVoronoiShader.load("shaders/voronoiTestShader");
    mRegularEntranceShader.load("shaders/regularEntranceShader");
    mHoneyObstacleShader.load("shaders/honeyObstacleShader");
    mMechaObstacleShader.load("shaders/mechaObstacleShader");
    mCogShader.load("shaders/cogShader");
    mTentacleShader.load("shaders/tentacleShader");
    mEyeShader.load("shaders/eyeShader");
    mBlobShader.load("shaders/tentacleShader");
    mPixelSpullies.setup();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if(key == ' ') {
        mNetwork.LockVertices = !mNetwork.LockVertices;
        mPixelSpullies.process();
    }
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
    if(key == 'q')
        mDrawCog = !mDrawCog;
    if(key == 't')
        mDrawTentacle = !mDrawTentacle;
    if(key == 'b')
        mDrawBlob = !mDrawBlob;
    if(key == 'e')
        mDrawEye = !mDrawEye;
    if(key == 's')
        bgResources.save();
    if(key == 357) //up
        mMenu.mouseScrolled(ofVec2f(0,0), 1);
    if(key == 359) //down
        mMenu.mouseScrolled(ofVec2f(0,0), -1);
    if(key == 356) //left
        bgResources.currentStyleIndex = bgResources.currentStyleIndex == 0 ? NUM_STYLES - 1 : bgResources.currentStyleIndex - 1;
    if(key == 358) //right
        bgResources.currentStyleIndex = (bgResources.currentStyleIndex + 1) % NUM_STYLES;
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
