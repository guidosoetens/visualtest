#include "BGBlob.h"
#include "BGResources.h"

BGBlob::BGBlob(ofVec2f pos, float orientation)
:   mPosition(pos)
,   mOrientation(orientation)
{

}

BGBlob::~BGBlob() {

}

void 
BGBlob::update(float dt) {
    
}

void 
BGBlob::render(ofShader & mBlobShader) {
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(mPosition);
    ofRotate(180 * mOrientation / M_PI);
    ofSetColor(100,200,255);
    ofCircle(0,0,10);
    ofRect(0,-10,100,20);
    ofPopMatrix();
    ofPopStyle();
}