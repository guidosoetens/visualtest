#include "BGNode.h"

BGNode::BGNode()
:   position(0, 0)
,   nodeRadius(40)
{

}

BGNode::~BGNode() {

}

float BGNode::traverseGetDepth() {
    return traverseGetDepth(NULL, 0);
}

float BGNode::traverseGetDepth(BGNode* parentNode, float currentDepth) {

    float maxDepth = currentDepth;

    for(int i=0; i<neighbours.size(); ++i) {
        BGNode* neighbour = neighbours[i];
        if(neighbour != parentNode)
            maxDepth = MAX(maxDepth, neighbour->traverseGetDepth(this, currentDepth + 1));
    }

    return maxDepth;
}

void BGNode::traverseDraw(BGGraphics & graphics) {
    traverseDraw(graphics, NULL, 0);
}

void BGNode::traverseDraw(BGGraphics & graphics, BGNode* parentNode, float offset) {
    
     ofMesh mesh;

     bool renderFlow = false;
     ofVec2f flowSource, flowSink1, flowSink2;

     int n = neighbours.size();
     if(n == 0)
        pushFullCircletoMesh(mesh, nodeRadius - BRIM_WIDTH);
     else if(n == 1)
        pushSingleToMesh(mesh, nodeRadius, neighbours[0]);
     else {
        pushMultipleToMesh(mesh, nodeRadius, neighbours);

        if(n > 2) {
             renderFlow = true;
        }

     }

    graphics.drawSplineMesh(mesh);

    //traverse draw:
    for(int i=0; i<neighbours.size(); ++i) {
        BGNode* neighbour = neighbours[i];
        if(neighbour != parentNode)
            neighbour->traverseDraw(graphics, this, offset + 1);
    }
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

        float d1 = 0;
        float d2 = (d1 + 0) / 2;
        float dCenter = (d1 + d2) / 2;//.7 * d1 + .3 * d2;


        int samples = SINGLE_SPLINE_SAMPLES;
        pushSplineToMesh(mesh, samples, anchorRight, controlPoint, q);
        pushSplineToMesh(mesh, samples, q, controlPoint, anchorLeft);
        
        int splineStitchOffset = mesh.getVertices().size();
        
        float distances[2] = { d1, d2 };
        stitchPreviousSplines(mesh, 2, samples, distances, dCenter);
        
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
        
        /*
        mesh.addTriangle(circleOffset, circleOffset + 1, splineStitchOffset - 1);
        mesh.addTriangle(circleOffset, splineStitchOffset - 1, splineStitchOffset - 2);
        mesh.addTriangle(centerOffset, circleOffset, splineStitchOffset - 2);
        mesh.addTriangle(centerOffset, splineStitchOffset - 2, centerOffset + 1);
        mesh.addTriangle(centerOffset, centerOffset + 1, offset - 2);
        mesh.addTriangle(centerOffset + 1, offset - 2, offset - 1);
        mesh.addTriangle(centerOffset + 1, centerOffset + 2, offset - 1);


        //mesh.addTriangle(centerOffset, circleOffset, splineStitchOffset - 2);
        //mesh.addTriangle(centerOffset, splineStitchOffset, splineStitchOffset - 2);
        
        /*
        mesh.addTriangle(centerOffset, splineStitchOffset, centerOffset + 1);
        mesh.addTriangle(centerOffset, offset - 2, centerOffset + 1);
        mesh.addTriangle(offset - 1, offset - 2, centerOffset + 1);
        mesh.addTriangle(offset - 1, centerOffset + 1, centerOffset + 2);
        */
        
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

        float d = (0 +0) / 2.0;
        distances[i] = d;

        pushSplineToMesh(mesh, samples, a1, position, a2);
    }
    
    stitchPreviousSplines(mesh, n, samples, distances, 0);
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
   
    pushVertex(mesh, pt.x, pt.y, 1.0, 0, 0, 1, 0.5);
    pushVertex(mesh, pt.x + BRIM_WIDTH * normal.x, pt.y + BRIM_WIDTH * normal.y, 0, normal.x, normal.y, 0, 1);
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
   
   int fillCenterOffset = offset + splineCount * halfSamples;
   
   ofVec3f sumCenter(0,0);
   float centerLengths[splineCount];
   int locIndices[splineCount];
   int oppIndices[splineCount];
   
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
           
           //float t = s / (float)(halfSamples-1);

/*
           float fromDistance = distances[i] < nodeDistance ? 0 : 1;

           float toDistance = .5;
           float d = (1 - t) * fromDistance + t * .5;// (1 - t) * distances[i] + t * centerDistance;

           //mesh.setTexCoord(oppositeIdx, ofVec2f(d, off_dist1));
           float locD = d;
           if(splineCount > 2) {
                float tt = t * t;
                t *=  (1 - tt) + tt * mCenterFactor;
                locD = (1 - t) * fromDistance + t * .5;
           }*/

           ofVec3f loc = (v1 + v2) / 2.0;
           pushVertex(mesh, loc.x, loc.y, 1, 0, 0, 1, 0);

           float fromCenterDist = (v1 - loc).length();
           float off_dist1 = fromCenterDist / (fromCenterDist + BRIM_WIDTH);

           float t = s / (float)(halfSamples-1);

           if(splineCount > 2) {
              off_dist1 = .7 * t;// pow(t, 1.2);
           }

            //update distances:
           mesh.setTexCoord(localIdx, ofVec2f(off_dist1, 0));
           mesh.setTexCoord(localIdx + 1, ofVec2f(1, 0));
           mesh.setTexCoord(oppositeIdx, ofVec2f(off_dist1, 0));
           mesh.setTexCoord(oppositeIdx + 1, ofVec2f(1, 0));
           
           /*
           float fromCenterDist = (v1 - loc).length();
           float firstDistance = d + fromCenterDist;
           float firstZ = sqrtf(BRIM_WIDTH / (BRIM_WIDTH + fromCenterDist));
           firstZ = 1;//.5 + .5 * firstZ;
           float firstNormalFactor = sqrtf(1 - firstZ * firstZ);
           */

          // float off_dist1 = fromCenterDist / (fromCenterDist + BRIM_WIDTH);

/*
           if(splineCount > 2) {
              off_dist1 = mOffsetFactor * t;
           }*/
           
           /*
           //update distances:
           mesh.setTexCoord(localIdx, ofVec2f(d, off_dist1));
           mesh.setTexCoord(localIdx + 1, ofVec2f(d, 1));
           mesh.setTexCoord(oppositeIdx, ofVec2f(d, off_dist1));
           mesh.setTexCoord(oppositeIdx + 1, ofVec2f(d, 1));
           
           
           //update normals:
           mesh.setNormal(localIdx, ofVec3f(firstNormalFactor * n1.x, firstNormalFactor * n1.y, firstZ));
           mesh.setNormal(oppositeIdx, ofVec3f(firstNormalFactor * n2.x, firstNormalFactor * n2.y, firstZ));
           
           //update locations:
           mesh.setVertex(localIdx, ofVec3f(v1.x, v1.y, firstZ));
           mesh.setVertex(oppositeIdx, ofVec3f(v2.x, v2.y, firstZ));
           */
           
           //updateVertexAttributes(mesh, localIdx, v1.x, v1.y, firstZ, firstNormalFactor * n1.x, firstNormalFactor * n1.y, firstZ, d + firstDistance);
           //updateVertexAttributes(mesh, localIdx + 1, v1 + n1 * BRIM_WIDTH, n1, d + firstDistance + BRIM_WIDTH);
           //updateVertexAttributes(mesh, oppositeIdx, v2.x, v2.y, firstZ, firstNormalFactor * n2.x, firstNormalFactor * n2.y, firstZ, d + firstDistance);
           //updateVertexAttributes(mesh, oppositeIdx + 1, v2 + n2 * BRIM_WIDTH, n2, d + firstDistance + BRIM_WIDTH);
           
           if(s > 0) {
               mesh.addTriangle(localIdx, localIdx - 2, centerSamplesIdx);
               mesh.addTriangle(localIdx - 2, centerSamplesIdx, centerSamplesIdx - 1);
               mesh.addTriangle(centerSamplesIdx - 1, oppositeIdx + 2, oppositeIdx);
               mesh.addTriangle(centerSamplesIdx - 1, centerSamplesIdx, oppositeIdx);

                if(splineCount > 2 && s == halfSamples - 1) {
                    locIndices[i] = localIdx;
                    oppIndices[i] = oppositeIdx;

                    //mesh.addTriangle(localIdx, centerSamplesIdx, offset - 1);
                    //mesh.addTriangle(oppositeIdx, centerSamplesIdx, offset - 1);
                    sumCenter += loc;
                }
           }

           ++centerSamplesIdx;
       }
   }
   
   if(splineCount > 2) {

       ofVec3f avgCenter = sumCenter / splineCount;
       pushVertex(mesh, avgCenter.x, avgCenter.y, 1, 0, 0, 1, 0);// nodeDistance);

       //ofVec3f v, n;

       int firstIdx = 0, prevIdx = 0;

       for(int i=0; i<splineCount; ++i) {

            //int j = (i + 1) % splineCount;

            int idx = centerSamplesIdx - 1 - (splineCount - i - 1) * halfSamples; 
            mesh.addTriangle(idx, locIndices[i], centerSamplesIdx);//centerSamplesIdx, oppIndices[i], idx);
            mesh.addTriangle(idx, oppIndices[i], centerSamplesIdx);

            //int idx2 = centerSamplesIdx - 1 - (splineCount - j - 1) * halfSamples; 

            /*
            mesh.addTriangle(idx1, locIndices[i], idx2);//centerSamplesIdx, oppIndices[i], idx);
            if(i == 0)
                firstIdx = idx1;
            else if(i > 1) {
                //add rectangle to center fan:
                 mesh.addTriangle(idx1, firstIdx, prevIdx);
            }
            prevIdx = idx1;
            */
       }
   }
}

/*
void BGNode::stitchPreviousSplines(ofMesh & mesh, int splineCount, int splineSamples, float* distances, float centerDistance) {

    int offset = mesh.getVertices().size();

    int halfSamples = splineSamples / 2 + 1;
    for(int i=0; i<splineCount; ++i) {
        int j = i == 0 ? (splineCount - 1) : (i - 1);
       
        int localOffset = offset - 2 * splineSamples * (splineCount - i);
        int oppositeOffset = offset - 2 * splineSamples * (splineCount - j);

        for(int s=1; s<halfSamples; ++s) {
           
           int localIdx = localOffset + 2 * s;
           int oppositeIdx = oppositeOffset + 2 * (splineSamples - s - 1);
            mesh.addTriangle(localIdx, localIdx - 2, oppositeIdx);
            mesh.addTriangle(localIdx - 2, oppositeIdx, oppositeIdx + 2); 
        }
    }

    //stitch centers:
    if(splineCount > 2) {

        int idx1 = offset - 2 * splineSamples * (splineCount - 0) + 2 * (halfSamples - 1);
        int idx2 = offset - 2 * splineSamples * (splineCount - 1) + 2 * (halfSamples - 1);

        for(int i=2; i<splineCount; ++i) {
            int idx3 = offset - 2 * splineSamples * (splineCount - i) + 2 * (halfSamples - 1);
            mesh.addTriangle(idx1, idx2, idx3);
            idx2 = idx3;
        }
    }
}*/

int BGNode::pushCenterToMesh(ofMesh & mesh) {
    int centerOffset = mesh.getVertices().size();
    pushVertex(mesh, position.x, position.y, 1, 0, 0, 1, 0);
    return centerOffset;
}

void BGNode::pushFullCircletoMesh(ofMesh & mesh, float subRadius) {

    int centerOffset = pushCenterToMesh(mesh);
    pushCircleArctoMesh(mesh, centerOffset, subRadius, 0, 2 * M_PI, true);
}

void BGNode::pushCircleArctoMesh(ofMesh & mesh, int centerOffset, float subRadius, float startAngle, float dAngle) {

    pushCircleArctoMesh(mesh, centerOffset, subRadius, startAngle, dAngle, false);
}

void BGNode::pushCircleArctoMesh(ofMesh & mesh, int centerOffset, float subRadius, float startAngle, float dAngle, bool includeBounds) {

    ofVec2f baseVec(1,0);
    if(neighbours.size() > 0)
        baseVec = (neighbours[0]->position - position).normalize();
    ofVec2f perp(-baseVec.y, baseVec.x);

    int samples = (int)(10 * dAngle / M_PI);
    samples = max(1, samples);
    
    int currOffset;
    for(int s = 0; s<samples; ++s) {

        float t = includeBounds ? (s / (float)(samples - 1)) : ((s + 1) / (float)(samples + 1));

        float angle = startAngle + t * dAngle;

        ofVec2f normal = ofVec2f(cosf(angle), sinf(angle));
        ofVec2f pt = position + subRadius * normal;

        currOffset = mesh.getVertices().size();

        ofVec2f pt2 = pt + BRIM_WIDTH * normal;
     
        pushVertex(mesh, pt.x, pt.y, 1, 0, 0, 1, subRadius / (subRadius + BRIM_WIDTH));
        pushVertex(mesh, pt.x + BRIM_WIDTH * normal.x, pt.y + BRIM_WIDTH * normal.y, 0, normal.x, normal.y, 0, 1.0);

        //mesh.setTexCoord(currOffset, ofVec2f(0, subRadius / (subRadius + BRIM_WIDTH)));
        //mesh.setTexCoord(currOffset + 1, ofVec2f(0, 1));

        if(s > 0) {
            mesh.addTriangle(centerOffset, currOffset - 2, currOffset);
            mesh.addTriangle(currOffset, currOffset - 1, currOffset - 2);
            mesh.addTriangle(currOffset, currOffset - 1, currOffset + 1);
        }
    }
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
