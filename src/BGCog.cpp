#include "BGCog.h"
#import "BGResources.h"


BGCog::BGCog(ofVec2f pos, float _angle) {

    mPosition = pos;
    mOrientation = _angle;
    mTimeAnimParam = 0;
    mCellsAnimParam = 0;

    mBumpImage.loadImage("bump.jpg");

    mCenterMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    mCenterMesh.addVertex(ofVec2f(0,0));
    mCenterMesh.addNormal(ofVec3f(0,0,1));

    mTeethMesh.setMode(OF_PRIMITIVE_TRIANGLES);

    int reps = 15;
    float a1 = 2 * M_PI / reps * .15;
    float a2 = 2 * M_PI / reps * .3;
    float r1 = 130;
    float r2 = 150;

    float angOffsets[4] = { 0, a1, a1 + a2, 2 * a1 + a2 };
    float radii[4] = { r1, r2, r2, r1 };

    int teethVertices = 16 * reps;

    for(int i=0; i<reps; ++i) {
        float ang = i / (float)reps * 2 * M_PI;
        for(int j=0; j<4; ++j) {
            float calcAng = ang + angOffsets[j];
            ofVec2f to = ofVec2f(cosf(calcAng),sinf(calcAng));
            ofVec2f p1 = radii[j] * to;
            ofVec2f p2 = (radii[j] + 10) * to;
            mCenterMesh.addVertex(p1);
            mCenterMesh.addNormal(ofVec3f(0,0,1));

            mTeethMesh.addVertex(p1);
            mTeethMesh.addVertex(p2);
            mTeethMesh.addVertex(p1);
            mTeethMesh.addVertex(p2);

            mTeethMesh.addNormal(ofVec3f(0,0,1));
            mTeethMesh.addNormal(to);
            mTeethMesh.addNormal(ofVec3f(0,0,1));
            mTeethMesh.addNormal(to);

            int idx = 16 * i + 4 * j;
            int next_idx = (16 * i + 4 * (j + 1)) % teethVertices;
            mTeethMesh.addTriangle(idx + 2, idx + 3, next_idx);
            mTeethMesh.addTriangle(idx + 3, next_idx, next_idx + 1);
        }
    }

    //calculate normals:
    for(int i=0; i<reps; ++i) {

        for(int j=0; j<2; ++j) {
            int idx1 = i * 16 + j * 8 + 3;
            int idx2 = i * 16 + j * 8 + 5;

            ofVec2f p1 = mTeethMesh.getVertex(idx1);
            ofVec2f p2 = mTeethMesh.getVertex(idx2);
            ofVec2f to = p2 - p1;
            ofVec2f n = ofVec2f(to.y, -to.x);
            n = n.normalize();
            mTeethMesh.setNormal(idx1, n);
            mTeethMesh.setNormal(idx2, n);
        }
    }


    mCenterMesh.addVertex(mCenterMesh.getVertex(1));
    mCenterMesh.addNormal(ofVec3f(0,0,1));
}

BGCog::~BGCog() {
    
}

void BGCog::render(ofShader & mCogShader) {

    float totalRotation = mOrientation + mTimeAnimParam * 2 * M_PI;
    
    mCogShader.begin();
    mCogShader.setUniform1f("uRotation", totalRotation);
    mCogShader.setUniformTexture("uBumpMap", mBumpImage.getTextureReference(), 0);
    mCogShader.setUniformTexture("uCellsTexture", bgResources.getImageReference(ObstacleImage2Key)->getTextureReference(), 1);
    mCogShader.setUniform1f("uRadius", 150);
    mCogShader.setUniform1f("uTime", mCellsAnimParam);
    ofPushMatrix();
    ofTranslate(mPosition);
    ofRotate(180 * totalRotation / M_PI);
    mCenterMesh.draw();
    mTeethMesh.draw();
    ofPopMatrix();
    mCogShader.end();
}

void BGCog::update(float dt) {
    mTimeAnimParam = fmodf(mTimeAnimParam + dt / 30.0, 1.0);
    mCellsAnimParam = fmodf(mCellsAnimParam + dt / 50.0, 1.0);
}