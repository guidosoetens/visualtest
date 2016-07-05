#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    mLockVertices = false;
    mNetworkShader.load("shaders/testShader");

    touchNodes[0].nodeRadius = 60;
    touchNodes[0].position = ofVec2f(50,50);
    touchNodes[1].position = ofVec2f(300,50);
    touchNodes[2].position = ofVec2f(50,400);
    touchNodes[3].position = ofVec2f(400,300);
    touchNodes[4].position = ofVec2f(400,450);

    internalNodes[0].pushNeighbor(touchNodes);
    internalNodes[0].pushNeighbor(touchNodes + 1);
    internalNodes[0].pushNeighbor(internalNodes + 1);
    
    internalNodes[1].pushNeighbor(touchNodes + 2);
    internalNodes[1].pushNeighbor(internalNodes);
    internalNodes[1].pushNeighbor(internalNodes + 2);
    
    internalNodes[2].pushNeighbor(internalNodes + 1);
    internalNodes[2].pushNeighbor(internalNodes + 3);
    
    internalNodes[3].pushNeighbor(touchNodes + 3);
    internalNodes[3].pushNeighbor(touchNodes + 4);
    internalNodes[3].pushNeighbor(internalNodes + 2);
    
    internalNodes[3].pushNeighbor(internalNodes + 4);
    internalNodes[4].pushNeighbor(internalNodes + 3);
    internalNodes[4].lockPosition = true;
    internalNodes[4].position = ofVec2f(500,500);
    
    //testje
    //internalNodes[2].pushNeighbor(touchNodes);

    /*
    internalNodes[0].setNeighbours(touchNodes, touchNodes + 1, internalNodes + 1);
    internalNodes[1].setNeighbours(touchNodes + 2, internalNodes, internalNodes + 2);
    internalNodes[2].setNeighbours(touchNodes + 3, touchNodes + 4, internalNodes + 1);
    */
    
    touchNodes[0].setNeighbour(internalNodes);
    touchNodes[1].setNeighbour(internalNodes);
    touchNodes[2].setNeighbour(internalNodes + 1);
    touchNodes[3].setNeighbour(internalNodes + 3);
    touchNodes[4].setNeighbour(internalNodes + 3);

    mNetworkTarget.allocate(SCENE_WIDTH, SCENE_HEIGHT, GL_RGBA);
    mNetworkTarget.begin();
    ofClear(0);
    mNetworkTarget.end();

    for(int i=0; i<TOUCHNODES_COUNT; ++i)
        allNodes.push_back(touchNodes + i);

    for(int i=0; i<INTERNALNODES_COUNT; ++i)
        allNodes.push_back(internalNodes + i);

    mFont.loadFont("Courier Bold", 14);
}

//--------------------------------------------------------------
void ofApp::update(){
    if(!mLockVertices) {
        for(int i=0; i<INTERNALNODES_COUNT; ++i)
            internalNodes[i].update(1 / 30.0);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(0,0,0,255);

    /*
        OF_BLENDMODE_DISABLED
        OF_BLENDMODE_ALPHA
        OF_BLENDMODE_ADD
        OF_BLENDMODE_SUBTRACT
        OF_BLENDMODE_MULTIPLY
        OF_BLENDMODE_SCREEN
    */

/*
    for(int i=0; i<allNodes.size(); ++i)
        allNodes[i]->traversalId = 0;

    mNetworkTarget.begin();
    ofClear(0,0,0,255);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);

    for(int i=0; i<INTERNALNODES_COUNT; ++i)
        internalNodes[i].renderGradient();

    mNetworkTarget.end();

    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    */

    /*
    mShader.begin();
    mShader.setUniform2f("uResolution", 1024, 768);
    mMesh.draw();
    mShader.end();

    mSquareShader.begin();
    mSquareShader.setUniform2f("uResolution", 1024, 768);
    mSquare.draw();
    mSquareShader.end();

    ofSetColor(255);
    mNetworkTarget.draw(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
    */

    /*
    ofSetColor(255);
    for(int i=0; i<INTERNALNODES_COUNT; ++i)
        internalNodes[i].render();

    for(int i=0; i<TOUCHNODES_COUNT; ++i)
        touchNodes[i].render();
    */

    mNetworkShader.begin();
    mNetworkShader.setUniform2f("uResolution", SCENE_WIDTH, SCENE_HEIGHT);

    ofEnableDepthTest();
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    for(int i=0; i<allNodes.size(); ++i)
        allNodes[i]->pushToMesh(mesh);
    mesh.draw();
    ofDisableDepthTest();

    mNetworkShader.end();

    ofSetColor(255);
    std::string str = std::string("[SPACE] lock vertices: ") + (mLockVertices ? "YES" : "NO");
    mFont.drawString(str, 0, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if(key == ' ') {
        mLockVertices = !mLockVertices;
    }
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
