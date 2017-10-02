#include "BGTentacle.h"
#include "BGResources.h"

BGTentacle::BGTentacle(ofVec2f pos, float orientation)
:   mPosition(pos)
,   mOrientation(orientation)
,   mEntrance(ofVec2f(0,0), .5 * M_PI, false)
{
    mMesh.setMode(OF_PRIMITIVE_TRIANGLES);

    mEntrance.globalScale = 1.1;
    mEntrance.overrideColor(ofFloatColor(.5, .2, .9));
}

BGTentacle::~BGTentacle() {

}

void 
BGTentacle::update(float dt) {

    mEntrance.update(dt);

    // if(mMesh.getVertices().size() > 0)
    //     return;

    mMesh.clearVertices();
    mMesh.clearNormals();
    mMesh.clearIndices();

    mTimeParameter = fmodf(mTimeParameter + dt / 10.0, 1.0);

    const int numBones = 60;
    const float boneLength = 5.0;
    const float baseRadius = 30.0;
    const float topRadius = 10.0;
    const int circleSamples = 8;
    const float maxCurve = 5;
    ofVec2f normal(1,0);
    ofVec2f position(0,0);
    ofVec2f tangent(-normal.y, normal.x);

    mMesh.addVertex(position - tangent * baseRadius);
    mMesh.addVertex(position);
    mMesh.addVertex(position + tangent * baseRadius);

    mMesh.addNormal(-tangent);
    mMesh.addNormal(ofVec3f(0,0,1));
    mMesh.addNormal(tangent);

    float boneAngleBase = maxCurve * sinf(mTimeParameter * 2 * M_PI);
    for(int boneIdx=0; boneIdx<numBones; ++boneIdx) {

        position += normal * boneLength;

        float frac = 1 - boneIdx / (float)(numBones - 1);
        float radius = topRadius + frac * (baseRadius - topRadius);
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

        float boneAngle = powf(1 - frac, .5) * boneAngleBase;
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
BGTentacle::render(ofShader & mTentacleShader, ofShader & mEntranceShader) {
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(mPosition);
    ofRotate(180 * mOrientation / M_PI);
    mEntrance.renderBack(mEntranceShader);
    mTentacleShader.begin();
    mTentacleShader.setUniform1f("uRotation", mOrientation);
    mTentacleShader.setUniform1i("uShadowMode", 1);
    mMesh.draw();
    mTentacleShader.setUniform1i("uShadowMode", 0);
    mMesh.draw();
    mTentacleShader.end();
    mEntrance.render(mEntranceShader);

    // ofSetColor(255);
    // mMesh.drawWireframe();

    ofPopMatrix();
    ofPopStyle();
}