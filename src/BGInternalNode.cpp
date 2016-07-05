#include "BGInternalNode.h"

BGInternalNode::BGInternalNode()
:   lockPosition(false)
{
    
}

BGInternalNode::~BGInternalNode() {

}

void BGInternalNode::pushNeighbor(BGNode * n) {
    neighbours.push_back(n);
}

void BGInternalNode::update(float dt) {
    
    if(lockPosition)
        return;

    int n = neighbours.size();
    if(n == 0)
        return;

    ofVec2f sumLoc(0,0);
    for(int i=0; i<n; ++i) {
        sumLoc += neighbours[i]->position;
    }
    ofVec2f meanLoc = sumLoc / (float)n;
    position = .9 * position + .1 * meanLoc;
}

void BGInternalNode::render() {
    
    int n = neighbours.size();
    
    ofSetColor(0, 50, 0, 255);
    for(int i=0; i<n; ++i) {
        ofLine(position, neighbours[i]->position);
    }
}

void BGInternalNode::pushToMesh(ofMesh & mesh) {

    int n = neighbours.size();
    if(neighbours.size() == 0)
        return;

    float halfEdgeWidth = .5 * EDGE_WIDTH;

    ofVec2f p = position;

    int indices[n];
    float angles[n];
    for(int i=0; i<n; ++i) {
        indices[i] = -1;
        angles[i] = 10000.0;
    }
    
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
    
    //NOTE: must be uneven!
    int offset = mesh.getVertices().size();
    int samples = 15;
    int stitchTo = samples / 2;
    for(int i=0; i<n; ++i) {

        int j = (i + 1) % n;
        int prev = i == 0 ? (n - 1) : (i - 1);
        
        ofVec2f e1 = (position + neighbours[indices[i]]->position) / 2.0;
        ofVec2f e2 = (position + neighbours[indices[j]]->position) / 2.0;
        ofVec2f to1 = (e1 - position);
        ofVec2f to2 = (e2 - position);
        float toDist1 = to1.length();
        float toDist2 = to2.length();
        to1 /= toDist1;
        to2 /= toDist2;

        int prevStitchIndex = 0;
        for(int s=0; s<samples; ++s) {
            float t = s / (float)(samples - 1);
            float coeff1 = (1 - t) * (1 - t);
            float coeff2 = 2 * (1 - t) * t;
            float coeff3 = t * t;
            
            ofVec2f pt = coeff1 * e1 + coeff2 * p + coeff3 * e2;
            ofVec2f deriv = 2 * (1 - t) * (p - e1) + 2 * t * (e2 - p);
            float dLength = deriv.length();
            ofVec2f normal = ofVec2f(deriv.y / dLength, -deriv.x / dLength);

            int currOffset = offset + 2 * (i * samples + s);
            
            //add spline samples:
            mesh.addVertex(ofVec3f(pt.x, pt.y, 1));
            mesh.addNormal(ofVec3f(0,0,1));
            mesh.addVertex(ofVec3f(pt.x + halfEdgeWidth * normal.x, pt.y + halfEdgeWidth * normal.y, 0));
            mesh.addNormal(ofVec3f(normal.x, normal.y, 0));

            if(s > 0) {
                //boundary (edge-width)
                mesh.addTriangle(currOffset, currOffset - 1, currOffset - 2);
                mesh.addTriangle(currOffset, currOffset - 1, currOffset + 1);

                //stitch opposite splines:
                int splineIdx = (s <= stitchTo) ? prev : j;
                int oppositeOffset = offset + 2 * (splineIdx * samples + samples - s - 1);
                mesh.addTriangle(currOffset, currOffset - 2, oppositeOffset);
                    
                prevStitchIndex = oppositeOffset;
            }
            else
                prevStitchIndex = currOffset;
        }
        
        if(i > 1) {
            
            int stride = 2 * samples;
            int centerIdx = 2 * stitchTo;
            int off1 = offset + centerIdx;
            int off2 = offset + stride * (i - 1) + centerIdx;
            int off3 = offset + stride * i + centerIdx;
            mesh.addTriangle(off1, off2, off3);
            
        }
        
        /*
        //in case of multiple splines?!!
        if(n == 3) {
            //add center triangle...
            int centerIdx = 2 * stitchTo;
            mesh.addTriangle(offset + centerIdx, offset + 2 * samples + centerIdx, offset + 4 * samples + centerIdx);
        }*/
    }
}
