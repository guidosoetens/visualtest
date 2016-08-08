#include "BGEntrance.h"

/*
BGEntrance::BGEntrance()
{
    
}
*/

BGEntrance::~BGEntrance() {

}

void BGEntrance::update(float dt) {
    
}

void BGEntrance::render() {
    
    ofMesh mesh;

    float offset = 50;

    for(int sideIdx=0; sideIdx<5; ++sideIdx) {

        float pitch = sideIdx / 5.0 * 2 * M_PI; 
        ofVec3f pitchVector = ofVec3f(cosf(pitch), 0, sinf(pitch));


    }


    for(int i=0; i<5; ++i) {

        float ang1 = i / 5.0 * 2.0 * M_PI;
        float ang2 = (i + 1) / 5.0 * 2.0 * M_PI;
        ofVec2f to1(cosf(ang1), sinf(ang1));
        ofVec2f to2(cosf(ang2), sinf(ang2));

        mesh.addVertex(ofVec3f(mPosition.x, mPosition.y, 0));
        mesh.addVertex(ofVec3f(mPosition.x + offset * to1.x, mPosition.y + offset * to1.y, 0));
        mesh.addVertex(ofVec3f(mPosition.x + offset * to2.x, mPosition.y + offset * to2.y, 0));

        mesh.addNormal(ofVec3f(0,0,1));
        mesh.addNormal(ofVec3f(to1.x,to1.y,0));
        mesh.addNormal(ofVec3f(to2.x,to2.y,0));

        mesh.addTexCoord(ofVec2f(0,0));
        mesh.addTexCoord(ofVec2f(-1,0));
        mesh.addTexCoord(ofVec2f(1,0));
    }

    mesh.draw();
    
}
