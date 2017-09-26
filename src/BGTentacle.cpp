#include "BGTentacle.h"
#include "BGResources.h"

BGTentacle::BGTentacle(ofVec2f pos, float orientation)
:   mPosition(pos)
,   mOrientation(orientation)
{
    mMesh.setMode(OF_PRIMITIVE_TRIANGLES);
}

BGTentacle::~BGTentacle() {

}

void 
BGTentacle::update(float dt) {

    mMesh.clearVertices();
    mMesh.clearNormals();

    mTimeParameter = fmodf(mTimeParameter + dt / 5.0, 1.0);

    const int numBones = 10;
    const float boneLength = 20.0;
    const float baseWidth = 10.0;
    ofVec2f normal(1,0);
    ofVec2f position(0,0);
    ofVec2f tangent(-normal.y, normal.x);

    for(int boneIdx=0; boneIdx<numBones; ++boneIdx) {


    }
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