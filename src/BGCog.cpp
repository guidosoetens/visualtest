#include "BGCog.h"

BGCog::BGCog(ofVec2f pos, float _angle) {

    mPosition = pos;
    mOrientation = _angle;

    mMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    mMesh.addVertex(ofVec2f(0,0));

    int reps = 10.0;
    float a1 = 2 * M_PI / reps * .15;
    float a2 = 2 * M_PI / reps * .3;
    float r1 = 100;
    float r2 = 120;

    for(int i=0; i<reps; ++i) {
        float ang = i / 10.0 * 2 * M_PI;
        mMesh.addVertex(r1 * ofVec2f(cosf(ang),sinf(ang)));
        mMesh.addVertex(r2 * ofVec2f(cosf(ang + a1),sinf(ang + a1)));
        mMesh.addVertex(r2 * ofVec2f(cosf(ang + a1 + a2),sinf(ang + a1 + a2)));
        mMesh.addVertex(r1 * ofVec2f(cosf(ang + 2 * a1 + a2),sinf(ang + 2 * a1 + a2)));
    }

    mMesh.addVertex(mMesh.getVertex(1));
}

BGCog::~BGCog() {
    
}

void BGCog::render(ofShader & mCogShader) {
    
    ofPushMatrix();
    ofTranslate(mPosition);
    ofRotate(180 * mOrientation / M_PI);
    mMesh.draw();
    ofPopMatrix();
}

void BGCog::update(float dt) {
    
}