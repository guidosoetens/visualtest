#include "BGEye.h"
#include "BGResources.h"

BGEye::BGEye(ofVec2f pos, float orientation)
:   mPosition(pos)
,   mOrientation(orientation)
{

}

BGEye::~BGEye() {

}

void 
BGEye::update(float dt) {
    
}

void 
BGEye::render(ofShader & mEyeShader) {
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(mPosition);
    ofSetColor(255,0,0);
    ofCircle(0,0,50);
    ofPopMatrix();
    ofPopStyle();
}