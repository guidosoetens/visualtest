#include "BGInternalNode.h"

BGInternalNode::BGInternalNode()
{
    for(int i=0; i<3; ++i)
        neighbours[i] = NULL;
}

BGInternalNode::~BGInternalNode() {

}

void BGInternalNode::setNeighbours(BGNode * n1, BGNode * n2, BGNode * n3) {
    neighbours[0] = n1;
    neighbours[1] = n2;
    neighbours[2] = n3;
}

void BGInternalNode::update(float dt) {

    ofVec2f sumLoc(0,0);
    for(int i=0; i<3; ++i) {
        sumLoc += neighbours[i]->position;
    }
    ofVec2f meanLoc = sumLoc / 3.0;
    position = .9 * position + .1 * meanLoc;
}

void BGInternalNode::renderGradientBetween(ofVec2f p1, ofVec2f p2) {

    const int circleSamples = 10;

    ofColor white(255,255,255,255);
    ofColor black(255,255,255,0);

    ofVec2f to = (p2 - p1).normalize();
    ofVec2f perp(to.y, -to.x);

    float radius = 80.0;

    ofMesh mesh;

    //center of circle:
    mesh.addVertex(ofVec3f(p1.x, p1.y, 0));
    mesh.addColor(white);

    //first sample of circle:
    mesh.addVertex(ofVec3f(p1.x + radius * perp.x, p1.y + radius * perp.y, 0));
    mesh.addColor(black);

    //traverse first circle and knit points (indices):
    for(int i=0; i<circleSamples; ++i) {

        float angle = (i + 1) / (float)circleSamples * M_PI;
        float cosAng = cosf(angle);
        float sinAng = sinf(angle);

        ofVec2f toPt = ofVec2f(cosAng * perp.x + sinAng * perp.y, cosAng * perp.y - sinAng * perp.x);
        mesh.addVertex(ofVec3f(p1.x + radius * toPt.x, p1.y + radius * toPt.y, 0));
        mesh.addColor(black);

        mesh.addIndex(0);
        mesh.addIndex(1 + i);
        mesh.addIndex(2 + i);
    }

    //center of other circle:
    mesh.addVertex(ofVec3f(p2.x, p2.y, 0));
    mesh.addColor(white);

    //first sample of other circle:
    perp = -perp;
    mesh.addVertex(ofVec3f(p2.x + radius * perp.x, p2.y + radius * perp.y, 0));
    mesh.addColor(black);

    int secondCenterOffset = 2 + circleSamples;

    //traverse second circle:
    for(int i=0; i<circleSamples; ++i) {

        float angle = (i + 1) / (float)circleSamples * M_PI;
        float cosAng = cosf(angle);
        float sinAng = sinf(angle);

        ofVec2f toPt = ofVec2f(cosAng * perp.x + sinAng * perp.y, cosAng * perp.y - sinAng * perp.x);
        mesh.addVertex(ofVec3f(p2.x + radius * toPt.x, p2.y + radius * toPt.y, 0));
        mesh.addColor(black);

        mesh.addIndex(secondCenterOffset);
        mesh.addIndex(secondCenterOffset + 1 + i);
        mesh.addIndex(secondCenterOffset + 2 + i);
    }

    int lastIdx = 3 + 2 * circleSamples;

    //BAR:
    mesh.addIndex(0);
    mesh.addIndex(1);
    mesh.addIndex(lastIdx);

    mesh.addIndex(0);
    mesh.addIndex(secondCenterOffset);
    mesh.addIndex(lastIdx);

    mesh.addIndex(0);
    mesh.addIndex(secondCenterOffset - 1);
    mesh.addIndex(secondCenterOffset + 1);

    mesh.addIndex(0);
    mesh.addIndex(secondCenterOffset);
    mesh.addIndex(secondCenterOffset + 1);


    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    mesh.draw();


}

void BGInternalNode::renderGradient() {

    ofSetColor(0, 50, 0, 255);
    for(int i=0; i<3; ++i) {
        if(neighbours[i]->traversalId == 0)
            renderGradientBetween(position, neighbours[i]->position);
    }

    traversalId = 1;
}

void BGInternalNode::render() {
    ofSetColor(0, 50, 0, 255);
    //ofCircle(position, 10);

    for(int i=0; i<3; ++i) {
        ofLine(position, neighbours[i]->position);
    }
}

void BGInternalNode::pushToMesh(ofMesh & mesh) {

    float halfEdgeWidth = .5 * EDGE_WIDTH;

    ofVec2f p = position;

    int centerOffset = mesh.getVertices().size();
    mesh.addVertex(ofVec3f(p.x, p.y, 1));
    mesh.addNormal(ofVec3f(0, 0, 1));

    int indices[3] = {-1, -1, -1};
    float angles[3] = {10000.0, 10000.0, 10000.0};
    for(int i=0; i<3; ++i) {

        ofVec2f pos = neighbours[i]->position;
        float angle = atan2f(pos.y - position.y, pos.x - position.x);
        if(angle < 0)
            angle += 2 * M_PI;

        for(int idx=0; idx<3; ++idx) {

            if(angle < angles[idx]) {

                //insert:
                for(int j=2; j>idx; --j) {
                    angles[j] = angles[j - 1];
                    indices[j] = indices[j - 1];
                }

                angles[idx] = angle;
                indices[idx] = i;

                break;
            }
        }
    }

    ofVec2f avgCenter(0, 0);

    int samples = 5;
    for(int i=0; i<3; ++i) {

        int j = (i + 1) % 3;
        ofVec2f e1 = (position + neighbours[indices[i]]->position) / 2.0;
        ofVec2f e2 = (position + neighbours[indices[j]]->position) / 2.0;
        ofVec2f to1 = (e1 - position);
        ofVec2f to2 = (e2 - position);
        float toDist1 = to1.length();
        float toDist2 = to2.length();
        to1 /= toDist1;
        to2 /= toDist2;

        for(int s=0; s<samples; ++s) {
            float t = s / (float)(samples - 1);
            float coeff1 = (1 - t) * (1 - t);
            float coeff2 = 2 * (1 - t) * t;
            float coeff3 = t * t;

            ofVec2f pt = coeff1 * e1 + coeff2 * p + coeff3 * e2;
            ofVec2f deriv = 2 * (1 - t) * (p - e1) + 2 * t * (e2 - p);
            float dLength = deriv.length();
            ofVec2f normal = ofVec2f(deriv.y / dLength, -deriv.x / dLength);

            int currOffset = centerOffset + 1 + 2 * (i * samples + s);// mesh.getVertices().size();
            //add spline samples:
            mesh.addVertex(ofVec3f(pt.x, pt.y, 1));
            mesh.addNormal(ofVec3f(0, 0, 1));
            mesh.addVertex(ofVec3f(pt.x + halfEdgeWidth * normal.x, pt.y + halfEdgeWidth * normal.y, 0));
            mesh.addNormal(ofVec3f(normal.x, normal.y, 0));

            if(s > 0) {
                //mesh.addTriangle(centerOffset, currOffset - 2, currOffset);
                mesh.addTriangle(currOffset, currOffset - 1, currOffset - 2);
                mesh.addTriangle(currOffset, currOffset - 1, currOffset + 1);
            }

        }

        avgCenter += .25 * e1 + .5 * p + .25 * e2;
    }

    avgCenter /= 3;
    mesh.setVertex(centerOffset, ofVec3f(avgCenter.x, avgCenter.y, 1));
}

/*
void BGInternalNode::pushToMeshOLD(ofMesh & mesh) {

    float halfEdgeWidth = .5 * EDGE_WIDTH;

    ofVec2f p = position;

    int centerOffset = mesh.getVertices().size();
    mesh.addVertex(ofVec3f(p.x, p.y, 1));
    mesh.addNormal(ofVec3f(0, 0, 1));

    int samples = 31;
    float coeff[samples][4];
    float dCoeff[samples][3];
    for(int i=0; i<samples; ++i) {
        float t = i / (float)(samples - 1);
        float tt = t * t;
        float min_t = 1 - t;
        float min_tt = min_t * min_t;

        coeff[i][0] = min_t * min_tt;
        coeff[i][1] = 3 * min_tt * t;
        coeff[i][2] = 3 * min_t * tt;
        coeff[i][3] = t * tt;

        dCoeff[i][0] = 3 * min_tt;
        dCoeff[i][1] = 6 * min_t * t;
        dCoeff[i][2] = 3 * tt;
    }

    int indices[3] = {-1, -1, -1};
    float angles[3] = {10000.0, 10000.0, 10000.0};
    for(int i=0; i<3; ++i) {

        ofVec2f pos = neighbours[i]->position;
        float angle = atan2f(pos.y - position.y, pos.x - position.x);
        if(angle < 0)
            angle += 2 * M_PI;

        for(int idx=0; idx<3; ++idx) {

            if(angle < angles[idx]) {

                //insert:
                for(int j=2; j>idx; --j) {
                    angles[j] = angles[j - 1];
                    indices[j] = indices[j - 1];
                }

                angles[idx] = angle;
                indices[idx] = i;

                break;
            }
        }
    }

    for(int i=0; i<3; ++i) {

        int j = (i + 1) % 3;
        ofVec2f e1 = (position + neighbours[indices[i]]->position) / 2.0;
        ofVec2f e2 = (position + neighbours[indices[j]]->position) / 2.0;
        ofVec2f to1 = (e1 - position);
        ofVec2f to2 = (e2 - position);
        float toDist1 = to1.length();
        float toDist2 = to2.length();
        to1 /= toDist1;
        to2 /= toDist2;

        int offset = mesh.getVertices().size();

        /*
        //add edge point:
        mesh.addVertex(ofVec3f(e1.x, e1.y, 1));
        mesh.addNormal(ofVec3f(0, 0, 1));
        * /

        ofVec2f a1 = e1 + halfEdgeWidth * ofVec2f(-to1.y, to1.x);
        ofVec2f a2 = e2 + halfEdgeWidth * ofVec2f(to2.y, -to2.x);

        float anchorDist = (a2 - a1).length();

        ofVec2f c1 = a1 - to1 * .5 * min(toDist1, anchorDist);
        ofVec2f c2 = a2 - to2 * .5 * min(toDist2, anchorDist);

        for(int s=0; s<samples; ++s) {

            float t = s / (float)(samples - 1);
            ofVec2f edgePos = position;
            if(t < .5) {
                t = t / .5;
                edgePos = (1 - t) * e1 + t * position;
            }
            else {
                t = (t - .5) / .5;
                edgePos = (1 - t) * position + t * e2;
            }

            //add edge sample:
            mesh.addVertex(ofVec3f(edgePos.x, edgePos.y, 1));
            mesh.addNormal(ofVec3f(0, 0, 1));

            ofVec2f pt = coeff[s][0] * a1 + coeff[s][1] * c1 + coeff[s][2] * c2 + coeff[s][3] * a2;
            ofVec2f deriv = dCoeff[s][0] * (c1 - a1) + dCoeff[s][1] * (c2 - c1) + dCoeff[s][2] * (a2 - c2);
            deriv = deriv.normalize();

            deriv /= 1 + 1 * ((pt - edgePos).length() / halfEdgeWidth - 1);

            //add spline sample:
            mesh.addVertex(ofVec3f(pt.x, pt.y, 0));
            mesh.addNormal(ofVec3f(deriv.y, -deriv.x, 0));

            if(s > 0) {
                int offIdx = offset + 2 * s;
                mesh.addTriangle(offIdx - 2, offIdx - 1, offIdx + 1);
                mesh.addTriangle(offIdx - 2, offIdx + 1, offIdx);
            }
        }
    }

    /*
    //close elements:
    int num = mesh.getVertices().size() - centerOffset - 2;
    for(int i=0; i<num; ++i)
        mesh.addTriangle(centerOffset, centerOffset + i + 1, centerOffset + i + 2);
    mesh.addTriangle(centerOffset, centerOffset + 1, centerOffset + num + 1);
    * /

}*/

