#include "BGEntranceMesh.h"

#define NUM_TENTACLE_DIVS 5
#define NUM_TENTACLE_SAMPLES 8
#define NUM_CENTER_DIVS 3

BGEntranceMesh::BGEntranceMesh(ofVec2f position, float orientation) {
    mPosition = position + ofVec2f(0,200);
    mOrientation = orientation;

    mMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    

    //back tentacles:
    ofVec2f offset1(0.2, .05);
    ofVec2f offset2(0.23, 0.22);
    ofVec2f offset3(0.4, 0.215);

    int tentacleOffsets[4];

    //append tentacles:
    for(int i=0; i<2; ++i) {
        ofVec2f flipVector(i % 2 == 0 ? -1 : 1, 1);
        ofVec2f p0(.35, 0);
        ofVec2f p1 = flipVector * (p0 + offset1);
        ofVec2f p2 = flipVector * (p0 + offset2);
        ofVec2f p3 = flipVector * (p0 + offset3);
        p0 = flipVector * p0;
        tentacleOffsets[i] = mMesh.getVertices().size();
        pushTentacle(mMesh, p0, p1, p2, p3);
    }

    //front tentacles:
    offset1 = ofVec2f(0.16, .05);
    offset2 = ofVec2f(0.2, 0.16);
    offset3 = ofVec2f(0.35, 0.22);

    //append tentacles:
    for(int i=0; i<2; ++i) {
        ofVec2f flipVector(i % 2 == 0 ? -1 : 1, 1);
        ofVec2f p0(.16, .18);
        ofVec2f p1 = flipVector * (p0 + offset1);
        ofVec2f p2 = flipVector * (p0 + offset2);
        ofVec2f p3 = flipVector * (p0 + offset3);
        p0 = flipVector * p0;
        tentacleOffsets[2 + i] = mMesh.getVertices().size();
        pushTentacle(mMesh, p0, p1, p2, p3);
    }

    //find last index of back tentacles which is above the front tentacles:
    int numTopDivs = NUM_TENTACLE_DIVS;
    for(int i=0; i<NUM_TENTACLE_DIVS; ++i) {
        ofVec2f back = mMesh.getVertex(tentacleOffsets[0] + i);
        ofVec2f front = mMesh.getVertex(tentacleOffsets[2]);
        if(back.y >= front.y) {
            numTopDivs = i;
            break;
        }
    }

    //stitch tentacles:
    int numCenterDivs = numTopDivs + NUM_TENTACLE_DIVS;
    for(int i=0; i<numCenterDivs; ++i) {
        int idx = mMesh.getVertices().size();
        int idx1, idx2, prevIdx1, prevIdx2;

        if(i < numTopDivs) {
            //stick back tentacles:
            idx1 = tentacleOffsets[0] + i;
            idx2 = tentacleOffsets[1] + NUM_TENTACLE_DIVS - 1 - i;
            prevIdx1 = idx1 - 1;
            prevIdx2 = idx2 + 1;
        }
        else {
            //stitch front tentacles:
            int ii = i - numTopDivs;
            idx1 = tentacleOffsets[2] + ii;
            idx2 = tentacleOffsets[3] + NUM_TENTACLE_DIVS - 1 - ii;
            if(ii == 0) {
                //stitch to back tentacles:
                prevIdx1 = tentacleOffsets[0] + numTopDivs - 1;
                prevIdx2 = tentacleOffsets[1] + NUM_TENTACLE_DIVS - numTopDivs;
            }
            else {
                prevIdx1 = idx1 - 1;
                prevIdx2 = idx2 + 1;
            }
        }


        // int idx1 = tentacleOffsets[0] + i;
        // int idx2 = tentacleOffsets[1] + NUM_TENTACLE_DIVS - 1 - i;

        ofVec2f p1 = mMesh.getVertex(idx1);
        ofVec2f p2 = mMesh.getVertex(idx2);
        ofVec3f n1 = mMesh.getNormal(idx1);
        ofVec3f n2 = mMesh.getNormal(idx2);

        for(int j=0; j<NUM_CENTER_DIVS; ++j) {
            float t = (j + 1) / (float)(NUM_CENTER_DIVS + 1);
            ofVec2f p = (1 - t) * p1 + t * p2;
            mMesh.addVertex(p);

            ofVec3f n(0,0,1);
            if(t < .5) {
                float tt = t / .5;
                n = (1 - tt) * n1 + tt * n;
            }
            else {
                float tt = (t - .5) / .5;
                n = (1 - tt) * n + tt * n2;
            }

            mMesh.addNormal(n.normalize());

            //stitch center div:
            if(i > 0 && j > 0) {
                mMesh.addTriangle(idx + j, idx + j - 1, idx + j - NUM_CENTER_DIVS);
                mMesh.addTriangle(idx + j - NUM_CENTER_DIVS - 1, idx + j - 1, idx + j - NUM_CENTER_DIVS);
            }
        }

        //stitch center div to tentacles on sides:
        if(i > 0) {
            mMesh.addTriangle(prevIdx1, idx1, idx - NUM_CENTER_DIVS);
            mMesh.addTriangle(idx, idx1, idx - NUM_CENTER_DIVS);
            mMesh.addTriangle(idx + NUM_CENTER_DIVS - 1, idx - 1, prevIdx2);
            mMesh.addTriangle(idx + NUM_CENTER_DIVS - 1, idx2, prevIdx2);
        }
    }

    //add top circles (back):
    //TODO

    //add top circles (front):
    int colSamples = 6;
    int rowSamples = 5;
    int topCirclesOffset = mMesh.getVertices().size();
    for(int c=0; c<3; ++c) {
        for(int s=0; s<colSamples; ++s) {
            float t = s / (float)(colSamples - 1);
            float tTotal = (c + t) / 3.0;
            float totalReparam = -cosf(tTotal * M_PI);

            ofVec2f pivotPos(.32 * totalReparam, -0.45 + .03 * sinf(t * M_PI));
            float maxUpOffset = .1 + .15 * sinf(t * M_PI);
            float maxPerpOffset = .025 * totalReparam;

            float upMeshAngle = -.5 * M_PI + 0.6 * totalReparam;//+ 1.2 * (tTotal - .5);
            ofVec2f toTopVector(cosf(upMeshAngle), sinf(upMeshAngle));
            ofVec2f curveVector(-toTopVector.y, toTopVector.x);

            for(int i=0; i<rowSamples; ++i) {
                float rt = i / (float)(rowSamples - 1);

                int idx = mMesh.getVertices().size();

                float curveOffset = sinf(rt * M_PI) * maxPerpOffset;

                ofVec2f pos = pivotPos + rt * maxUpOffset * toTopVector + curveOffset * curveVector;

                // float y = -.4 - rt * height;
                // float x = mid_x + curveOffset * sinf(rt * M_PI);

                mMesh.addVertex(pos);
                mMesh.addNormal(ofVec3f(0,0,1));

                if((c > 0 || s > 0) && i > 0) {
                    //stitch backwards:
                    mMesh.addTriangle(idx, idx - 1, idx - rowSamples);
                    mMesh.addTriangle(idx - rowSamples - 1, idx - 1, idx - rowSamples);
                }
            }
        }
    }

    //add face mesh:
    int layers = 4;
    int divs = NUM_CENTER_DIVS + 2;
    float height = 0.22;
    ofVec2f pBottomLeft = mMesh.getVertex(tentacleOffsets[0]);
    ofVec2f pBottomRight = mMesh.getVertex(tentacleOffsets[1] + NUM_TENTACLE_DIVS - 1);
    ofVec2f pTopLeft = mMesh.getVertex(topCirclesOffset);
    ofVec2f pTopRight = mMesh.getVertex(mMesh.getVertices().size() - rowSamples);
    float bottomWidth = pBottomRight.x - pBottomLeft.x;
    float topWidth = pTopRight.x - pTopLeft.x;
    height = ABS(pTopLeft.y - pBottomLeft.y);

    for(int i=0; i<layers; ++i) {
        float tLayer = (i + 1) / (float)(layers + 1);
        float y = pBottomLeft.y - height * tLayer;
        float width_t = tLayer * tLayer;
        float width = (1 - width_t) * bottomWidth + width_t * topWidth; //baseWidth * (.7 + .3 * cosf(tLayer * .5 * M_PI));
        float startX = pBottomLeft.x + (bottomWidth - width) / 2.0;
        int idx = mMesh.getVertices().size();
        for(int j=0; j<divs; ++j) {


            float t = j / (float)(divs - 1);
            float x = startX + t * width;

            mMesh.addVertex(ofVec2f(x, y));
            mMesh.addNormal(ofVec3f(0,0,1));

            if(i > 0 && j > 0) {
                mMesh.addTriangle(idx + j, idx + j - 1, idx + j - divs);
                mMesh.addTriangle(idx + j - divs - 1, idx + j - 1, idx + j - divs);
            }
        
        }
    }

    //stitch face mesh to bottom:

}

void BGEntranceMesh::sampleSpline(ofVec2f p0, ofVec2f p1, ofVec2f p2, ofVec2f p3, float t, ofVec2f & p, ofVec2f & n) {
    float min_t = 1 - t;
    float tt = t * t;
    float min_tt = min_t * min_t;

    p = min_t * min_tt * p0 + 3 * min_tt * t * p1 + 3 * min_t * tt * p2 + t * tt * p3;
    n = 3 * min_tt * (p1 - p0) + 6 * min_t * t * (p2 - p1) + 3 * tt * (p3 - p2);
    n.normalize();
    n = ofVec2f(-n.y, n.x);
}

int BGEntranceMesh::sign(float f) {
    if(f < 0)
        return -1;
    else if(f > 0)
        return 1;
    return 0;
}


void BGEntranceMesh::pushTentacle(ofMesh& mesh, ofVec2f p0, ofVec2f p1, ofVec2f p2, ofVec2f p3) {

    int samples = NUM_TENTACLE_SAMPLES;//8;
    int bands = NUM_TENTACLE_DIVS;//8;
    for(int i=0; i<samples; ++i) {
        float t = i / (float)(samples - 1);
        ofVec2f p, n;
        sampleSpline(p0, p1, p2, p3, t, p, n);
        ofVec2f tang(-n.y, n.x);

        float offset1 = -(1 - t) * 0.8 * sinf(t * t * M_PI);
        float offset2 = (1 - t) * 0.8 * sinf(t * M_PI);

        float radius = (.3 + .7 * cosf(t * .5 * M_PI)) * .2;
        float circleOffset = (.5 - .7 * t) * .3;

        for(int j=0; j<bands; ++j) {
            t = j / (float)(bands - 1);
            // float offset = 0;
            // if(t < .5)
            //     offset = t / .5 * offset1;
            // else
            //     offset = (t - .5) / .5 * offset2;

            // ofVec2f pos = p + n * offset;

            float curve = sqrt(ofClamp(1 - powf(2 * t - 1, 2.0), 0, 1));//sinf(t * M_PI);
            float normalOffset = (1 - 2 * t);


            ofVec2f pos = p + n * normalOffset * radius + (curve * circleOffset) * tang;

            //float normalFactor = powf(abs(normalOffset), 2.0);
            normalOffset = sign(normalOffset) * powf(abs(normalOffset), 0.8);
            ofVec2f normal = n * normalOffset;
            float z = sqrt(1 - normalOffset * normalOffset);


            mesh.addVertex(pos);
            mesh.addNormal(ofVec3f(normal.x, normal.y, z));

            if(i > 0 && j > 0) {
                int idx = mesh.getVertices().size() - 1;
                mesh.addTriangle(idx, idx - 1, idx - bands);
                mesh.addTriangle(idx - bands - 1, idx - 1, idx - bands);
            }
        }
    }
}

BGEntranceMesh::~BGEntranceMesh() {
    
}

void BGEntranceMesh::render(ofShader & mEntranceMeshShader) {

    float scale = 300;

    ofPushMatrix();
    ofTranslate(mPosition);
    ofRotate(180 * mOrientation / M_PI);
    ofScale(scale, scale, scale);
    mEntranceMeshShader.begin();
    mEntranceMeshShader.setUniform1i("uShadowMode", 1);
    mMesh.draw();
    mEntranceMeshShader.setUniform1i("uShadowMode", 0);
    mMesh.draw();
    mEntranceMeshShader.end();
    ofSetColor(255, 255, 255, 20);
    mMesh.drawWireframe();
    
    ofPopMatrix();
}

void BGEntranceMesh::update(float dt) {
    
}