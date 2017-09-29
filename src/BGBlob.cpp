#include "BGBlob.h"
#include "BGResources.h"

BGBlob::BGBlob(ofVec2f pos, float orientation, float length)
:   mPosition(pos)
,   mOrientation(orientation)
,   mAnimParam(0)
,   mBaseLength(length)
{
    mMesh.setMode(OF_PRIMITIVE_TRIANGLES);
}

BGBlob::~BGBlob() {

}

void sampleSpline(ofVec2f p0, ofVec2f p1, ofVec2f p2, ofVec2f p3, float t, ofVec2f & p, ofVec2f & n) {
    float min_t = 1 - t;
    float tt = t * t;
    float min_tt = min_t * min_t;

    p = min_t * min_tt * p0 + 
}

void 
BGBlob::update(float dt) {

    mAnimParam = fmodf(mAnimParam + dt / 10.0, 1.0);

    float moveFrac = 0.1;
    float stayFrac = 0.5 - moveFrac;
    float stretch = 0;
    if(mAnimParam < stayFrac)
        stretch = 0;
    else if(mAnimParam < 0.5) {
        float t = (mAnimParam - stayFrac) / moveFrac;
        stretch = .5 - .5 * cosf(t * M_PI);
    }
    else if(mAnimParam < 1 - moveFrac)
        stretch = 1;
    else {
        float t = (mAnimParam - (1 - moveFrac)) / moveFrac;
        stretch = .5 + .5 * cosf(t * M_PI);
    }

    const int circleSamples = 20;
    const float baseWidth = 50;
    const float headRadius = 30;
    const float minLength = 100;






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
    mBlobShader.begin();
    mMesh.draw();
    mBlobShader.end();
    ofPopMatrix();
    ofPopStyle();
}