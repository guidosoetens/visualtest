#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    mRenderWireframe = true;
    mTimeParameter = 0;
    mLockVertices = false;
    mRenderFlow = false;
    mDepthTest = true;
    mCenterFactor = 0.86;
    mOffsetFactor = 0.5;
    
    mNetworkShader.load("shaders/testShader");
    
    mBezierShader.load("shaders/bezierShader");

    touchNodes[0].nodeRadius = 50;
    touchNodes[0].position = ofVec2f(200,200);
    touchNodes[1].position = ofVec2f(800,200);
    touchNodes[2].position = ofVec2f(200,500);
    touchNodes[3].position = ofVec2f(700,300);
    touchNodes[4].position = ofVec2f(600,600);
    
    internalNodes[0].pushNeighbor(touchNodes);
    internalNodes[0].pushNeighbor(touchNodes + 1);
    internalNodes[0].pushNeighbor(internalNodes + 1);
    
    internalNodes[1].pushNeighbor(touchNodes + 2);
    internalNodes[1].pushNeighbor(internalNodes);
    internalNodes[1].pushNeighbor(internalNodes + 2);
    
    internalNodes[2].pushNeighbor(touchNodes + 3);
    internalNodes[2].pushNeighbor(touchNodes + 4);
    internalNodes[2].pushNeighbor(internalNodes + 1);
    
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
}

//--------------------------------------------------------------
void ofApp::update(){

    float dt = 1 / 30.0;

    mTimeParameter = fmodf(mTimeParameter + dt / 6.0, 1.0);

    if(!mLockVertices) {
        for(int i=0; i<INTERNALNODES_COUNT; ++i)
            internalNodes[i].update(dt);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofClear(0,0,0,255);

    //render mesh

    mNetworkShader.begin();
    mNetworkShader.setUniform2f("uResolution", SCENE_WIDTH, SCENE_HEIGHT);
    mNetworkShader.setUniform1f("uTimeParameter", mTimeParameter);
    mNetworkShader.setUniform1i("uRenderFlow", mRenderFlow ? 1 : 0);

    if(mDepthTest)
        ofEnableDepthTest();
    else
        ofDisableDepthTest();
            
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    touchNodes[0].traversePushToMesh(mesh, mCenterFactor, mOffsetFactor);
    mesh.draw();
    ofDisableDepthTest();

    mNetworkShader.end();
    
    //render shape:
    ofSetColor(255, 255, 255);
    if(mRenderWireframe)
        mesh.drawWireframe();
        
            
    mBezierShader.begin();
    mBezierShader.setUniform2f("uResolution", SCENE_WIDTH, SCENE_HEIGHT);
    mBezierShader.setUniform1f("uTimeParameter", mTimeParameter);
    
    ofMesh bezierMesh;
    bezierMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    for(int i=0; i<3; ++i) {
        ofVec2f p = touchNodes[i].position;
        bezierMesh.addVertex(ofVec3f(p.x, p.y, 0));
    }
    
    bezierMesh.addTexCoord(ofVec2f(0,0));
    bezierMesh.addTexCoord(ofVec2f(0.5,0));
    bezierMesh.addTexCoord(ofVec2f(1,1));
    
    bezierMesh.draw();
    mBezierShader.end();
        
    //render settings:
    ofSetColor(255, 255, 255);
    std::string str = std::string("[SPACE] lock vertices: ") + (mLockVertices ? "YES" : "NO");
    mFont.drawString(str, 0, 15);
    str = std::string("[W] render wireframe: ") + (mRenderWireframe ? "YES" : "NO");
    mFont.drawString(str, 0, 30);
    str = std::string("[F] render flow: ") + (mRenderFlow ? "YES" : "NO");
    mFont.drawString(str, 0, 45);
    mFont.drawString("[R] reload shader", 0, 60);
    str = std::string("[D] depth test: ") + (mDepthTest ? "YES" : "NO");
    mFont.drawString(str, 0, 75);

    std::stringstream stream (stringstream::in | stringstream::out);
    stream << mCenterFactor;
    str = std::string("[left / right] center pull: ") + stream.str();
    mFont.drawString(str, 0, 90);

    stream.str(std::string());
    stream << mOffsetFactor;
    str = std::string("[up / down] offset factor: ") + stream.str();
    mFont.drawString(str, 0, 105);
   
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if(key == ' ')
        mLockVertices = !mLockVertices;
    else if(key == 'w')
        mRenderWireframe = !mRenderWireframe;
    if(key == 'f')
        mRenderFlow = !mRenderFlow;
    if(key == 'r') {
        mNetworkShader.load("shaders/testShader");
        mBezierShader.load("shaders/bezierShader");
    }
    if(key == 'd')
        mDepthTest = !mDepthTest;
    if(key == OF_KEY_LEFT)
        mCenterFactor -= 0.005;
    if(key == OF_KEY_RIGHT)
        mCenterFactor += 0.005;
    if(key == OF_KEY_DOWN)
        mOffsetFactor -= 0.005;
    if(key == OF_KEY_UP)
        mOffsetFactor += 0.005;

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
