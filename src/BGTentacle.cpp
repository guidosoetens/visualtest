#include "BGTentacle.h"
#include "BGResources.h"

BGTentacle::BGTentacle(ofVec2f pos, float orientation)
:   mPosition(pos)
,   mOrientation(orientation)
{
    
}

BGTentacle::~BGTentacle() {

}

void 
BGTentacle::update(float dt) {
    
}

void 
BGTentacle::render() {
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(mPosition);
    ofRotate(180 * mOrientation / M_PI);
    ofSetColor(150,30,100);
    ofCircle(0,0,10);
    ofRect(0,-10,100,20);
    ofPopMatrix();
    ofPopStyle();
}