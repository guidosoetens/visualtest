#include "BGEye.h"
#include "BGResources.h"

BGEye::BGEye(ofVec2f pos, float orientation)
:   mPosition(pos)
,   mOrientation(orientation)
,   mAnimParam(0)
,   mContainerAnimParam(0)
{
    mVeinsImage.loadImage("spots.jpg");
    mContainerImage.loadImage("spots.jpg");

    mQuad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    for(int i=0; i<4; ++i) {
        ofVec2f coord(i % 2, i / 2);
        mQuad.addVertex(2 * (coord - ofVec2f(.5)));
        mQuad.addTexCoord(coord);
    }

    mAnimParam = ofRandom(1.0);


    mMesh.setMode(OF_PRIMITIVE_TRIANGLES);

    int sides = 8;
    int samples = 8;
    int strips = 4;
    float baseOffset = 1.0;
    float minOffset = 1.5;
    float maxOffset = 1.8;
    float partAngle = 2 * M_PI / (float)sides;

    int totalVertices = sides * strips * samples;

    for(int i=0; i<sides; ++i) {
        float texXBase = i / (float)sides;
        float baseAngle = (i + .5) * partAngle;
        for(int j=0; j<samples; ++j) {
            float t = j / (float)(samples - 1);
            float angle = baseAngle + t * partAngle;
            ofVec2f to(cosf(angle), sinf(angle));

            float boundaryAngle = t * M_PI;
            float boundaryOffset = minOffset + (maxOffset - minOffset) * sinf(boundaryAngle);
            ofVec2f boundaryNormal(1.5, -cosf(boundaryAngle));
            boundaryNormal.rotate(180 * baseAngle / M_PI);
            boundaryNormal.normalize();

            // float straight = .5 + .5 * cosf(t * 2 * M_PI);
            // boundaryNormal = (1 - straight) * boundaryNormal + straight * to;
            // boundaryNormal.normalize();
            //boundaryNormal = to;

            for(int s=0; s<strips; ++s) {
                float st = s / (float)(strips - 1);
                float offset = (1 - st) * baseOffset + st * boundaryOffset;
                ofVec2f p = offset * to;
                mMesh.addVertex(p);

                if(st < .5) {
                    //curve to center..
                    float stt = powf(st / .5, 1.0);
                    ofVec3f normal = (1 - stt) * ofVec3f(-to.x, -to.y, 1) + stt * ofVec3f(0,0,1);
                    //ofVec3f normal = (1 - stt) * ofVec3f(0,0,1) + stt * -to;
                    mMesh.addNormal(normal.normalize());
                }
                else {
                    //curve outwards:
                    float stt = powf((st - .5) / .5, 1.5);
                    ofVec3f normal = (1 - stt) * ofVec3f(0,0,1) + stt * ofVec3f(boundaryNormal.x, boundaryNormal.y, .5);
                    mMesh.addNormal(normal.normalize());
                }

                mMesh.addTexCoord(ofVec2f(texXBase + t / sides, st));
            }

            int idx = (i * samples + j) * strips;
            int prevIdx = idx - strips;
            if(prevIdx < 0)
                prevIdx += totalVertices;
            
            for(int s=1; s<strips; ++s) {
                mMesh.addTriangle(idx + s, idx + s - 1, prevIdx + s - 1);
                mMesh.addTriangle(idx + s, prevIdx + s, prevIdx + s - 1);
            }
        }
    }
}

BGEye::~BGEye() {

}

void 
BGEye::update(float dt) {
    mAnimParam = fmodf(mAnimParam + dt / 20.0, 1.0);
    mContainerAnimParam = fmodf(mContainerAnimParam + dt / 120.0, 1.0);
}

void 
BGEye::render(ofShader & mEyeShader, ofShader & mContainerShader) {

    ofVec3f lookDirection(0,0,1);
    float lookDir = 2 * mAnimParam * 2 * M_PI;
    float lookAngle = 50 * sinf(1 * mAnimParam * 2 * M_PI);
    lookDirection.rotate(lookAngle, ofVec3f(cosf(lookDir), sinf(lookDir), 0.0f));

    float radius = 40;

    ofPushStyle();
    ofPushMatrix();
    ofTranslate(mPosition);
    ofScale(radius, radius);

    ofSetColor(255,0,0);
    mContainerShader.begin();
    mContainerShader.setUniform1f("uAnimParam", mContainerAnimParam);
    mContainerShader.setUniform1i("uShadowMode", 2);
    mContainerShader.setUniformTexture("uTexture", mContainerImage.getTextureReference(), 0);
    mMesh.draw();
    mContainerShader.setUniform1i("uShadowMode", 1);
    mMesh.draw();
    mContainerShader.setUniform1i("uShadowMode", 0);
    mMesh.draw();
    mContainerShader.end();
    ofSetColor(255);


    // ofSetColor(0,0,0,100);
    // ofCircle(0,0,1.1);
    ofScale(1.01,1.01);
    ofSetColor(255);
    mEyeShader.begin();
    mEyeShader.setUniform3f("uLookVector", lookDirection.x, lookDirection.y, lookDirection.z);
    mEyeShader.setUniformTexture("uVeinsTexture", mVeinsImage.getTextureReference(), 0);
    mQuad.draw();
    mEyeShader.end();


    //mMesh.drawWireframe();

    // float containerScale = 4.1 / mContainerImage.width;
    // ofScale(containerScale, containerScale);
    // mEntranceShader.begin();
    // mEntranceShader.setUniform3f("uColor", .5, .3, .5);
    // mContainerImage.draw(-mContainerImage.width / 2, -mContainerImage.height / 2);
    // mEntranceShader.end();

    ofPopMatrix();
    ofPopStyle();
}