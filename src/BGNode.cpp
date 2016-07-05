#include "BGNode.h"

BGNode::BGNode()
:   position(0, 0)
,   traversalId(0)
,   nodeRadius(40)
{

}

BGNode::~BGNode() {

}

void BGNode::traversePushToMesh(ofMesh & mesh) {
    traversePushToMesh(mesh, NULL);
}

void BGNode::traversePushToMesh(ofMesh & mesh, BGNode* parentNode) {

    //root call?
    if(parentNode == NULL || parentNode == this)
        traverseDistributeDistance(0, NULL);

    pushToMesh(mesh);

    for(int i=0; i<neighbours.size(); ++i) {
        BGNode* neighbour = neighbours[i];
        if(neighbour != parentNode)
            neighbour->traversePushToMesh(mesh, this);
    }
}

void BGNode::traverseDistributeDistance(float parentDistance, BGNode* parentNode) {

    if(this == parentNode || parentNode == NULL)
        nodeDistance = 0;
    else
        nodeDistance = parentDistance + (position - parentNode->position).length(); 

    for(int i=0; i<neighbours.size(); ++i) {
        BGNode* neighbour = neighbours[i];
        if(neighbour != parentNode)
            neighbour->traverseDistributeDistance(nodeDistance, this);
    }

}

void BGNode::pushToMesh(ofMesh & mesh) {
     
     int n = neighbours.size();
     
     if(n == 0)
        pushSeparateToMesh(mesh, nodeRadius);
     else if(n == 1)
        pushSingleToMesh(mesh, nodeRadius, neighbours[0]);
     else
        pushMultipleToMesh(mesh, nodeRadius, neighbours);
 }

void BGNode::pushSeparateToMesh(ofMesh & mesh, float nodeRadius) {
    
    int centerOffset = pushCenterToMesh(mesh);
    pushCircletoMesh(mesh, centerOffset, nodeRadius - HALF_EDGE_WIDTH, 0, 2 * M_PI, false);
    
}

void BGNode::pushSingleToMesh(ofMesh & mesh, float nodeRadius, BGNode* neighbour) {
    
    float subRadius = nodeRadius - HALF_EDGE_WIDTH;

    ofVec2f q = (position + neighbour->position) / 2.0;
    ofVec3f center = ofVec3f(position.x, position.y, 0);

    ofVec2f to = q - position;
    float toDist = to.length();
    to.x /= toDist;
    to.y /= toDist;
    ofVec2f perp = ofVec2f(-to.y, to.x);

    if(toDist > subRadius) {
        
        int centerOffset = pushCenterToMesh(mesh);

        //perform spline traversal...
        ofVec2f controlPoint = .5 * (position + subRadius * to) + .5 * q;
        float angle = acosf(subRadius / toDist);

        float facU = subRadius * cosf(angle);
        float facV = subRadius * sinf(angle);

        float anchorProj = sqrtf(.5 * subRadius * subRadius);
        ofVec2f anchorLeft  = position + facU * to + facV * perp;
        ofVec2f anchorRight = position + facU * to - facV * perp;

        float d1 = nodeDistance;
        float d2 = (d1 + neighbour->nodeDistance) / 2;

        //traverse left spline:
        pushSplineToMesh(mesh, centerOffset, q, controlPoint, anchorLeft, d2, d1);

        //sample circle:
        pushCircletoMesh(mesh, centerOffset, subRadius, atan2f(to.y, to.x) + angle, 2 * (M_PI - angle), true);

        //traverse right spline:
        pushSplineToMesh(mesh, centerOffset, anchorRight, controlPoint, q, d1, d2);
    }
    else
        pushSeparateToMesh(mesh, nodeRadius);
}

void BGNode::pushMultipleToMesh(ofMesh & mesh, float nodeRadius, std::vector<BGNode*> & neighbours) {

    int n = neighbours.size();
    if(neighbours.size() == 0)
        return;

    int indices[n];
    float angles[n];
    for(int i=0; i<n; ++i) {
        indices[i] = -1;
        angles[i] = 10000.0;
    }

    //ORDER VERTICES:
    for(int i=0; i<n; ++i) {

        ofVec2f pos = neighbours[i]->position;
        float angle = atan2f(pos.y - position.y, pos.x - position.x);
        if(angle < 0)
            angle += 2 * M_PI;

        for(int idx=0; idx<n; ++idx) {

            if(angle < angles[idx]) {

                //insert:
                for(int j=n-1; j>idx; --j) {
                    angles[j] = angles[j - 1];
                    indices[j] = indices[j - 1];
                }

                angles[idx] = angle;
                indices[idx] = i;

                break;
            }
        }
    }
    
    //NOTE: samples must be uneven!
    int offset = mesh.getVertices().size();
    int samples = 21;
    int stitchTo = samples / 2;
    for(int i=0; i<n; ++i) {

        int j = (i + 1) % n;
        int prev = i == 0 ? (n - 1) : (i - 1);
        
        ofVec2f a1 = (position + neighbours[indices[i]]->position) / 2.0;
        ofVec2f a2 = (position + neighbours[indices[j]]->position) / 2.0;
        ofVec2f to1 = (a1 - position);
        ofVec2f to2 = (a2 - position);
        float toDist1 = to1.length();
        float toDist2 = to2.length();
        to1 /= toDist1;
        to2 /= toDist2;

        float d1 = (nodeDistance + neighbours[indices[i]]->nodeDistance) / 2.0;
        float d2 = (nodeDistance + neighbours[indices[j]]->nodeDistance) / 2.0;
        float dCenter = .3 * (d1 + d2) / 2.0 + .7 * nodeDistance;

        int prevStitchIndex = 0;
        for(int s=0; s<samples; ++s) {
            float t = s / (float)(samples - 1);

            float d = t;
            if(t < .5) {
                float frac = t / .5;
                d = (1 - frac) * d1 + frac * dCenter;
            }
            else {
                float frac = (t - .5) / .5;
                d = (1 - frac) * dCenter + frac * d2;
            }

            //float d = (1 - t) * d1 + t * d2;

            if(s > 0) {
                int splineIdx = (s <= stitchTo) ? prev : j;
                int oppositeOffset = offset + 2 * (splineIdx * samples + samples - s - 1);
                pushSplineSampleToMesh(mesh, t, oppositeOffset, a1, position, a2, d);
            }
            else {
                pushSplineSampleToMesh(mesh, t, a1, position, a2, d);
            }
        }
        
        if(i > 1) {
            
            int stride = 2 * samples;
            int centerIdx = 2 * stitchTo;
            int off1 = offset + centerIdx;
            int off2 = offset + stride * (i - 1) + centerIdx;
            int off3 = offset + stride * i + centerIdx;
            mesh.addTriangle(off1, off2, off3);
        }
    }
}

void BGNode::pushSplineToMesh(ofMesh & mesh, int centerOffset, ofVec2f a1, ofVec2f c, ofVec2f a2, float d1, float d2) {

    int samples = 10;
    for(int s=0; s<samples; ++s) {

        float t = s / (float)(samples - 1);

        float d = (1 - t) * d1 + t * d2;

        if(s > 0)
            pushSplineSampleToMesh(mesh, t, centerOffset, a1, c, a2, d);
        else 
            pushSplineSampleToMesh(mesh, t, a1, c, a2, d);
    }
}

void BGNode::pushSplineSampleToMesh(ofMesh & mesh, float t, int centerOffset, ofVec2f a1, ofVec2f c, ofVec2f a2, float d) {

    pushSplineSampleToMesh(mesh, t, a1, c, a2, d);

    int currOffset = mesh.getVertices().size();
    mesh.addTriangle(centerOffset, currOffset - 4, currOffset - 2);
    mesh.addTriangle(currOffset - 2, currOffset - 3, currOffset - 4);
    mesh.addTriangle(currOffset - 2, currOffset - 3, currOffset - 1);

}

void BGNode::pushSplineSampleToMesh(ofMesh & mesh, float t, ofVec2f a1, ofVec2f c, ofVec2f a2, float d) {

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
    mesh.addColor(ofFloatColor(d, 0, 0));
    mesh.addVertex(ofVec3f(pt.x + HALF_EDGE_WIDTH * normal.x, pt.y + HALF_EDGE_WIDTH * normal.y, 0));
    mesh.addNormal(ofVec3f(normal.x, normal.y, 0));
    mesh.addColor(ofFloatColor(d, 0, 0));
}

int BGNode::pushCenterToMesh(ofMesh & mesh) {
    int centerOffset = mesh.getVertices().size();
    mesh.addVertex(ofVec3f(position.x, position.y, 1));
    mesh.addNormal(ofVec3f(0, 0, 1));
    mesh.addColor(ofFloatColor(nodeDistance, 0, 0));
    return centerOffset;
}

void BGNode::pushCircletoMesh(ofMesh & mesh, int centerOffset, float subRadius, float startAngle, float dAngle, bool stitchToNeighbours) {

    int samples = (int)(10 * dAngle / M_PI);
    samples = max(1, samples);
    
    int currOffset;
    for(int s = 0; s<samples; ++s) {

        float t = 0;
        if(stitchToNeighbours)
            t = (s + 1) / (float)(samples + 1);
        else
            t = s / (float)(samples - 1);

        float angle = startAngle + t * dAngle;

        ofVec2f normal = ofVec2f(cosf(angle), sinf(angle));
        ofVec2f pt = position + subRadius * normal;

        currOffset = mesh.getVertices().size();

        mesh.addVertex(ofVec3f(pt.x, pt.y, 1));
        mesh.addNormal(ofVec3f(0, 0, 1));
        mesh.addColor(ofFloatColor(nodeDistance, 0, 0));
        mesh.addVertex(ofVec3f(pt.x + HALF_EDGE_WIDTH * normal.x, pt.y + HALF_EDGE_WIDTH * normal.y, 0));
        mesh.addNormal(ofVec3f(normal.x, normal.y, 0));
        mesh.addColor(ofFloatColor(nodeDistance, 0, 0));

        if(stitchToNeighbours || s > 0) {
            mesh.addTriangle(centerOffset, currOffset - 2, currOffset);
            mesh.addTriangle(currOffset, currOffset - 1, currOffset - 2);
            mesh.addTriangle(currOffset, currOffset - 1, currOffset + 1);
        }
    }

    if(stitchToNeighbours) {
        //bind circle to next spline:
        currOffset = mesh.getVertices().size();
        mesh.addTriangle(centerOffset, currOffset - 2, currOffset);
        mesh.addTriangle(currOffset, currOffset - 1, currOffset - 2);
        mesh.addTriangle(currOffset, currOffset - 1, currOffset + 1);
    }
}
