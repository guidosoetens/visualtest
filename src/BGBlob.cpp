#include "BGBlob.h"
#include "BGResources.h"

#define HEAD_RADIUS 35

BGBlob::BGBlob(ofVec2f pos, float orientation, float length)
:   mPosition(pos)
,   mOrientation(orientation)
,   mAnimParam(0)
,   mBaseLength(length)
{
    mMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    mBorderMesh.setMode(OF_PRIMITIVE_TRIANGLES);

    mFrontImage.loadImage("front.png");
    mBackImage.loadImage("back.png");
}

BGBlob::~BGBlob() {

}

void BGBlob::sampleSpline(ofVec2f p0, ofVec2f p1, ofVec2f p2, ofVec2f p3, float t, ofVec2f & p, ofVec2f & n) {
    float min_t = 1 - t;
    float tt = t * t;
    float min_tt = min_t * min_t;

    p = min_t * min_tt * p0 + 3 * min_tt * t * p1 + 3 * min_t * tt * p2 + t * tt * p3;
    n = 3 * min_tt * (p1 - p0) + 6 * min_t * t * (p2 - p1) + 3 * tt * (p3 - p2);
    n.normalize();
    n = ofVec2f(-n.y, n.x);
}

void 
BGBlob::update(float dt) {

    mAnimParam = fmodf(mAnimParam + dt / 10.0, 1.0);
    //mOrientation = fmodf(mOrientation + dt, 2 * M_PI);

    //mOrientation = .75 * M_PI;

    mMesh.clearVertices();
    mMesh.clearNormals();
    mMesh.clearIndices();
    mMesh.clearTexCoords();

    mBorderMesh.clearVertices();
    mBorderMesh.clearNormals();
    mBorderMesh.clearIndices();
    mBorderMesh.clearTexCoords();

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
    mCurrentStretch = stretch;

    //stretch = .1;

    const int halfCircleSamples = 100;//10;
    const int splineSamples = 100;//30;
    const float baseWidth = 50;
    const float headRadius = HEAD_RADIUS;
    const float minLength = 60;

    //half angle within the head circle:
    float theta = (.25 + .4 * stretch) * M_PI;
    ofVec2f outVector(cosf(theta), sinf(theta));
    ofVec2f outTangent(-outVector.y, outVector.x);


    float length = minLength + stretch * (mBaseLength - minLength);
    mCurrentLength = length;

    ofVec2f headLoc(length, 0);

    float tangentFactor = .3 + .15 * (1 - stretch);

    ofVec2f p0(0, .5 * baseWidth);
    ofVec2f p1 = p0 + ofVec2f(-outTangent.x, outTangent.y) * tangentFactor * length;//(.25 * length, .25 * baseWidth);
    ofVec2f p3 = headLoc + outVector * headRadius;
    ofVec2f p2 = p3 + outTangent * headRadius;//tangentFactor * length; //(.75 * length, .5 * headRadius);

    vector<ofVec2f> points;
    vector<ofVec2f> normals;

    for(int i=0; i<splineSamples; ++i) {
        float t = i / (float)(splineSamples - 1);
        ofVec2f p, n;
        sampleSpline(p0, p1, p2, p3, t, p, n);
        points.push_back(p);
        normals.push_back(n);

        // mMesh.addVertex(p);
        // mMesh.addVertex(ofVec2f(p.x, 0));
        // mMesh.addVertex(ofVec2f(p.x, 0));
        // mMesh.addVertex(ofVec2f(p.x, -p.y));

        // mMesh.addNormal(n);
        // mMesh.addNormal(ofVec3f(n.x,0,n.y));
        // mMesh.addNormal(ofVec3f(n.x,0,n.y));
        // mMesh.addNormal(ofVec2f(n.x, -n.y));

        // if(i > 0) {
        //     int idx = 4 * i;
        //     mMesh.addTriangle(idx, idx - 4, idx - 3);
        //     mMesh.addTriangle(idx, idx - 3, idx + 1);
        //     mMesh.addTriangle(idx + 2, idx - 2, idx + 3);
        //     mMesh.addTriangle(idx + 3, idx - 2, idx - 1);
        // }
    }

    //add circle:
    for(int i=0; i<halfCircleSamples; ++i) {
        float t = (i + 1) / (float)(halfCircleSamples);
        float angle = (1 - t) * theta;
        ofVec2f to(cosf(angle), sinf(angle));
        points.push_back(headLoc + to * headRadius);
        normals.push_back(to);

        // mMesh.addVertex(headLoc + to * headRadius);
        // mMesh.addVertex(headLoc + ofVec2f(to.x, 0) * headRadius);
        // mMesh.addVertex(headLoc + ofVec2f(to.x, 0) * headRadius);
        // mMesh.addVertex(headLoc + ofVec2f(to.x, -to.y) * headRadius);

        // mMesh.addNormal(to);
        // mMesh.addNormal(ofVec3f(to.x,0,to.y));
        // mMesh.addNormal(ofVec3f(to.x,0,to.y));
        // mMesh.addNormal(ofVec2f(to.x, -to.y));

        // int idx = 4 * (splineSamples + i);
        // mMesh.addTriangle(idx, idx - 4, idx - 3);
        // mMesh.addTriangle(idx, idx - 3, idx + 1);
        // mMesh.addTriangle(idx + 2, idx - 2, idx + 3);
        // mMesh.addTriangle(idx + 3, idx - 2, idx - 1);
    }

    //generate mesh:
    float ptCnt = points.size();
    for(int i=0; i<ptCnt; ++i) {


        ofVec2f p = points[i];
        ofVec2f n = normals[i];

        float frac = i / (float)(ptCnt);
        float baseFrac = .5 * ofClamp(p.x / length, 0, 1);
        float outTexX = .5 * frac;
        float baseY = 0;

        if(p.x > length) {
            baseY = (p.x - length) / headRadius;
        }

        mMesh.addVertex(p);
        mMesh.addVertex(ofVec2f(p.x, 0));
        mMesh.addVertex(ofVec2f(p.x, 0));
        mMesh.addVertex(ofVec2f(p.x, -p.y));

        mMesh.addNormal(n);
        mMesh.addNormal(ofVec3f(n.x,0,n.y));
        mMesh.addNormal(ofVec3f(n.x,0,n.y));
        mMesh.addNormal(ofVec2f(n.x, -n.y));

        mMesh.addTexCoord(ofVec2f(outTexX, 1));
        mMesh.addTexCoord(ofVec2f(baseFrac, baseY));
        mMesh.addTexCoord(ofVec2f(1 - baseFrac, baseY));
        mMesh.addTexCoord(ofVec2f(1 - outTexX, 1));

        if(i > 0) {
            int idx = 4 * i;
            mMesh.addTriangle(idx, idx - 4, idx - 3);
            mMesh.addTriangle(idx, idx - 3, idx + 1);
            mMesh.addTriangle(idx + 2, idx - 2, idx + 3);
            mMesh.addTriangle(idx + 3, idx - 2, idx - 1);
        }

        mBorderMesh.addVertex(p);
        mBorderMesh.addVertex(p + 3 * n);
        mBorderMesh.addVertex(ofVec2f(p.x, -p.y));
        mBorderMesh.addVertex(ofVec2f(p.x, -p.y) + 3 * ofVec2f(n.x, -n.y));

        if(i > 0) {
            int idx = 4 * i;
            mBorderMesh.addTriangle(idx, idx - 4, idx - 3);
            mBorderMesh.addTriangle(idx, idx - 3, idx + 1);
            mBorderMesh.addTriangle(idx + 2, idx - 2, idx + 3);
            mBorderMesh.addTriangle(idx + 3, idx - 2, idx - 1);
        }
    }
}

void 
BGBlob::render(ofShader & mBlobShader, ofShader & mEntranceShader) {

    float entranceScale = 0.25;

    ofPushStyle();
    ofPushMatrix();
    ofTranslate(mPosition);
    ofRotate(180 * mOrientation / M_PI);
    
    ofPushMatrix();
    ofScale(entranceScale, entranceScale);
    ofRotate(90);
    mEntranceShader.begin();
    ofFloatColor entranceColor = bgResources.getColorSetting(ObstacleLightColorKey)->value;
    mEntranceShader.setUniform3f("uColor", entranceColor.r, entranceColor.g, entranceColor.b);
    mEntranceShader.setUniform1f("uDarken", 1);
    mBackImage.draw(-mBackImage.width / 2, -mBackImage.height / 2 - 150);
    mEntranceShader.end();
    ofPopMatrix();


    mBlobShader.begin();

    for(int i=0; i<3; ++i) {
        ofColor c = bgResources.getColorSetting(BGResourceKey(BlobColor1Key + i))->value;
        mBlobShader.setUniform3f("uColor" + ofToString(i + 1), c.r / 255.0, c.g / 255.0, c.b / 255.0);
    }

    mBlobShader.setUniform1f("uRotation", mOrientation);
    //mBlobShader.setUniform1i("uShadowMode", 1);
    mBlobShader.setUniform1f("uStretch", mCurrentStretch);
    mBlobShader.setUniform1f("uHeadDistance", mCurrentLength);
    mBlobShader.setUniform1f("uHeadRadius", HEAD_RADIUS);
    mBlobShader.setUniform1f("uAnimParam", mAnimParam);
    //mMesh.draw();
    mBlobShader.setUniform1i("uShadowMode", 0);
    ofPushMatrix();
    ofTranslate(30,0);
    mMesh.draw();
    mBlobShader.end();
    ofSetColor(0, 50, 0);
    mBorderMesh.draw();
    ofPopMatrix();

    ofSetColor(255);
    ofPushMatrix();
    ofScale(entranceScale, entranceScale);
    ofRotate(90);
    mEntranceShader.begin();
    mEntranceShader.setUniform3f("uColor", entranceColor.r, entranceColor.g, entranceColor.b);
    mEntranceShader.setUniform1f("uDarken", 1);
    mFrontImage.draw(-mFrontImage.width / 2, -mFrontImage.height / 2);
    mEntranceShader.end();
    ofPopMatrix();
    
    ofPopMatrix();

    ofPopStyle();
}