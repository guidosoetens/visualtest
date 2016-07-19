#include "BGGraphics.h"

BGGraphics::BGGraphics() {
    mNetworkShader.load("shaders/testShader");
    renderWireframe = false;
    mTimeParameter = 0;
    renderFlow = false;
    depthTest = true;
    maxDepth = 1;
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
    mTimeParameter = fmodf(mTimeParameter + dt / 6.0, 1.0);
}

void BGGraphics::reload() {
    mNetworkShader.load("shaders/testShader");
}

void BGGraphics::renderSeparateNode(ofVec2f position, float nodeRadius, float depth) {
    renderCirclePart(position, nodeRadius, 0, 2 * M_PI, depth);
}

void BGGraphics::renderSingleConnectedNode(ofVec2f position, float nodeRadius, ofVec2f edgePoint, float depth) {

    float innerRadius = nodeRadius - NETWORK_OFFSET;

    ofVec2f to = edgePoint - position;
    float toDist = to.length();
    to /= toDist;
    ofVec2f perp = ofVec2f(-to.y, to.x);

    float depthFactor = depth < .1 ? .5 : -.5;

    if(toDist > innerRadius) {
        
        //perform spline traversal...
        ofVec2f controlPoint = .5 * (position + innerRadius * to) + .5 * edgePoint;
        float angle = acosf(innerRadius / toDist);

        renderCirclePart(position, nodeRadius, atan2f(to.y, to.x) + angle, 2 * (M_PI - angle), 0);

        float toAng = atan2f(to.y, to.x);
        ofVec2f toA1(cosf(toAng + angle), sinf(toAng + angle));
        ofVec2f toA2(cosf(toAng - angle), sinf(toAng - angle));


        ofMesh mesh;

        float facU = innerRadius * cosf(angle);
        float facV = innerRadius * sinf(angle);

        float anchorProj = sqrtf(.5 * innerRadius * innerRadius);
        ofVec2f anchorLeft  = position + innerRadius * toA1;
        ofVec2f anchorRight = position + innerRadius * toA2;

        float innerRadiusFactor = innerRadius / nodeRadius;

        //add first point:
        pushVertex(mesh, anchorLeft.x + NETWORK_OFFSET * toA1.x, anchorLeft.y + NETWORK_OFFSET * toA1.y, 0, toA1.x, toA1.y, 0, depth, 1);
        pushVertex(mesh, anchorLeft.x, anchorLeft.y, 1, 0, 0, 1, depth, innerRadiusFactor);
        pushVertex(mesh, position.x, position.y, 1, 0, 0, 1, depth, 0);
        pushVertex(mesh, anchorRight.x, anchorRight.y, 1, 0, 0, 1, depth, innerRadiusFactor);
        pushVertex(mesh, anchorRight.x + NETWORK_OFFSET * toA2.x, anchorRight.y + NETWORK_OFFSET * toA2.y, 0, toA2.x, toA2.y, 0, depth, 1);

        int samples = 10;
        for(int i=1; i<samples; ++i) {

            float t = i / (float)(samples - 1);

            float localDepth = depth + depthFactor * t;

            ofVec2f pt, normal;
            sampleSpline(anchorLeft, controlPoint, edgePoint, t, pt, normal);
            normal = -normal;

            //project point on skeleton:
            ofVec2f centerSample;
            getIntersection(position, to, pt, normal, centerSample);

            float innerSampleOffset = (pt - centerSample).length();
            float innerSampleOffsetFactor = innerSampleOffset / (innerSampleOffset + NETWORK_OFFSET);

            ofVec2f otherPt = position + reflect(pt - position, to);
            ofVec2f otherNormal = reflect(normal, to);

            pushVertex(mesh, pt.x + NETWORK_OFFSET * normal.x, pt.y + NETWORK_OFFSET * normal.y, 0, normal.x, normal.y, 0, localDepth, 1);
            pushVertex(mesh, pt.x, pt.y, 1, 0, 0, 1, localDepth, innerSampleOffsetFactor);
            pushVertex(mesh, centerSample.x, centerSample.y, 1, 0, 0, 1, localDepth, 0);
            pushVertex(mesh, otherPt.x, otherPt.y, 1, 0, 0, 1, localDepth, innerSampleOffsetFactor);
            pushVertex(mesh, otherPt.x + NETWORK_OFFSET * otherNormal.x, otherPt.y + NETWORK_OFFSET * otherNormal.y, 0, otherNormal.x, otherNormal.y, 0, localDepth, 1);

            if(i > 0) {
                int offset = 5 * i;

                for(int j=0; j<4; ++j) {
                     mesh.addTriangle(offset + j, offset - 5 + j, offset - 4 + j);
                     mesh.addTriangle(offset + j, offset + 1 + j, offset - 4 + j);
                }
            }
        }

        drawMesh(mesh);
    }
    else
        renderSeparateNode(position, nodeRadius, depth);
}

void BGGraphics::renderDoubleConnectedNode(ofVec2f position, ofVec2f startEdgePoint, ofVec2f endEdgePoint, float depth) {

    ofMesh mesh;

    int samples = 10;
    for(int i=0; i<samples; ++i) {

        float t = i / (float)(samples - 1);

        float localDepth = depth - .5 + t;

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

    drawMesh(mesh);
}

void BGGraphics::renderTripleConnectedNode(ofVec2f position, ofVec2f startEdgePoint, ofVec2f endEdgePoint1, ofVec2f endEdgePoint2, float depth) {
 
    ofVec2f orderedPts[3] = {
        startEdgePoint,
        endEdgePoint1,
        endEdgePoint2
    };

    float baseSize = sqrtf(3.0);
    ofVec2f focusPts[3] = {
        ofVec2f(baseSize, 1),
        ofVec2f(0, -2),
        ofVec2f(-baseSize, 1),
    };

    float deltaAngle = M_PI / 3.0;

    float focusAngles[3] = {
        -M_PI,
         deltaAngle,
        -deltaAngle
    };

    ofMesh centerTriangleMesh;

    int halfSamples = 6;
    for(int idx=0; idx<3; ++idx) {

        int prevIdx = idx == 0 ? 2 : (idx - 1);
        int nextIdx = (idx + 1) % 3;

        ofVec2f focus1 = focusPts[idx];
        ofVec2f focus2 = focusPts[prevIdx];

        ofMesh mesh;

        for(int i=0; i<halfSamples; ++i) {

            float t = .5 * i / (float)(halfSamples - 1);

            ofVec2f pt1, normal1, pt2, normal2;
            sampleSpline(orderedPts[idx], position, orderedPts[nextIdx], t, pt1, normal1);
            sampleSpline(orderedPts[idx], position, orderedPts[prevIdx], t, pt2, normal2);
            normal2 = -normal2;

            ofVec2f center = (pt1 + pt2) / 2.0;

            float offAngle1 = focusAngles[idx] + t * deltaAngle;
            ofVec2f offVector1 = ofVec2f(cosf(offAngle1), sinf(offAngle1));
            float offAngle2 = focusAngles[prevIdx] + (1. - t) * deltaAngle;
            ofVec2f offVector2 = ofVec2f(cosf(offAngle2), sinf(offAngle2));

            ofVec2f innerTexPt1 = focus1 + baseSize * offVector1;
            ofVec2f outerTexPt1 = focus1 + .5 * baseSize * offVector1;
            ofVec2f innerTexPt2 = focus2 + baseSize * offVector2;
            ofVec2f outerTexPt2 = focus2 + .5 * baseSize * offVector2;
            ofVec2f centerTexPt = (innerTexPt1 + innerTexPt2) / 2.0;

            pushVertex(mesh, pt1.x + NETWORK_OFFSET * normal1.x, pt1.y + NETWORK_OFFSET * normal1.y, 0, normal1.x, normal1.y, 0, outerTexPt1.x, outerTexPt1.y);
            pushVertex(mesh, pt1.x, pt1.y, 1, 0, 0, 1, innerTexPt1.x, innerTexPt1.y);
            pushVertex(mesh, center.x, center.y, 1, 0, 0, 1, centerTexPt.x, centerTexPt.y);
            pushVertex(mesh, pt2.x, pt2.y, 1, 0, 0, 1, innerTexPt2.x, innerTexPt2.y);
            pushVertex(mesh, pt2.x + NETWORK_OFFSET * normal2.x, pt2.y + NETWORK_OFFSET * normal2.y, 0, normal2.x, normal2.y, 0, outerTexPt2.x, outerTexPt2.y);

            if(i > 0) {
                int offset = 5 * i;
                for(int j=0; j<4; ++j) {
                    mesh.addTriangle(offset + j, offset - 5 + j, offset - 4 + j);
                    mesh.addTriangle(offset + j, offset + 1 + j, offset - 4 + j);
                }
            }

            if(i == halfSamples - 1)
                pushVertex(centerTriangleMesh, pt1.x, pt1.y, 1, 0, 0, 1, innerTexPt1.x, innerTexPt1.y);
        }

        drawMesh(mesh, true);
    }

    drawMesh(centerTriangleMesh, true);
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

void BGGraphics::drawMesh(ofMesh & mesh) {

    drawMesh(mesh, false);
}

void BGGraphics::drawMesh(ofMesh & mesh, bool triangulateFlowTexture) {
    if(depthTest)
        ofEnableDepthTest();
    else 
        ofDisableDepthTest();

    mNetworkShader.begin();
    mNetworkShader.setUniform1i("uTriangulateOffset", triangulateFlowTexture ? 1 : 0);
    mNetworkShader.setUniform1f("uTimeParameter", mTimeParameter);
    mesh.draw();
    mNetworkShader.end();

    ofDisableDepthTest();

    if(renderWireframe)
        mesh.drawWireframe();
}

void BGGraphics::renderCirclePart(ofVec2f position, float nodeRadius, float minAngle, float deltaAngle, float depth) {

    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);

    pushVertex(mesh, position.x, position.y, 1, 0, 0, 1, depth, 0);

    float innerRadius = nodeRadius - NETWORK_OFFSET;

    float internalOffsetY = innerRadius / nodeRadius;

    int samples = (int)(15 * deltaAngle / M_PI);
    samples = max(1, samples);

    for(int i=0; i<samples; ++i) {
        float angle = minAngle + deltaAngle * i / (float)(samples - 1);
        ofVec2f to(cosf(angle), sinf(angle));

        ofVec2f p1 = position + to * innerRadius;
        ofVec2f p2 = position + to * nodeRadius;

        //pushVertex(ofMesh & mesh, float x, float y, float z, float nx, float ny, float nz, float offsetX, float offsetY) 
        pushVertex(mesh, p1.x, p1.y, 1, 0, 0, 1, depth, internalOffsetY);
        pushVertex(mesh, p2.x, p2.y, 0, to.x, to.y, 0, depth, 1);

        if(i > 0) {
            int offset = 1 + 2 * i;
            mesh.addTriangle(0, offset - 2, offset);
            mesh.addTriangle(offset - 2, offset - 1, offset);
            mesh.addTriangle(offset - 1, offset, offset + 1);
        }
    }

    drawMesh(mesh);
}