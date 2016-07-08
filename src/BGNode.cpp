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
        pushFullCircletoMesh(mesh, nodeRadius - BRIM_WIDTH);
     else if(n == 1)
        pushSingleToMesh(mesh, nodeRadius, neighbours[0]);
     else
        pushMultipleToMesh(mesh, nodeRadius, neighbours);
 }

void BGNode::pushSeparateToMesh(ofMesh & mesh, float nodeRadius) {
    pushFullCircletoMesh(mesh, nodeRadius - BRIM_WIDTH);
}

void BGNode::pushSingleToMesh(ofMesh & mesh, float nodeRadius, BGNode* neighbour) {
    
    float subRadius = nodeRadius - BRIM_WIDTH;

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
        float dCenter = (d1 + d2) / 2;


        int samples = SINGLE_SPLINE_SAMPLES;
        pushSplineToMesh(mesh, samples, anchorRight, controlPoint, q);
        pushSplineToMesh(mesh, samples, q, controlPoint, anchorLeft);
        
        int splineStitchOffset = mesh.getVertices().size();
        
        float distances[2] = { d1, d2 };
        stitchPreviousSplines(mesh, 2, samples, distances, dCenter);
        
        /*
        int offset = mesh.getVertices().size();
        mesh.addTriangle(centerOffset, centerOffset + 1, offset - 2);
        */
        
        //sample circle:
        int circleOffset = mesh.getVertices().size();
        pushCircleArctoMesh(mesh, centerOffset, subRadius, atan2f(to.y, to.x) + angle, 2 * (M_PI - angle));
        
        //stitch 'em up:
        int offset = mesh.getVertices().size();
        
        mesh.addTriangle(circleOffset, circleOffset + 1, splineStitchOffset - 1);
        mesh.addTriangle(circleOffset, splineStitchOffset - 1, splineStitchOffset - 2);
        mesh.addTriangle(centerOffset, circleOffset, splineStitchOffset - 2);
        mesh.addTriangle(centerOffset, splineStitchOffset, splineStitchOffset - 2);
        
        
        mesh.addTriangle(centerOffset, splineStitchOffset, centerOffset + 1);
        mesh.addTriangle(centerOffset, offset - 2, centerOffset + 1);
        mesh.addTriangle(offset - 1, offset - 2, centerOffset + 1);
        mesh.addTriangle(offset - 1, centerOffset + 1, centerOffset + 2);
        
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
    int samples = SPLINE_SAMPLES;
    float distances[n];
    for(int i=0; i<n; ++i) {

        int j = (i + 1) % n;
        
        ofVec2f a1 = (position + neighbours[indices[i]]->position) / 2.0;
        ofVec2f a2 = (position + neighbours[indices[j]]->position) / 2.0;

        float d = (nodeDistance + neighbours[indices[i]]->nodeDistance) / 2.0;
        distances[i] = d;

        pushSplineToMesh(mesh, samples, a1, position, a2);
    }
    
    stitchPreviousSplines(mesh, n, samples, distances, nodeDistance);
}

void BGNode::pushSplineToMesh(ofMesh & mesh, int splineSamples, ofVec2f a1, ofVec2f c, ofVec2f a2) {

    for(int s=0; s<splineSamples; ++s) {
        float t = s / (float)(splineSamples - 1);  
        pushSplineSampleToMesh(mesh, t, a1, c, a2);
        if(s > 0)
            stitchPreviousSplineSamples(mesh);
    }
}

void BGNode::pushSplineSampleToMesh(ofMesh & mesh, float t, ofVec2f a1, ofVec2f c, ofVec2f a2) {

    float coeff1 = (1 - t) * (1 - t);
    float coeff2 = 2 * (1 - t) * t;
    float coeff3 = t * t;

    ofVec2f pt = coeff1 * a1 + coeff2 * c + coeff3 * a2;
    ofVec2f deriv = 2 * (1 - t) * (c - a1) + 2 * t * (a2 - c);
    float dLength = deriv.length();
    ofVec2f normal = ofVec2f(deriv.y / dLength, -deriv.x / dLength);
   
   // pt += (.5 * EDGE_WIDTH - BRIM_WIDTH) * normal;

    pushVertex(mesh, pt.x, pt.y, .7, normal.x, normal.y, 0, 0);
    pushVertex(mesh, pt.x + BRIM_WIDTH * normal.x, pt.y + BRIM_WIDTH * normal.y, 0, normal.x, normal.y, 0, 0);
}

void BGNode::stitchPreviousSplineSamples(ofMesh & mesh) {
    int currOffset = mesh.getVertices().size();
    mesh.addTriangle(currOffset - 2, currOffset - 3, currOffset - 4);
    mesh.addTriangle(currOffset - 2, currOffset - 3, currOffset - 1);
}

void BGNode::stitchPreviousSplines(ofMesh & mesh, int splineCount, int splineSamples, float* distances, float centerDistance) {
   // int currOffset = 
   int offset = mesh.getVertices().size();
   
   int centerSamplesIdx = offset;
   
   int halfSamples = splineSamples / 2 + 1;
   
   bool fillCenter = splineCount > 2;
   ofVec3f centers[splineCount];
   float centerDistances[splineCount];
   int fillCenterOffset = offset + splineCount * halfSamples;
   
   for(int i=0; i<splineCount; ++i) {
       int j = i == 0 ? (splineCount - 1) : (i - 1);
       
       int localOffset = offset - 2 * splineSamples * (splineCount - i);
       int oppositeOffset = offset - 2 * splineSamples * (splineCount - j);
       
       for(int s=0; s<halfSamples; ++s) {
           
           int localIdx = localOffset + 2 * s;
           int oppositeIdx = oppositeOffset + 2 * (splineSamples - s - 1);
           
           ofVec3f v1, v2, n1, n2;
           sampleVertex(mesh, localIdx, v1, n1);
           sampleVertex(mesh, oppositeIdx, v2, n2);
           
           float t = s / (float)(halfSamples-1);
           float d = (1 - t) * distances[i] + t * centerDistance;
           
           ofVec3f loc = (v1 + v2) / 2.0;
           pushVertex(mesh, loc.x, loc.y, 1, 0, 0, 1, d);
           
           float fromCenterDist = (v1 - loc).length();
           float firstDistance = d + fromCenterDist;
           float firstZ = sqrtf(BRIM_WIDTH / (BRIM_WIDTH + fromCenterDist));
           //firstZ = .5 + .5 * firstZ;
           float firstNormalFactor = sqrtf(1 - firstZ * firstZ);
           
           //update distances:
           mesh.setTexCoord(localIdx, ofVec2f(firstDistance, 0));
           mesh.setTexCoord(localIdx + 1, ofVec2f(firstDistance + BRIM_WIDTH, 0));
           mesh.setTexCoord(oppositeIdx, ofVec2f(firstDistance, 0));
           mesh.setTexCoord(oppositeIdx + 1, ofVec2f(firstDistance + BRIM_WIDTH, 0));
           
           //update normals:
           mesh.setNormal(localIdx, ofVec3f(firstNormalFactor * n1.x, firstNormalFactor * n1.y, firstZ));
           mesh.setNormal(oppositeIdx, ofVec3f(firstNormalFactor * n2.x, firstNormalFactor * n2.y, firstZ));
           
           //update locations:
           mesh.setVertex(localIdx, ofVec3f(v1.x, v1.y, firstZ));
           mesh.setVertex(oppositeIdx, ofVec3f(v2.x, v2.y, firstZ));
           
           //updateVertexAttributes(mesh, localIdx, v1.x, v1.y, firstZ, firstNormalFactor * n1.x, firstNormalFactor * n1.y, firstZ, d + firstDistance);
           //updateVertexAttributes(mesh, localIdx + 1, v1 + n1 * BRIM_WIDTH, n1, d + firstDistance + BRIM_WIDTH);
           //updateVertexAttributes(mesh, oppositeIdx, v2.x, v2.y, firstZ, firstNormalFactor * n2.x, firstNormalFactor * n2.y, firstZ, d + firstDistance);
           //updateVertexAttributes(mesh, oppositeIdx + 1, v2 + n2 * BRIM_WIDTH, n2, d + firstDistance + BRIM_WIDTH);
           
           if(s > 0) {
               mesh.addTriangle(localIdx, localIdx - 2, centerSamplesIdx);
               mesh.addTriangle(localIdx - 2, centerSamplesIdx, centerSamplesIdx - 1);
               mesh.addTriangle(centerSamplesIdx - 1, oppositeIdx + 2, oppositeIdx);
               mesh.addTriangle(centerSamplesIdx - 1, centerSamplesIdx, oppositeIdx);
               
               if(fillCenter && s == halfSamples - 1) {
                   
                   int otherFillCenterIdx = (i == 0) ? (fillCenterOffset + splineCount - 1) : (fillCenterOffset + i - 1);
    
                   //add triangles:
                   centers[i] = loc;
                   centerDistances[i] = firstDistance;
                   //sumCenter += loc;
                   mesh.addTriangle(localIdx, centerSamplesIdx, fillCenterOffset + i);
                   mesh.addTriangle(oppositeIdx, centerSamplesIdx, otherFillCenterIdx);
                   mesh.addTriangle(centerSamplesIdx, otherFillCenterIdx, fillCenterOffset + i);
                   mesh.addTriangle(fillCenterOffset + splineCount, otherFillCenterIdx, fillCenterOffset + i);
                   //mesh.addTriangle(fillCenterOffset + splineCount, otherFillCenterIdx, fillCenterOffset + i);
               }
           }
           
           ++centerSamplesIdx;
       }
   }
   
   if(fillCenter) {
       
       ofVec3f sumCenter(0,0);
       for(int i=0; i<splineCount; ++i) {
           int j = (i + 1) % splineCount;
           ofVec3f c1 = centers[i];
           ofVec3f c2 = centers[j];
           
           pushVertex(mesh, (c1.x + c2.x) / 2, (c1.y + c2.y) / 2, 1, 0, 0, 1, .8 * centerDistance + .2 * (centerDistances[i] + centerDistances[j]) / 2);
           
           sumCenter += c1;
       }
       
       pushVertex(mesh, sumCenter.x / splineCount, sumCenter.y / splineCount, 1, 0, 0, 1, centerDistance);
   }
}

int BGNode::pushCenterToMesh(ofMesh & mesh) {
    int centerOffset = mesh.getVertices().size();
    pushVertex(mesh, position.x, position.y, 1, 0, 0, 1, nodeDistance);
    return centerOffset;
}

void BGNode::pushFullCircletoMesh(ofMesh & mesh, float subRadius) {
    
    int centerOffset = pushCenterToMesh(mesh);
    
    int samples = 20;
    for(int s = 0; s<samples; ++s) {

        float t = s / (float)(samples - 1);
        float angle = t * 2 * M_PI;

        ofVec2f normal = ofVec2f(cosf(angle), sinf(angle));
        ofVec2f pt = position + subRadius * normal;

        int currOffset = mesh.getVertices().size();
        
        pushVertex(mesh, pt.x, pt.y, 1, 0, 0, 1, nodeDistance + subRadius);
        pushVertex(mesh, pt.x + BRIM_WIDTH * normal.x, pt.y + BRIM_WIDTH * normal.y, 0, normal.x, normal.y, 0, nodeDistance + subRadius + BRIM_WIDTH);

        if(s > 0) {
            mesh.addTriangle(centerOffset, currOffset - 2, currOffset);
            mesh.addTriangle(currOffset, currOffset - 1, currOffset - 2);
            mesh.addTriangle(currOffset, currOffset - 1, currOffset + 1);
        }
    }
}

void BGNode::pushCircleArctoMesh(ofMesh & mesh, int centerOffset, float subRadius, float startAngle, float dAngle) {

    int samples = (int)(10 * dAngle / M_PI);
    samples = max(1, samples);
    
    int currOffset;
    for(int s = 0; s<samples; ++s) {

        float t = (s + 1) / (float)(samples + 1);

        float angle = startAngle + t * dAngle;

        ofVec2f normal = ofVec2f(cosf(angle), sinf(angle));
        ofVec2f pt = position + subRadius * normal;

        currOffset = mesh.getVertices().size();
     
        pushVertex(mesh, pt.x, pt.y, 1, 0, 0, 1, nodeDistance + subRadius);
        pushVertex(mesh, pt.x + BRIM_WIDTH * normal.x, pt.y + BRIM_WIDTH * normal.y, 0, normal.x, normal.y, 0, nodeDistance + subRadius + BRIM_WIDTH);

        if(s > 0) {
            mesh.addTriangle(centerOffset, currOffset - 2, currOffset);
            mesh.addTriangle(currOffset, currOffset - 1, currOffset - 2);
            mesh.addTriangle(currOffset, currOffset - 1, currOffset + 1);
        }
    }

    /*
    //bind circle to next spline:
    currOffset = mesh.getVertices().size();
    mesh.addTriangle(centerOffset + 1, centerOffset + 2, currOffset - 1);
    mesh.addTriangle(centerOffset + 1, currOffset - 1, currOffset - 2);
    mesh.addTriangle(centerOffset, centerOffset + 1, currOffset - 2);
    */
}

 void BGNode::pushVertex(ofMesh & mesh, float x, float y, float z, float nx, float ny, float nz, float d) {
    mesh.addVertex(ofVec3f(x, y, z));
    mesh.addNormal(ofVec3f(nx, ny, nz));
    mesh.addTexCoord(ofVec2f(d, 0));
    mesh.addColor(ofFloatColor(255, 255, 255));
 }
 
 void BGNode::updateVertexAttributes(ofMesh & mesh, int idx, ofVec3f pos, ofVec3f normal, float d) {
     updateVertexAttributes(mesh, idx, pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, d);
 }
 
 void BGNode::updateVertexAttributes(ofMesh & mesh, int idx, float x, float y, float z, float nx, float ny, float nz, float d) {
    mesh.setVertex(idx, ofVec3f(x, y, z));
    mesh.setNormal(idx, ofVec3f(nx, ny, nz));
    mesh.setTexCoord(idx, ofVec2f(d, 0));
 }
 
 void BGNode::sampleVertex(ofMesh & mesh, int idx, ofVec3f & pos, ofVec3f & normal) {
     pos = mesh.getVertex(idx);
     normal = mesh.getNormal(idx);
 }
