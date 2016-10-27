#include "BGAntenna.h"

static int rootcounter = 0;

BGAntenna::BGAntenna(ofVec2f pos, float angle)
:   mPosition(pos)
,   mAngle(angle)
,   mTimeParameter(0)
{
    mRandomRoot = rootcounter++;
}

BGAntenna::~BGAntenna() {

}

void 
BGAntenna::render() {

    ofSetColor(255);

    ofPushMatrix();
    ofTranslate(mPosition.x, mPosition.y);
    ofRotate(180 * mAngle / M_PI + 90);


    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    for(int idx=0; idx<5; ++idx) {

        ofVec2f src((idx - 2) * 10, 0);
        ofVec2f pt = src;
        ofVec2f normal(0, 1);

        int samples = 50;
        for(int i=0; i<samples; ++i) {
            
            float t = i / (float)(samples - 1);
            float x = 20 * sin((t + mTimeParameter) * 2 * M_PI) * t;
            float y = -t * 200;
            float width = 5;
            float halfWidth = width / 2.0;

            mesh.addVertex(ofVec3f(x - halfWidth, y, 0));
            mesh.addVertex(ofVec3f(x + halfWidth, y, 0));

            pt += normal;
        }
    }

    mesh.draw();

    ofPopMatrix();
}

void 
BGAntenna::update(float dt) {
    mTimeParameter = fmodf(mTimeParameter + dt / 2.0, 1.0);
}