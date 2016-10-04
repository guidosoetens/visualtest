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

    float scale = 0.8;
    topOffset *= scale;
    topSize *= scale;
    midOffset *= scale;
    midSize *= scale;
    btmOffset *= scale;
    btmSize *= scale;

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

        if(sideIdx < 2)
            mBackMeshes.push_back(mesh);
        else
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

void 
BGEntrance::update(float dt) {
    
}

void 
BGEntrance::render(ofShader & mEntranceShader) {
    renderMeshes(mEntranceShader, mMeshes);
}

 void 
 BGEntrance::renderBack(ofShader & mEntranceShader) {
    renderMeshes(mEntranceShader, mBackMeshes);
 }

 void 
 BGEntrance::renderMeshes(ofShader & mEntranceShader, vector<ofMesh> & meshes) {
    mEntranceShader.begin();

    ofPushMatrix();
    ofTranslate(mPosition.x, mPosition.y);
    ofRotate(180 * mOrientation / M_PI);

    for(int i=0; i<meshes.size(); ++i) {
        ofVec3f faceNormal = meshes[i].getNormal(0);
        mEntranceShader.setUniform3f("uFaceNormal", faceNormal.x, faceNormal.y, faceNormal.z);
        meshes[i].draw();
    }

    ofPopMatrix();

    mEntranceShader.end();
 }
