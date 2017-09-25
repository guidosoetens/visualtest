#include "BGNetwork.h"

BGNetwork::BGNetwork()
:   LockVertices(false) {

}

BGNetwork::~BGNetwork() {

}

void 
BGNetwork::setup(int width, int height) {

    touchNodes[0].nodeRadius = 40;
    touchNodes[0].bindSurface(ofVec2f(1,1).normalize());

    touchNodes[5].nodeRadius = 30;
    touchNodes[5].bindSurface(ofVec2f(-1,1).normalize());

    touchNodes[0].position = ofVec2f(390,155);
    touchNodes[1].position = ofVec2f(700,100);
    touchNodes[2].position = ofVec2f(200,500);
    touchNodes[3].position = ofVec2f(300,600);
    touchNodes[4].position = ofVec2f(600,600);
    touchNodes[5].position = ofVec2f(1600,200);
    
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

    mNetworkTarget.allocate(width, height, GL_RGBA);
    mNetworkTarget.begin();
    ofClear(0);
    mNetworkTarget.end();

    for(int i=0; i<TOUCHNODES_COUNT; ++i)
        allNodes.push_back(touchNodes + i);

    for(int i=0; i<INTERNALNODES_COUNT; ++i)
        allNodes.push_back(internalNodes + i);
}

void 
BGNetwork::render(BGGraphics & graphics, ofShader & eyeShader) {


    //get bounding rectangle:
    graphics.networkBounds = touchNodes[0].traverseGetBounds();
    graphics.networkBounds.scale(1 / 1024.0, 1 / 768.0);

    mNetworkTarget.begin();
    ofDisableAlphaBlending();
    ofClear(0,0,0,0);

    for(int i=0; i<2; ++i) {

        int idx = i == 0 ? 0 : 5;

        //render mesh
        touchNodes[idx].traverseBeginDraw(graphics);

        //render glow:
        graphics.boundOffset = 100;
        graphics.drawMode = 0;
        touchNodes[idx].traverseDraw(graphics);

        /*
        //render outline:
        glClear( GL_DEPTH_BUFFER_BIT );
        graphics.boundOffset = 3;
        graphics.drawMode = 1;
        touchNodes[idx].traverseDraw(graphics);
        */

        //render center:
        glClear( GL_DEPTH_BUFFER_BIT );
        graphics.boundOffset = 10;
        graphics.drawMode = 2;
        touchNodes[idx].traverseDraw(graphics);

        //draw face:
        //touchNodes[idx].drawFace(eyeShader);
    }
    mNetworkTarget.end();

    ofEnableAlphaBlending();
    mNetworkTarget.draw(0,0);
}

void 
BGNetwork::update(float dt) {
    if(!LockVertices) {
        for(int i=0; i<INTERNALNODES_COUNT; ++i)
            internalNodes[i].update(dt);
    }
}

void 
BGNetwork::mouseDown(ofVec2f pos) {
    for(int i=0; i<TOUCHNODES_COUNT; ++i) {
        if((pos - touchNodes[i].position).length() < touchNodes[i].nodeRadius) {
            touchNodes[i].hasInput = true;
            break;
        }
    }
}

void 
BGNetwork::mouseMove(ofVec2f pos) {
    for(int i=0; i<TOUCHNODES_COUNT; ++i) {
        if(touchNodes[i].hasInput) {
            touchNodes[i].position = pos;
            //cout << pos.x << " " << pos.y << endl;
        }
    }
}

void 
BGNetwork::mouseUp(ofVec2f pos) {
    for(int i=0; i<TOUCHNODES_COUNT; ++i) {
        touchNodes[i].hasInput = false;
    }
}