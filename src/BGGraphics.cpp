#include "BGGraphics.h"

BGGraphics::BGGraphics() {
    mNetworkShader.load("shaders/boogerShader");
    renderWireframe = false;
    mTime = 0;
    renderFlow = false;
    depthTest = true;
    maxDepth = 1;
    mRevealParameter = 0;
}

BGGraphics::~BGGraphics() {

}

float BGGraphics::dot(ofVec2f a, ofVec2f b) {
    return a.x * b.x + a.y * b.y;
}

float BGGraphics::cross(ofVec2f a, ofVec2f b) {
    return a.x * b.y - a.y * b.x;
}

ofVec2f BGGraphics::reflect(ofVec2f v, ofVec2f n) {
    float d = dot(v, n);
    return v + 2 * (d * n - v);
}

bool BGGraphics::getIntersection(ofVec2f p, ofVec2f r, ofVec2f q, ofVec2f s, ofVec2f & result) {
    float div = cross(r, s);
    if(ABS(div) < .00001)
        return false;
        
    float u = cross(q - p, r) / div;
    result = q + u * s;
    return true;
}

ofVec2f BGGraphics::getBarycentricCoords(ofVec2f p, ofVec2f a, ofVec2f b, ofVec2f c)
{
    ofVec2f v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;
    return ofVec2f(v, w);
}

void BGGraphics::update(float dt) {
    mTime = fmodf(mTime + dt, 1000000.0);
    mRevealParameter = fmodf(mRevealParameter + dt / 6.0, 1.0);
}

void BGGraphics::reload() {
    mNetworkShader.load("shaders/boogerShader");
}

void BGGraphics::pushSeparateNode(ofMesh& mesh, ofVec2f position, float nodeRadius) {
    pushCirclePart(mesh, position, nodeRadius, 0, 2 * M_PI);
}

void BGGraphics::pushSingleConnectedNode(ofMesh& mesh, ofVec2f position, float nodeRadius, ofVec2f edgePoint, bool isRoot) {

    float innerRadius = nodeRadius - NETWORK_OFFSET;

    ofVec2f to = edgePoint - position;
    float toDist = to.length();
    to /= toDist;
    ofVec2f perp = ofVec2f(-to.y, to.x);

    float depthFactor = isRoot ? .5 : -.5;

    if(toDist > innerRadius) {
        
        //perform spline traversal...
        ofVec2f controlPoint = .5 * (position + innerRadius * to) + .5 * edgePoint;
        float angle = acosf(innerRadius / toDist);

        pushCirclePart(mesh, position, nodeRadius, atan2f(to.y, to.x) + angle, 2 * (M_PI - angle));
        int splineOffset = mesh.getVertices().size();

        float toAng = atan2f(to.y, to.x);
        ofVec2f toA1(cosf(toAng + angle), sinf(toAng + angle));
        ofVec2f toA2(cosf(toAng - angle), sinf(toAng - angle));

        float facU = innerRadius * cosf(angle);
        float facV = innerRadius * sinf(angle);

        float anchorProj = sqrtf(.5 * innerRadius * innerRadius);
        ofVec2f anchorLeft  = position + innerRadius * toA1;
        ofVec2f anchorRight = position + innerRadius * toA2;

        float innerRadiusFactor = .5;// innerRadius / nodeRadius;

        //add first point:
        pushVertex(mesh, anchorLeft.x + NETWORK_OFFSET * toA1.x, anchorLeft.y + NETWORK_OFFSET * toA1.y, 0, toA1.x, toA1.y, 0, 0, 1);
        pushVertex(mesh, anchorLeft.x, anchorLeft.y, 1, 0, 0, 1, 0, innerRadiusFactor);
        pushVertex(mesh, position.x, position.y, 1, 0, 0, 1, 0, 0);
        pushVertex(mesh, anchorRight.x, anchorRight.y, 1, 0, 0, 1, 0, innerRadiusFactor);
        pushVertex(mesh, anchorRight.x + NETWORK_OFFSET * toA2.x, anchorRight.y + NETWORK_OFFSET * toA2.y, 0, toA2.x, toA2.y, 0, 0, 1);

        int samples = 15;
        for(int i=1; i<samples; ++i) {

            float t = i / (float)(samples - 1);

            float localDepth = depthFactor * t;

            ofVec2f pt, normal;
            sampleSpline(anchorLeft, controlPoint, edgePoint, t, pt, normal);
            normal = -normal;
/*
            //project point on skeleton:
            //float projDistance = dot(pt - position, to);
            float innerSampleOffset = dot(pt - position, to);//(pt - centerSample).length();
            ofVec2f centerSample = position + innerSampleOffset * to;



            float centerSampleDepth = depth + depthFactor * (innerSampleOffset / toDist);
            centerSampleDepth = (centerSampleDepth + localDepth) * .5;
            */
            ofVec2f centerSample;
            getIntersection(position, to, pt, normal, centerSample);
            

            //float innerSampleOffset = (pt - centerSample).length();
            float innerSampleOffsetFactor = (1 - t) * .5;// innerSampleOffset / (innerSampleOffset + NETWORK_OFFSET);

            ofVec2f otherPt = position + reflect(pt - position, to);
            ofVec2f otherNormal = reflect(normal, to);

            pushVertex(mesh, pt.x + NETWORK_OFFSET * normal.x, pt.y + NETWORK_OFFSET * normal.y, 0, normal.x, normal.y, 0, localDepth, 1);
            pushVertex(mesh, pt.x, pt.y, 1, 0, 0, 1, localDepth, innerSampleOffsetFactor);
            pushVertex(mesh, centerSample.x, centerSample.y, 1, 0, 0, 1, localDepth, 0);
            pushVertex(mesh, otherPt.x, otherPt.y, 1, 0, 0, 1, localDepth, innerSampleOffsetFactor);
            pushVertex(mesh, otherPt.x + NETWORK_OFFSET * otherNormal.x, otherPt.y + NETWORK_OFFSET * otherNormal.y, 0, otherNormal.x, otherNormal.y, 0, localDepth, 1);

            if(i > 0) {
                int offset = splineOffset + 5 * i;

                for(int j=0; j<4; ++j) {
                     mesh.addTriangle(offset + j, offset - 5 + j, offset - 4 + j);
                     mesh.addTriangle(offset + j, offset + 1 + j, offset - 4 + j);
                }
            }
        }
    }
    else
        pushSeparateNode(mesh, position, nodeRadius);
}

void BGGraphics::pushDoubleConnectedNode(ofMesh& mesh, ofVec2f position, ofVec2f startEdgePoint, ofVec2f endEdgePoint) {

    int samples = 10;
    for(int i=0; i<samples; ++i) {

        float t = i / (float)(samples - 1);

        float localDepth = -.5 + t;

        ofVec2f pt, normal;
        sampleSpline(startEdgePoint, position, endEdgePoint, t, pt, normal);

        pushVertex(mesh, pt.x + NETWORK_OFFSET * normal.x, pt.y + NETWORK_OFFSET * normal.y, 0, normal.x, normal.y, 0, localDepth, 1);
        pushVertex(mesh, pt.x, pt.y, 1, 0, 0, 1, localDepth, 0);
        pushVertex(mesh, pt.x - NETWORK_OFFSET * normal.x, pt.y - NETWORK_OFFSET * normal.y, 0, -normal.x, -normal.y, 0, localDepth, 1);

        if(i > 0) {
            int offset = 3 * i;
            for(int j=0; j<2; ++j) {
                mesh.addTriangle(offset + j, offset - 3 + j, offset - 2 + j);
                mesh.addTriangle(offset + j, offset + 1 + j, offset - 2 + j);
            }
        }
    }
}

void BGGraphics::pushTripleConnectedNode(ofMesh& mesh, ofVec2f position, ofVec2f startEdgePoint, ofVec2f endEdgePoint1, ofVec2f endEdgePoint2) {
 
    ofVec2f orderedPts[3] = {
        startEdgePoint,
        endEdgePoint1,
        endEdgePoint2
    };

    /*
    ofMesh centerTriangleMesh;
    centerTriangleMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    ofVec2f centerTriangleTexPt = calculateInternalTexOffset(0.5, true, true, -1);
    pushVertex(centerTriangleMesh, position.x, position.y, 1, 0, 0, 1, centerTriangleTexPt.x, centerTriangleTexPt.y);
    */

    ofVec2f sumCenterPosition(0,0);

    int centerIndices[9];

    int halfSamples = 6;
    for(int idx=0; idx<3; ++idx) {

        int prevIdx = idx == 0 ? 2 : (idx - 1);
        int nextIdx = (idx + 1) % 3;

        //ofVec2f focus1 = focusPts[idx];
        //ofVec2f focus2 = focusPts[prevIdx];

        for(int i=0; i<halfSamples; ++i) {

            float t = .5 * i / (float)(halfSamples - 1);

            ofVec2f pt1, normal1, pt2, normal2;
            sampleSpline(orderedPts[idx], position, orderedPts[nextIdx], t, pt1, normal1);
            sampleSpline(orderedPts[idx], position, orderedPts[prevIdx], t, pt2, normal2);
            normal2 = -normal2;

            ofVec2f center = (pt1 + pt2) / 2.0;

            /*
            float offAngle1 = focusAngles[idx] + t * deltaAngle;
            ofVec2f offVector1 = ofVec2f(cosf(offAngle1), sinf(offAngle1));
            float offAngle2 = focusAngles[prevIdx] + (1. - t) * deltaAngle;
            ofVec2f offVector2 = ofVec2f(cosf(offAngle2), sinf(offAngle2));
            */

            //ofVec2f BGGraphics::calculateInternalTexOffset(float t, bool isSourceSpline, int offsetIndex)

            bool isSourceSegment = idx == 0;
            bool isSourceSpline1 = true, isSourceSpline2 = true;
            float t1 = t, t2 = t;

            if(idx == 1) {
                isSourceSpline1 = false;
                t2 = 1. - t;
            }
            else if(idx == 2) {
                //mirrored version of idx == 1
                isSourceSpline2 = false;
                t1 = 1. - t;
            }


            ofVec2f centerTexPt = calculateInternalTexOffset(t1, isSourceSpline1, isSourceSegment, 0);
            ofVec2f innerTexPt1 = calculateInternalTexOffset(t1, isSourceSpline1, isSourceSegment, 1);
            ofVec2f outerTexPt1 = calculateInternalTexOffset(t1, isSourceSpline1, isSourceSegment, 2);
            ofVec2f innerTexPt2 = calculateInternalTexOffset(t2, isSourceSpline2, isSourceSegment, 1);
            ofVec2f outerTexPt2 = calculateInternalTexOffset(t2, isSourceSpline2, isSourceSegment, 2);

            //TODO: remove!!
            //centerTexPt = (innerTexPt1 + innerTexPt2) / 2.0;

            //innerTexPt2 = ofVec2f(-10000,-10000);
            //outerTexPt2 = innerTexPt2;

            /*
            ofVec2f innerTexPt1 = focus1 + baseSize * offVector1;
            ofVec2f outerTexPt1 = focus1 + .5 * baseSize * offVector1;
            ofVec2f innerTexPt2 = focus2 + baseSize * offVector2;
            ofVec2f outerTexPt2 = focus2 + .5 * baseSize * offVector2;
            ofVec2f centerTexPt = (innerTexPt1 + innerTexPt2) / 2.0;
            */

            pushVertex(mesh, pt1.x + NETWORK_OFFSET * normal1.x, pt1.y + NETWORK_OFFSET * normal1.y, 0, normal1.x, normal1.y, 0, outerTexPt1.x, outerTexPt1.y);
            pushVertex(mesh, pt1.x, pt1.y, 1, 0, 0, 1, innerTexPt1.x, innerTexPt1.y);
            pushVertex(mesh, center.x, center.y, 1, 0, 0, 1, centerTexPt.x, centerTexPt.y);
            pushVertex(mesh, pt2.x, pt2.y, 1, 0, 0, 1, innerTexPt2.x, innerTexPt2.y);
            pushVertex(mesh, pt2.x + NETWORK_OFFSET * normal2.x, pt2.y + NETWORK_OFFSET * normal2.y, 0, normal2.x, normal2.y, 0, outerTexPt2.x, outerTexPt2.y);

            if(i > 0) {
                int offset = (5 * idx * halfSamples) + 5 * i;
                for(int j=0; j<4; ++j) {
                    mesh.addTriangle(offset + j, offset - 5 + j, offset - 4 + j);
                    mesh.addTriangle(offset + j, offset + 1 + j, offset - 4 + j);
                }

                if(i == halfSamples - 1) {
                    sumCenterPosition += pt1;
                    for(int j=0; j<3; ++j) {
                        centerIndices[3 * idx + j] = offset + 1 + j;
                    }
                }
            }
        }
    }

    //add center point:
    ofVec2f avgCenter = sumCenterPosition / 3.0;
    ofVec2f centerTriangleTexPt = calculateInternalTexOffset(0.5, true, true, -1);
    pushVertex(mesh, avgCenter.x, avgCenter.y, 1, 0, 0, 1, centerTriangleTexPt.x, centerTriangleTexPt.y);

    //stitch to bounds:
    int centerOffset = mesh.getVertices().size() - 1;
    for(int i=0; i<9; ++i)
         mesh.addTriangle(centerOffset, centerIndices[i], centerIndices[(i + 1) % 9]);


    //centerTriangleMesh.setVertex(0, ofVec3f(avgCenter.x, avgCenter.y, 1));

    /*
    ofVec2f avgCenter = sumCenterPosition / 3.0;
    centerTriangleMesh.setVertex(0, ofVec3f(avgCenter.x, avgCenter.y, 1));

    //stitch up center triangle fan:
    centerTriangleMesh.addVertex(centerTriangleMesh.getVertex(1));
    centerTriangleMesh.addTexCoord(centerTriangleMesh.getTexCoord(1));
    centerTriangleMesh.addNormal(centerTriangleMesh.getNormal(1));
    centerTriangleMesh.addColor(centerTriangleMesh.getColor(1));

    drawMesh(centerTriangleMesh, depth);
    */
}

void BGGraphics::sampleSpline(ofVec2f a1, ofVec2f c, ofVec2f a2, float t, ofVec2f & pt, ofVec2f & normal) {
    float coeff1 = (1 - t) * (1 - t);
    float coeff2 = 2 * (1 - t) * t;
    float coeff3 = t * t;

    pt = coeff1 * a1 + coeff2 * c + coeff3 * a2;

    ofVec2f deriv = 2 * (1 - t) * (c - a1) + 2 * t * (a2 - c);
    float dLength = deriv.length();
    normal = ofVec2f(deriv.y / dLength, -deriv.x / dLength);
}

void BGGraphics::pushVertex(ofMesh & mesh, float x, float y, float z, float nx, float ny, float nz, float offsetX, float offsetY) {
    mesh.addVertex(ofVec3f(x, y, z));
    mesh.addNormal(ofVec3f(nx, ny, nz));
    mesh.addTexCoord(ofVec2f(offsetX, offsetY));
    mesh.addColor(ofFloatColor(255, 255, 255));
}

void BGGraphics::drawMesh(ofMesh & mesh, ofVec2f nodeLocation, float nodeRadius, float nodeDepth, bool isExternal, bool deform, ofVec2f surfaceNormal) {

    if(depthTest)
        ofEnableDepthTest();
    else 
        ofDisableDepthTest();

    mNetworkShader.begin();
    mNetworkShader.setUniform1f("uTime", mTime);
    mNetworkShader.setUniform2f("uResolution", 1024, 768);
    mNetworkShader.setUniform1f("uMaxDepth", maxDepth);
    mNetworkShader.setUniform1f("uDepthOffset", nodeDepth);
    mNetworkShader.setUniform1f("uRevealParameter", .5 + .5 * sinf(mRevealParameter * 2 * M_PI));
    mNetworkShader.setUniform1i("uDrawMode", drawMode);
    mNetworkShader.setUniform1f("uBoundOffset", boundOffset);
    mNetworkShader.setUniform1i("uDeformNode", deform ? 1 : 0);
    mNetworkShader.setUniform2f("uSurfaceNormal", surfaceNormal.x, surfaceNormal.y);

    mNetworkShader.setUniform1f("uNodeRadius", nodeRadius);
    mNetworkShader.setUniform1i("uNodeIsExternal", isExternal ? 1 : 0);
    mNetworkShader.setUniform2f("uNodeCenter", nodeLocation.x, nodeLocation.y);

    mesh.draw();
    mNetworkShader.end();

    ofDisableDepthTest();

    if(renderWireframe)
        mesh.drawWireframe();
}

void BGGraphics::pushCirclePart(ofMesh& mesh, ofVec2f position, float nodeRadius, float minAngle, float deltaAngle) {

    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    pushVertex(mesh, position.x, position.y, 1, 0, 0, 1, 0, 0);

    float innerRadius = nodeRadius - NETWORK_OFFSET;

    float internalOffsetY = .5;// innerRadius / nodeRadius;

    int samples = (int)(15 * deltaAngle / M_PI);
    samples = max(1, samples);

    for(int i=0; i<samples; ++i) {
        float angle = minAngle + deltaAngle * i / (float)(samples - 1);
        ofVec2f to(cosf(angle), sinf(angle));

        ofVec2f p1 = position + to * innerRadius;
        ofVec2f p2 = position + to * nodeRadius;

        //pushVertex(ofMesh & mesh, float x, float y, float z, float nx, float ny, float nz, float offsetX, float offsetY) 
        pushVertex(mesh, p1.x, p1.y, 1, 0, 0, 1, 0, internalOffsetY);
        pushVertex(mesh, p2.x, p2.y, 0, to.x, to.y, 0, 0, 1);

        if(i > 0) {
            int offset = 1 + 2 * i;
            mesh.addTriangle(0, offset - 2, offset);
            mesh.addTriangle(offset - 2, offset - 1, offset);
            mesh.addTriangle(offset - 1, offset, offset + 1);
        }
    }
}

/*
    offsetIndex:    [0] skeleton
                    [1] spline
                    [2] offset spline
*/
ofVec2f BGGraphics::calculateInternalTexOffset(float t, bool isSourceSpline, bool isSourceSegment, int offsetIndex) {

    const float triangleHeight = .5 * tanf(M_PI / 3.0);

    const float baseSize = sqrtf(3.0);
    const float halfBaseSize = .5 * baseSize;
    const ofVec2f source(0, 1);
    const ofVec2f sink1(-halfBaseSize, -.5);
    const ofVec2f sink2(halfBaseSize, -.5);
    const ofVec2f center = (source + sink1 + sink2) / 3.0;
    const float bezierOffset = 0.5 * baseSize;
    const float maxInternalOffset = (.25 * source - .5 * center + .25 * sink1).length();
    const float centerStretchFactor = (maxInternalOffset + bezierOffset) / bezierOffset;


    ofVec2f focusPt = isSourceSpline ? ofVec2f(baseSize, 1) : ofVec2f(0, -2);
    float fromFocusAngle = M_PI * (isSourceSpline ? (1.0 + t / 3.0) : ((1.0 + t) / 3.0));
    ofVec2f toPtVector(cosf(fromFocusAngle), sinf(fromFocusAngle));

    float offset = (offsetIndex == 2) ? (.5 * baseSize) : baseSize;
    ofVec2f xy = focusPt + offset * toPtVector;

    if(offsetIndex == 0) {
        //project point on base spline
        ofVec2f projBase = isSourceSegment ? ofVec2f(0,1) : ofVec2f(halfBaseSize, -.5);
        xy = dot(xy, projBase) * projBase;
    }

    //in case we are dealing with the center point:
    if(offsetIndex == -1)
        xy = ofVec2f(0,0);
    
    const ofVec2f cornerTL = source + (sink1 - sink2);
    const ofVec2f cornerTR = source + (sink2 - sink1);
    const ofVec2f cornerB = sink1 + (sink2 - source);

    ofVec2f vecSource = (center - source).normalize();
    ofVec2f vecSink1 = (sink1 - center).normalize();
    ofVec2f vecSink2 = (sink2 - center).normalize();
 
    float traversalDistance = 2. * (center - source).length();
 
    float projSource = dot(xy - source, vecSource);
    float projSink1 = dot(xy - sink1, vecSink1);
    float projSink2 = dot(xy - sink2, vecSink2);
 
    float orSource = cross(xy - source, vecSource);
    float orSink1 = cross(xy - sink1, vecSink1);
    float orSink2 = cross(xy - sink2, vecSink2);
 
    float val1 = projSource / traversalDistance;
    float val2 = 1.0 + projSink1 / traversalDistance;
    float val3 = 1.0 + projSink2 / traversalDistance;

    float offsetX = 0;
    if(ABS(projSource) < .0001)
        offsetX = val1;
    else if(ABS(projSink1) < .0001)
        offsetX = val2;
    else if(ABS(projSink2) < .0001)
        offsetX = val3;
    else {
        float power = 2.0;
        float weight1 = powf(1.0 / ABS(projSource), power);
        float weight2 = powf(1.0 / ABS(projSink1), power);
        float weight3 = powf(1.0 / ABS(projSink2), power);
        float sumWeight = weight1 + weight2 + weight3;
    
        offsetX = (weight1 / sumWeight) * val1
                    + (weight2 / sumWeight) * val2
                    + (weight3 / sumWeight) * val3;
    }
 
 
  ofVec2f to = xy - focusPt;
  float toDist = to.length();
  to /= toDist;
 
  float dist = MAX(0.0, toDist - bezierOffset);
 
  float maxAng = M_PI / 6.;
 
  float angle = acos(dot(to, (center - focusPt).normalize()));
  float maxOffset = baseSize / cos(M_PI / 6.0 - angle) - bezierOffset;
 
  float circDistFrac = dist / (baseSize - bezierOffset);
  float projDistFrac = dist / maxOffset;
 
 
  float angleFrac = 1. - angle / maxAng;
 
 
  float offFactor = pow(projDistFrac, 2.0 + abs(angleFrac) * projDistFrac);
  float offsetY = (1. - offFactor) * circDistFrac + offFactor * projDistFrac;
  offsetY = 1. - offsetY;

  if(isnan(offsetX) || isnan(offsetY))
      cout << "OFFSET VALUE is NaN" << endl;

  return ofVec2f(offsetX - .5, offsetY);
}