#include "BGTouchNode.h"

BGTouchNode::BGTouchNode(float radius)
:   hasInput(false)
,   nodeRadius(radius)
{

}

BGTouchNode::~BGTouchNode() {

}

void BGTouchNode::setNeighbour(BGNode* neighbor) {
    mNeighbor = neighbor;
}

void BGTouchNode::render() {
    ofSetColor(0, 255, 0, 50);
    ofCircle(position, nodeRadius);
}

void BGTouchNode::update(float dt) {

}

void BGTouchNode::traverseSpline(ofMesh & mesh, int centerOffset, ofVec2f a1, ofVec2f c, ofVec2f a2) {

    float halfEdgeWidth = .5 * EDGE_WIDTH;

    int samples = 20;
    for(int s=0; s<samples; ++s) {

        float t = s / (float)(samples - 1);
        float coeff1 = (1 - t) * (1 - t);
        float coeff2 = 2 * (1 - t) * t;
        float coeff3 = t * t;

        ofVec2f pt = coeff1 * a1 + coeff2 * c + coeff3 * a2;
        ofVec2f deriv = 2 * (1 - t) * (c - a1) + 2 * t * (a2 - c);
        float dLength = deriv.length();
        ofVec2f normal = ofVec2f(deriv.y / dLength, -deriv.x / dLength);

        int currOffset = mesh.getVertices().size();
        //add spline samples:
        mesh.addVertex(ofVec3f(pt.x, pt.y, 1));
        mesh.addNormal(ofVec3f(0, 0, 1));
        mesh.addVertex(ofVec3f(pt.x + halfEdgeWidth * normal.x, pt.y + halfEdgeWidth * normal.y, 0));
        mesh.addNormal(ofVec3f(normal.x, normal.y, 0));

        if(s > 0) {
            mesh.addTriangle(centerOffset, currOffset - 2, currOffset);
            mesh.addTriangle(currOffset, currOffset - 1, currOffset - 2);
            mesh.addTriangle(currOffset, currOffset - 1, currOffset + 1);
        }
    }
}

void BGTouchNode::pushToMesh(ofMesh & mesh) {

    float halfEdgeWidth = .5 * EDGE_WIDTH;
    float subRadius = nodeRadius - halfEdgeWidth;

    ofVec2f p = position;
    ofVec2f q = (position + mNeighbor->position) / 2.0;
    ofVec3f center = ofVec3f(p.x, p.y, 0);

    ofVec2f to = q - p;
    float toDist = to.length();
    to.x /= toDist;
    to.y /= toDist;
    ofVec2f perp = ofVec2f(-to.y, to.x);

    int centerOffset = mesh.getVertices().size();
    mesh.addVertex(ofVec3f(p.x, p.y, 1));
    mesh.addNormal(ofVec3f(0, 0, 1));

    if(toDist > subRadius) {

        float f1 = nodeRadius > 45 ? .75 : .5;
        float f2 = 1 - f1;

        //perform spline traversal...
        ofVec2f controlPoint = f1 * (p + subRadius * to) + f2 * q;
        float angle = acosf(subRadius / toDist);

        float facU = subRadius * cosf(angle);
        float facV = subRadius * sinf(angle);

        float anchorProj = sqrtf(.5 * subRadius * subRadius);
        ofVec2f anchorLeft  = p + facU * to + facV * perp;
        ofVec2f anchorRight = p + facU * to - facV * perp;

        //traverse left spline:
        traverseSpline(mesh, centerOffset, q, controlPoint, anchorLeft);

        //sample circle:
        float minAngle = atan2f(to.y, to.x) + angle;
        float dAngle = 2 * (M_PI - angle);

        int currOffset;

        int samples = (int)(20 * dAngle / M_PI);
        samples = max(1, samples);
        for(int s =0; s<samples; ++s) {
            float t = (s + 1) / (float)(samples + 1);
            float angle = minAngle + t * dAngle;

            ofVec2f normal = ofVec2f(cosf(angle), sinf(angle));
            ofVec2f pt = p + subRadius * normal;

            currOffset = mesh.getVertices().size();

            mesh.addVertex(ofVec3f(pt.x, pt.y, 1));
            mesh.addNormal(ofVec3f(0, 0, 1));
            mesh.addVertex(ofVec3f(pt.x + halfEdgeWidth * normal.x, pt.y + halfEdgeWidth * normal.y, 0));
            mesh.addNormal(ofVec3f(normal.x, normal.y, 0));

            mesh.addTriangle(centerOffset, currOffset - 2, currOffset);
            mesh.addTriangle(currOffset, currOffset - 1, currOffset - 2);
            mesh.addTriangle(currOffset, currOffset - 1, currOffset + 1);
        }


        currOffset = mesh.getVertices().size();
        mesh.addTriangle(centerOffset, currOffset - 2, currOffset);
        mesh.addTriangle(currOffset, currOffset - 1, currOffset - 2);
        mesh.addTriangle(currOffset, currOffset - 1, currOffset + 1);


        //traverse right spline:
        traverseSpline(mesh, centerOffset, anchorRight, controlPoint, q);
    }
    else {

        //only sample circles...
        int currOffset;
        int samples = 30;
        for(int s =0; s<samples; ++s) {
            float t = s / (float)(samples - 1);
            float angle = t * 2 * M_PI;

            ofVec2f normal = ofVec2f(cosf(angle), sinf(angle));
            ofVec2f pt = p + subRadius * normal;

            currOffset = mesh.getVertices().size();

            mesh.addVertex(ofVec3f(pt.x, pt.y, 1));
            mesh.addNormal(ofVec3f(0, 0, 1));
            mesh.addVertex(ofVec3f(pt.x + halfEdgeWidth * normal.x, pt.y + halfEdgeWidth * normal.y, 0));
            mesh.addNormal(ofVec3f(normal.x, normal.y, 0));

            if(s > 0) {
                mesh.addTriangle(centerOffset, currOffset - 2, currOffset);
                mesh.addTriangle(currOffset, currOffset - 1, currOffset - 2);
                mesh.addTriangle(currOffset, currOffset - 1, currOffset + 1);
            }
        }
    }
}