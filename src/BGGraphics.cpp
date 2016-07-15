#include "BGNode.h"

BGGraphics::BGGraphics() {
    mBezierShader.load("shaders/superBezierShader");
    
    mTimeParameter = 0;
}

BGGraphics::~BGGraphics() {

}

float BGGraphics::dot(ofVec2f a, ofVec2f b) {
    return a.x * b.x + a.y * b.y;
}

float BGGraphics::cross(ofVec2f a, ofVec2f b) {
    return a.x * b.y - a.y * b.x;
}

bool BGGraphics::getIntersection(ofVec2f p, ofVec2f r, ofVec2f q, ofVec2f s, ofVec2f & result) {
    float div = cross(r, s);
    if(ABS(div) < .00001)
        return false;
        
    float u = cross(q - p, r) / div;
    result = q + u * s;
    return true;
}

void BGGraphics::update(float dt) {
    mTimeParameter = fmodf(mTimeParameter + dt / 6.0, 1.0);
}

void BGGraphics::reload() {
    mBezierShader.load("shaders/superBezierShader");
}

void BGGraphics::renderSeparateNode(ofVec2f position, float nodeRadius) {
    
}

void BGGraphics::renderSingleConnectedNode(ofVec2f position, float nodeRadius, ofVec2f edgePt) {
    
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

void BGGraphics::renderInternalNodePart(ofVec2f position, ofVec2f edgePt1, ofVec2f edgePt2) {
    
    //NETWORK_OFFSET
    
    ofVec2f to1 = edgePt1 - position;
    to1.normalize();
    
    ofVec2f to2 = edgePt2 - position;
    to2.normalize();
    
    ofVec2f normal1(-to1.y, to1.x);
    ofVec2f normal2(to2.y, -to2.x);
    
    //sample center sample of quadratic bezier:
    ofVec2f centerSample = .25 * edgePt1 + .5 * position + .25 * edgePt2;
    ofVec2f centerTangent = (edgePt2 - edgePt1).normalize();
    ofVec2f centerNormal(centerTangent.y, -centerTangent.x);
    
    bool extendedAng = cross(to1, to2) < 0.0;
     
    mBezierShader.begin();
    mBezierShader.setUniform2f("uResolution", 1024, 768);
    mBezierShader.setUniform1f("uNetworkOffset", NETWORK_OFFSET);
    mBezierShader.setUniform1f("uTimeParameter", mTimeParameter);
    mBezierShader.setUniform2f("uAnchor1", edgePt1.x, edgePt1.y);
    mBezierShader.setUniform2f("uController", position.x, position.y);
    mBezierShader.setUniform2f("uAnchor2", edgePt2.x, edgePt2.y);
    
    //construct mesh...
    
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    
    if(extendedAng) {
        //angle is greater than pi, so construct the mesh accordingly:
    }
    else {
        mesh.addVertex(ofVec3f(position.x, position.y, 0));
        mesh.addVertex(ofVec3f(edgePt1.x, edgePt1.y, 0));
        mesh.addVertex(ofVec3f(edgePt1.x + NETWORK_OFFSET * normal1.x, edgePt1.y + NETWORK_OFFSET * normal1.y, 0));
        mesh.addVertex(ofVec3f(centerSample.x + NETWORK_OFFSET * centerNormal.x, centerSample.y + NETWORK_OFFSET * centerNormal.y, 0));
        mesh.addVertex(ofVec3f(edgePt2.x + NETWORK_OFFSET * normal2.x, edgePt2.y + NETWORK_OFFSET * normal2.y, 0));
        mesh.addVertex(ofVec3f(edgePt2.x, edgePt2.y, 0));
    }
    
    for(int i=0; i<mesh.getVertices().size(); ++i) {
        
        ofVec3f vertex = mesh.getVertex(i);
        
        //add tex coord proportional to vertex:
        ofVec2f baryCoords = getBarycentricCoords(ofVec2f(vertex.x, vertex.y), position, edgePt1, edgePt2);
        
        //cout << "COORDS: " << baryCoords.x << ", " << baryCoords.y << endl;
        
        mesh.addTexCoord(baryCoords);
    }
    
    mesh.draw();
    
    mBezierShader.end();
    
}