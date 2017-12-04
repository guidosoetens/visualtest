#include "BGEntranceMesh.h"

#define NUM_TENTACLE_DIVS 20/*5*/
#define NUM_TENTACLE_SAMPLES 15/*8*/
#define NUM_CENTER_DIVS 3/*3*/
#define FACE_MESH_LAYERS 5/*2*/
#define TOP_COL_SAMPLES 15/*5*/
#define TOP_ROW_SAMPLES 8/*3*/

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
        ofVec2f p0(.18, .18);
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

    mergeVertexInto(mMesh, tentacleOffsets[2], tentacleOffsets[0] + numTopDivs - 1);
    mergeVertexInto(mMesh, tentacleOffsets[3] + NUM_TENTACLE_DIVS - 1, tentacleOffsets[1] + numTopDivs - 1);
    //mMesh.setVertex(tentacleOffsets[3] + NUM_TENTACLE_DIVS - 1,  mMesh.getVertex(tentacleOffsets[3] + NUM_TENTACLE_DIVS - 1) * 2.0);

    //flatten normals of sides:
    for(int i=0; i<2; ++i) {
        int idx = i == 0 ? tentacleOffsets[0] : (tentacleOffsets[1] + NUM_TENTACLE_DIVS - 1);
        ofVec3f n = mMesh.getNormal(idx);
        ofVec3f to(i ==0 ? -1 : 1, 0, 0);
        n = .5 * to + .5 * n;
        mMesh.setNormal(idx, n.normalize());
    }

    //push merged vertex upwards:
    int sideIndices[2] = { tentacleOffsets[0] + numTopDivs - 1, tentacleOffsets[1] + numTopDivs - 1 };
    for(int i=0; i<2; ++i) {
        int idx = sideIndices[i];
        ofVec3f n = mMesh.getNormal(idx);
        n.y += -.7;
        mMesh.setNormal(idx, n.normalize());
    }

    //stitch tentacles:
    int numCenterDivs = numTopDivs + NUM_TENTACLE_DIVS - 1;
    int btmCenterDivsOffset = mMesh.getVertices().size();
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
            idx1 = tentacleOffsets[2] + ii + 1;
            idx2 = tentacleOffsets[3] + NUM_TENTACLE_DIVS - 1 - ii - 1;
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
            if(i == numCenterDivs - 1)
                p.y -= 0.01;

            mMesh.addVertex(p);

            ofVec3f n(0,0,1);
            n = (1 - t) * n1 + t * n2;
            n.z = 0;
            float len = fminf(1, n.length());
            n.z = sqrt(1 - len * len);

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
    int colSamples = TOP_COL_SAMPLES;//5;
    int rowSamples = TOP_ROW_SAMPLES;//3;
    int topCirclesOffset = mMesh.getVertices().size();
    for(int c=0; c<3; ++c) {
        for(int s=0; s<colSamples; ++s) {
            float t = s / (float)(colSamples - 1);
            float tTotal = (c + t) / 3.0;
            float totalReparam = -cosf(tTotal * M_PI);

            float sideEffect = (.5 + .5 * cosf(tTotal * 2 * M_PI));
            ofVec2f pivotPos(.32 * totalReparam, -0.35 + .03 * sinf(t * M_PI) * (1 - sideEffect) + .02 * sideEffect);
            float maxUpOffset = .1 + .15 * sinf(t * M_PI)  * (1 - .5 * sideEffect) + .05 * sideEffect;
            float maxPerpOffset = .01 * totalReparam;

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


                ofVec3f n(0,0,1);
                float tNormal = t;
                if(c == 0)
                    tNormal = .5 * t;
                else if(c == 1)
                    tNormal = .2 + .6 * t;
                else    
                    tNormal = .5 + .5 * t;

                n = (1 - tNormal) * ofVec3f(-1,0,0) + tNormal * ofVec3f(1,0,0);
                float len = n.length();
                n.z = sqrtf(1 - len * len);

                //top samples:
                if(i == rowSamples - 1) {
                    //float nAngle = (-.8 + .6 * t) * M_PI;
                    float minAng = 0, maxAng = 0;
                    if(c == 0) {
                        minAng = -M_PI;
                        maxAng = -.2 * M_PI;
                    }
                    else if(c == 1) {
                        minAng = -.8 * M_PI;
                        maxAng = -.2 * M_PI;
                    }
                    else {
                        minAng = -.8 * M_PI;
                        maxAng = 0;
                    }
                    float nAngle = (1 - t) * minAng + t * maxAng;
                    n = ofVec3f(cosf(nAngle), sinf(nAngle), 0);
                }

                mMesh.addVertex(pos);
                mMesh.addNormal(n);

                if((c > 0 || s > 0) && i > 0) {
                    //stitch backwards:
                    mMesh.addTriangle(idx, idx - 1, idx - rowSamples);
                    mMesh.addTriangle(idx - rowSamples - 1, idx - 1, idx - rowSamples);
                }
            }
        }
    }

    //add face mesh:
    int layers = FACE_MESH_LAYERS;
    int divs = NUM_CENTER_DIVS + 2;
    int topDivs = 3 * (colSamples - 1) + 1;
    float height = 0.22;
    ofVec2f pBottomLeft = mMesh.getVertex(tentacleOffsets[0]);
    ofVec2f pBottomRight = mMesh.getVertex(tentacleOffsets[1] + NUM_TENTACLE_DIVS - 1);
    ofVec2f pTopLeft = mMesh.getVertex(topCirclesOffset);
    ofVec2f pTopRight = mMesh.getVertex(mMesh.getVertices().size() - rowSamples);
    float bottomWidth = pBottomRight.x - pBottomLeft.x;
    float topWidth = pTopRight.x - pTopLeft.x;
    height = ABS(pTopLeft.y - pBottomLeft.y);
    int faceMeshOffset = mMesh.getVertices().size();

    for(int i=0; i<layers; ++i) {
        float tLayer = (i + 1) / (float)(layers + 1);
        float y = pBottomLeft.y - height * tLayer;
        float width_t = tLayer * tLayer;
        float width = (1 - width_t) * bottomWidth + width_t * topWidth; //baseWidth * (.7 + .3 * cosf(tLayer * .5 * M_PI));
        float startX = pBottomLeft.x + (bottomWidth - width) / 2.0;
        int idx = mMesh.getVertices().size();

        bool isTop = i == layers - 1;
        int trackedPrevIdx = idx - divs;
        int currDivs = isTop ? topDivs : divs;

        float normalAngle = (-.9 + tLayer * .15) * M_PI;
        ofVec2f sideNormal(cosf(normalAngle), sinf(normalAngle));

        for(int j=0; j<currDivs; ++j) {

            float t = j / (float)(currDivs - 1);
            if(isTop) {
                t = .5 - .5 * cosf(t * M_PI);
            }

            float x = startX + t * width;

            ofVec2f normal = (1 - t) * sideNormal + t * ofVec2f(-sideNormal.x, sideNormal.y);
            float len = fminf(1, normal.length());
            float z = sqrt(1 - len * len);

            mMesh.addVertex(ofVec2f(x, y));
            mMesh.addNormal(ofVec3f(normal.x, normal.y, z));

            if(i > 0 && j > 0) {
                if(isTop) {

                    int prevBaseIdx = idx - divs;
                    int prevIdx = prevBaseIdx + min(divs - 1, (int)floor(t * divs));
                    mMesh.addTriangle(idx + j, idx + j - 1, prevIdx);

                    if(prevIdx != trackedPrevIdx) {
                        mMesh.addTriangle(idx + j - 1, trackedPrevIdx, prevIdx);
                        trackedPrevIdx = prevIdx;
                    }

                }
                else {
                    mMesh.addTriangle(idx + j, idx + j - 1, idx + j - divs);
                    mMesh.addTriangle(idx + j - divs - 1, idx + j - 1, idx + j - divs);
                }
            }
        
        }
    }

    int faceMeshTopBaseIndex = mMesh.getVertices().size() - topDivs;

    //stitch face mesh to bottom:
    for(int i=1; i<divs; ++i) {

        int idx1 = (i == 1) ? tentacleOffsets[0] : (btmCenterDivsOffset + i - 2);
        int idx2 = (i == divs - 1) ? tentacleOffsets[1] + NUM_TENTACLE_DIVS - 1 : (btmCenterDivsOffset + i - 1);
        mMesh.addTriangle(faceMeshOffset + i, faceMeshOffset + i - 1, idx2);
        mMesh.addTriangle(faceMeshOffset + i - 1, idx1, idx2);
    }

    //stitch face mesh to top:
    for(int i=1; i<topDivs; ++i) {
        int prev_i = i - 1;
        int idxTop = topCirclesOffset + (i + i / (rowSamples + 2)) * rowSamples;
        int prevIdxTop = topCirclesOffset + (prev_i + prev_i / (rowSamples + 2)) * rowSamples;
        int idxFace = faceMeshTopBaseIndex + i;

        mMesh.addTriangle(idxFace, idxFace - 1, prevIdxTop);
        mMesh.addTriangle(idxFace, idxTop, prevIdxTop);
    }

}

void BGEntranceMesh::mergeVertexInto(ofMesh& mesh, int src, int dst) {
    for(int i=0; i<mesh.getIndices().size(); ++i) {
        if(mesh.getIndex(i) == src)
            mesh.setIndex(i, dst);
    }
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
            float tBand = j / (float)(bands - 1);
            // float offset = 0;
            // if(t < .5)
            //     offset = t / .5 * offset1;
            // else
            //     offset = (t - .5) / .5 * offset2;

            // ofVec2f pos = p + n * offset;

            float curve = sqrt(ofClamp(1 - powf(2 * tBand - 1, 2.0), 0, 1));//sinf(tBand * M_PI);
            float normalOffset = (1 - 2 * tBand);


            ofVec2f pos = p + n * normalOffset * radius + (curve * circleOffset) * tang;

            //float normalFactor = powf(abs(normalOffset), 2.0);
            normalOffset = sign(normalOffset) * powf(abs(normalOffset), 0.8);
            ofVec3f normal(n.x * normalOffset, n.y * normalOffset, sqrt(1 - normalOffset * normalOffset));

            //flatten  normals by default:
            float defaultFlatten = .2 + .4 * t;
            normal = (1 - defaultFlatten) * normal + defaultFlatten * ofVec3f(-tang.x, -tang.y, 0);

            //flatten normal (towards tip):
            if(t > .5) {
                float flatten = (t - .5) / .5;
                float centerFactor = .5 - .5 * cosf(tBand * 2 * M_PI);
                ofVec3f flatNormal = (1 - centerFactor) * normal + centerFactor * ofVec3f(-tang.x, -tang.y, 0);
                normal = (1 - flatten) * normal + flatten * flatNormal;
            }

            normal.normalize();

            mesh.addVertex(pos);
            mesh.addNormal(normal);

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

    float scale = 600;//300;

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
    // ofSetColor(255, 255, 255, 20);
    // mMesh.drawWireframe();

    // //draw normals:
    // int n = mMesh.getNormals().size();
    // ofSetColor(255);
    // for(int i=0; i<n; ++i) {
    //     ofVec2f normal = mMesh.getNormal(i);
    //     ofVec2f pos = mMesh.getVertex(i);
    //     ofCircle(pos, .003);
    //     ofLine(pos, pos + .1 * normal);
    // }
    
    ofPopMatrix();
}

void BGEntranceMesh::update(float dt) {
    
}