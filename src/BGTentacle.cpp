#include "BGTentacle.h"
#include "BGResources.h"

BGTentacle::BGTentacle(ofVec2f pos, float orientation)
:   mPosition(pos)
,   mOrientation(orientation)
{
    mMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    mTentacleShader.load("shaders/tentacleShader");
}

BGTentacle::~BGTentacle() {

}

void 
BGTentacle::update(float dt) {

    mMesh.clearVertices();
    mMesh.clearNormals();
    mMesh.clearColors();

    mTimeParameter = fmodf(mTimeParameter + dt / 10.0, 1.0);

    const int numBones = 30;
    const float boneLength = 10.0;
    const float baseRadius = 30.0;
    const float topRadius = 10.0;
    const int circleSamples = 8;
    ofVec2f normal(1,0);
    ofVec2f position(0,0);
    ofVec2f tangent(-normal.y, normal.x);

    mMesh.addVertex(position - tangent * baseRadius);
    mMesh.addVertex(position);
    mMesh.addVertex(position + tangent * baseRadius);

    mMesh.addNormal(-tangent);
    mMesh.addNormal(ofVec3f(0,0,1));
    mMesh.addNormal(tangent);

    mMesh.addColor(ofFloatColor(1,0,0));
    mMesh.addColor(ofFloatColor(0,1,0));
    mMesh.addColor(ofFloatColor(0,0,1));

    float boneAngle = 5 * sinf(mTimeParameter * 2 * M_PI);
    for(int boneIdx=0; boneIdx<numBones; ++boneIdx) {

        position += normal * boneLength;

        float radius = topRadius + (1 - boneIdx / (float)(numBones - 1)) * (baseRadius - topRadius);
        mMesh.addVertex(position - tangent * radius);
        mMesh.addVertex(position);
        mMesh.addVertex(position + tangent * radius);
        mMesh.addNormal(-tangent);
        mMesh.addNormal(ofVec3f(0,0,1));
        mMesh.addNormal(tangent);

        int idx = (boneIdx + 1) * 3;
        mMesh.addTriangle(idx, idx + 1, idx - 3);
        mMesh.addTriangle(idx + 1, idx - 3, idx - 2);
        mMesh.addTriangle(idx + 1, idx - 2, idx - 1);
        mMesh.addTriangle(idx + 1, idx + 2, idx - 1);

        normal = normal.rotate(boneAngle);
        tangent = ofVec2f(-normal.y, normal.x);
    }

    int lastIdx = numBones * 3 + 2;

    //add circle top:
    for(int i=0; i<circleSamples; ++i) {
        float frac = (i + 1) / (float)(circleSamples + 1);
        float angle = (.5 - frac) * 180;
        ofVec2f locNormal = normal;//.rotate(angle);
        locNormal.rotate(angle);

        mMesh.addVertex(position + locNormal * topRadius);
        mMesh.addNormal(locNormal);
        //previous : current : center
        mMesh.addTriangle(lastIdx + i, lastIdx + 1 + i, lastIdx - 1);
    }

    mMesh.addTriangle(lastIdx + circleSamples, lastIdx - 1, lastIdx - 2);
}

void 
BGTentacle::render() {
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(mPosition);
    ofRotate(180 * mOrientation / M_PI);
    ofSetColor(150,30,100);
    ofCircle(0,0,10);
    //ofRect(0,-10,100,20);
    mTentacleShader.begin();
    mTentacleShader.setUniform1i("uShadowMode", 1);
    mMesh.draw();
    mTentacleShader.setUniform1i("uShadowMode", 0);
    mMesh.draw();
    mTentacleShader.end();
    ofPopMatrix();
    ofPopStyle();
}