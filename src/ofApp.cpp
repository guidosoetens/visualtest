#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    ofDisableArbTex();

    mTimeParameter = 0;
    mLockVertices = false;
    mDrawScene = false;
    mObstacleTimeParameter = 0;
    mCover = true;

    reloadShaders();

    mBumpMap.loadImage("bumpMap1.png");
    mBackgroundMesh.loadImage("backgroundCells.png");

    mEntrances.push_back(BGEntrance(ofVec2f(400, 400), 0));
    //mEntrances.push_back(BGEntrance(ofVec2f(800, 400), .25 * M_PI));


    touchNodes[0].nodeRadius = 60;
    touchNodes[0].bindSurface(ofVec2f(1,1).normalize());


    touchNodes[5].nodeRadius = 60;
    touchNodes[5].bindSurface(ofVec2f(-1,1).normalize());

    touchNodes[0].position = ofVec2f(200,200);
    touchNodes[1].position = ofVec2f(800,200);
    touchNodes[2].position = ofVec2f(200,500);
    touchNodes[3].position = ofVec2f(700,300);
    touchNodes[4].position = ofVec2f(600,600);
    touchNodes[5].position = ofVec2f(600,200);
    
    internalNodes[0].pushNeighbor(touchNodes);
    internalNodes[0].pushNeighbor(touchNodes + 1);
    internalNodes[0].pushNeighbor(internalNodes + 1);
    
    internalNodes[1].pushNeighbor(touchNodes + 2);
    internalNodes[1].pushNeighbor(internalNodes);
    internalNodes[1].pushNeighbor(internalNodes + 2);
    
    internalNodes[2].pushNeighbor(touchNodes + 3);
    internalNodes[2].pushNeighbor(touchNodes + 4);
    internalNodes[2].pushNeighbor(internalNodes + 1);

    //internalNodes[3].pushNeighbor(internalNodes + 2);
    //internalNodes[3].pushNeighbor(touchNodes + 4);
    
    touchNodes[0].setNeighbour(internalNodes);
    touchNodes[1].setNeighbour(internalNodes);
    touchNodes[2].setNeighbour(internalNodes + 1);
    touchNodes[3].setNeighbour(internalNodes + 2);
    touchNodes[4].setNeighbour(internalNodes + 2);

    mNetworkTarget.allocate(SCENE_WIDTH, SCENE_HEIGHT, GL_RGBA);
    mNetworkTarget.begin();
    ofClear(0);
    mNetworkTarget.end();

    for(int i=0; i<TOUCHNODES_COUNT; ++i)
        allNodes.push_back(touchNodes + i);

    for(int i=0; i<INTERNALNODES_COUNT; ++i)
        allNodes.push_back(internalNodes + i);

    mFont.loadFont("Courier Bold", 10);

    gettimeofday(&mLastSampledTime, NULL);
}

//--------------------------------------------------------------
void ofApp::update(){

    struct timeval currtime;
    gettimeofday(&currtime, NULL);

    long seconds = currtime.tv_sec - mLastSampledTime.tv_sec;
    long useconds = currtime.tv_usec - mLastSampledTime.tv_usec;
    float dt = round(seconds * 1000 + useconds / 1000.0) / 1000.0;
    mLastSampledTime = currtime;

    //float dt = 1 / 30.0;

    mTimeParameter = fmodf(mTimeParameter + dt / 6.0, 1.0);
    mGraphics.update(dt);

    mObstacleTimeParameter = fmodf(mObstacleTimeParameter + dt / 100.0, 1.0);

    if(!mLockVertices) {
        for(int i=0; i<INTERNALNODES_COUNT; ++i)
            internalNodes[i].update(dt);
    }

    for(int i=0; i<mEntrances.size(); ++i)
        mEntrances[i].update(dt);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofClear(250,200,150,255);

    for(int i=0; i<2; ++i) {

        int idx = i == 0 ? 0 : 5;

        //render mesh
        touchNodes[idx].traverseBeginDraw(mGraphics);

        //render glow:
        mGraphics.boundOffset = 40;
        mGraphics.drawMode = 0;
        touchNodes[idx].traverseDraw(mGraphics);

        //render outline:
        glClear( GL_DEPTH_BUFFER_BIT );
        mGraphics.boundOffset = 3;
        mGraphics.drawMode = 1;
        touchNodes[idx].traverseDraw(mGraphics);

        //render center:
        glClear( GL_DEPTH_BUFFER_BIT );
        mGraphics.boundOffset = 0;
        mGraphics.drawMode = 2;
        touchNodes[idx].traverseDraw(mGraphics);

        //draw face:
        touchNodes[idx].drawFace(mEyeShader);
    }

    mEntranceShader.begin();
    for(int i=0; i<mEntrances.size(); ++i)
        mEntrances[i].render(mEntranceShader);
    mEntranceShader.end();

    if(mDrawScene) {


        mBackgroundShader.begin();
        mBackgroundShader.setUniformTexture("uTexture", mBackgroundMesh.getTextureReference(), 0);
        mBackgroundShader.setUniform2f("uResolution", SCENE_WIDTH, SCENE_HEIGHT);
        mBackgroundShader.setUniform1f("uTime", fmodf(mObstacleTimeParameter * 10, 1.0));
        mBackgroundShader.setUniform4f("uBaseColor", 1, 0.725, 0.725, 1.);
        //mBackgroundMesh

        ofMesh bgQuad;
        bgQuad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        bgQuad.addVertex(ofVec3f(0,0,0));
        bgQuad.addVertex(ofVec3f(SCENE_WIDTH,0,0));
        bgQuad.addVertex(ofVec3f(0,SCENE_HEIGHT,0));
        bgQuad.addVertex(ofVec3f(SCENE_WIDTH,SCENE_HEIGHT,0));
        bgQuad.addTexCoord(ofVec2f(0,0));
        bgQuad.addTexCoord(ofVec2f(1,0));
        bgQuad.addTexCoord(ofVec2f(0,1));
        bgQuad.addTexCoord(ofVec2f(1,1));
        bgQuad.draw();

        mBackgroundShader.end();

        mVoronoiShader.begin();
        mVoronoiShader.setUniform2f("uResolution", SCENE_WIDTH, SCENE_HEIGHT);
        mVoronoiShader.setUniform1f("uTimeParameter", fmodf(mObstacleTimeParameter * 10, 1.0));
        bgQuad.draw();
        mVoronoiShader.end();

        for(int obsIdx=0; obsIdx<2; ++obsIdx) {
            ofMesh obsMesh;
            obsMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

            obsMesh.addVertex(ofVec3f(0,0,0));
            obsMesh.addNormal(ofVec3f(0,0,1));
            obsMesh.addTexCoord(ofVec2f(1,0));

            float rad = 100 + 60 * obsIdx;

            int reps = 5 + 3 * obsIdx;

            for(int i=0; i<reps; ++i) {

                float ang1 = i / (float)reps * 2 * M_PI;
                ofVec2f to1 = ofVec2f(cosf(ang1), sinf(ang1));

                float ang2 = (i + 1) / (float)reps * 2 * M_PI;
                ofVec2f to2 = ofVec2f(cosf(ang2), sinf(ang2));


                ofVec2f p0 = rad * to1;
                ofVec2f p3 = rad * to2;

                ofVec2f p1 = p0 + .2 * rad * ofVec2f(-to1.y, to1.x);
                ofVec2f p2 = p3 - .2 * rad * ofVec2f(-to2.y, to2.x);

                for(int it=0; it<20; ++it) {
                    
                    float t = it / 19.0;
                    float tMin = 1.0 - t;
                    float tt = t * t;
                    float ttMin = tMin * tMin;

                    ofVec2f p =     tMin * ttMin * p0 
                                +   3 * t * ttMin * p1 
                                +   3 * tt * tMin * p2 
                                +   t * tt * p3;

                    obsMesh.addVertex(ofVec3f(p.x, p.y,0));

                    p = p.normalize();
                    obsMesh.addNormal(ofVec3f(p.x,p.y,0));
                    obsMesh.addTexCoord(ofVec2f(-5 / rad,0));
                }
            }

            mObstacleShader.begin();
            mObstacleShader.setUniformTexture("uTexture", mBumpMap.getTextureReference(), 0);
            mObstacleShader.setUniform2f("uResolution", SCENE_WIDTH, SCENE_HEIGHT);
            mObstacleShader.setUniform1f("uTime", mObstacleTimeParameter);
            mObstacleShader.setUniform4f("uBaseColor", 1, 0, .4, 1);
            ofPushMatrix();
            ofTranslate(300 + 400 * obsIdx,300);
            obsMesh.draw();
            ofPopMatrix();
            mObstacleShader.end();
        }

    }

    if(mCover)
        ofClear(0);

    //render settings:
    ofSetColor(255, 255, 255);
    std::string str = std::string("[SPACE] lock vertices: ") + (mLockVertices ? "YES" : "NO");
    mFont.drawString(str, 0, 15);
    str = std::string("[W] render wireframe: ") + (mGraphics.renderWireframe ? "YES" : "NO");
    mFont.drawString(str, 0, 30);
    str = std::string("[F] render flow: ") + (mGraphics.renderFlow ? "YES" : "NO");
    mFont.drawString(str, 0, 45);
    mFont.drawString("[R] reload shader", 0, 60);
    str = std::string("[D] depth test: ") + (mGraphics.depthTest ? "YES" : "NO");
    mFont.drawString(str, 0, 75);
    str = std::string("[S] render scene: ") + (mDrawScene ? "YES" : "NO");
    mFont.drawString(str, 0, 90);
    str = std::string("[C] cover: ") + (mCover ? "YES" : "NO");
    mFont.drawString(str, 0, 105);
}

void ofApp::reloadShaders() {
    
    mGraphics.reload();
    mEyeShader.load("shaders/eyeShader");
    mEntranceShader.load("shaders/entranceShader");
    mObstacleShader.load("shaders/obstacleShader");
    mBackgroundShader.load("shaders/backgroundShader");
    mVoronoiShader.load("shaders/voronoiTestShader");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if(key == ' ')
        mLockVertices = !mLockVertices;
    else if(key == 'w')
        mGraphics.renderWireframe = !mGraphics.renderWireframe;
    if(key == 'f')
        mGraphics.renderFlow = !mGraphics.renderFlow;
    if(key == 'r')
        reloadShaders();
    if(key == 'd')
        mGraphics.depthTest = !mGraphics.depthTest;
    if(key == 's')
        mDrawScene = !mDrawScene;
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

    for(int i=0; i<TOUCHNODES_COUNT; ++i) {
        if(touchNodes[i].hasInput) {
            touchNodes[i].position = ofVec2f(x,y);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    ofVec2f pos(x,y);
    for(int i=0; i<TOUCHNODES_COUNT; ++i) {
        if((pos - touchNodes[i].position).length() < touchNodes[i].nodeRadius) {
            touchNodes[i].hasInput = true;
            break;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    for(int i=0; i<TOUCHNODES_COUNT; ++i) {
        touchNodes[i].hasInput = false;
    }
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
