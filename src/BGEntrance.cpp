#include "BGEntrance.h"

BGEntrance::BGEntrance(ofVec2f pos, float orientation)
:   mPosition(pos)
,   mOrientation(orientation)
{
    ofVec2f topOffset(0, -40);
    ofVec2f topSize(80,20);
    ofVec2f midOffset(0, 0);
    ofVec2f midSize(100,25);
    ofVec2f btmOffset(0, 30);
    ofVec2f btmSize(100,30);

    float dAng = 2 * M_PI / 5.0;
    for(int sideIdx=0; sideIdx<5; ++sideIdx) {

        float topAng1 = -.5 * M_PI + (1 - sideIdx) * dAng;
        float topAng2 = topAng1 - dAng;
        float btmAng = .5 * M_PI + (-2 - sideIdx) * dAng;

        ofVec2f topVec1(cosf(topAng1), sinf(topAng1));
        ofVec2f topVec2(cosf(topAng2), sinf(topAng2));
        ofVec2f btmVec(cosf(btmAng), sinf(btmAng));

        ofVec2f pts[5] = {
            topOffset + topVec1 * topSize,
            topOffset + topVec2 * topSize,
            midOffset + topVec2 * midSize,
            btmOffset + btmVec * btmSize,
            midOffset + topVec1 * midSize,
        };

        //center normal?
        float upTilt = 0.3;
        ofVec3f centerNormal(btmVec.x, upTilt, btmVec.y);
        centerNormal.normalize();

        //calculate uv-coordinate frame:
        ofVec3f up(0,1,0);
        ofVec3f u = up.getCrossed(centerNormal);
        u.normalize();
        ofVec3f v = centerNormal.getCrossed(u);

        //get surface normals from coordinate frame:
        ofVec3f normals[5];
        for(int i=0; i<5; ++i) {
            float normalAngle = .5 * M_PI + (-2 - i) * dAng;
            normals[i] = (cosf(normalAngle) * u) + (sinf(normalAngle) * v);
        }

        //calc center:
        ofVec2f center(0,0);
        for(int i=0; i<5; ++i)
            center += pts[i];
        center /= 5.0;

        //add triangles:
        ofMesh mesh;
        for(int i=0; i<5; ++i) {

            int j = (i + 1) % 5;

            mesh.addVertex(ofVec3f(center.x, center.y, 0));
            mesh.addVertex(ofVec3f(pts[i].x, pts[i].y, 0));
            mesh.addVertex(ofVec3f(pts[j].x, pts[j].y, 0));

            mesh.addNormal(centerNormal);
            mesh.addNormal(normals[i]);
            mesh.addNormal(normals[j]);

            mesh.addTexCoord(ofVec2f(0,0));
            mesh.addTexCoord(ofVec2f(-1,0));
            mesh.addTexCoord(ofVec2f(1,0));
        }

        mMeshes.push_back(mesh);

    }
}

BGEntrance::~BGEntrance() {



}

ofVec2f BGEntrance::addVertex(ofMesh & mesh, float angle, ofVec2f size, float y) {

    ofVec2f vec(cosf(angle), sinf(angle));

    ofVec2f result;
    result.x = vec.x * size.x;
    result.y = y + vec.y * size.y;

    mesh.addVertex(ofVec3f(result.x, result.y, 0));
    mesh.addNormal(ofVec3f(vec.x, vec.y, 0));

    return result;
}

void BGEntrance::update(float dt) {
    
}

void BGEntrance::render(ofShader & mEntranceShader) {

    ofPushMatrix();
    ofTranslate(mPosition.x, mPosition.y);
    ofRotate(180 * mOrientation / M_PI);

    for(int i=0; i<mMeshes.size(); ++i) {
        ofVec3f faceNormal = mMeshes[i].getNormal(0);
        mEntranceShader.setUniform3f("uFaceNormal", faceNormal.x, faceNormal.y, faceNormal.z);
        mMeshes[i].draw();
    }

    ofPopMatrix();

    /*
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
    */
    
}
