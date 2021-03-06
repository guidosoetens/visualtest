#include "BGNode.h"

BGNode::BGNode()
:   position(0, 0)
,   nodeRadius(30)
,   mHasSurface(false)
,   mSurfaceNormal(1,0)
{

}

BGNode::~BGNode() {

}

void BGNode::bindSurface(ofVec2f surfaceNormal) {
    mHasSurface = true;
    mSurfaceNormal = surfaceNormal;
}

int BGNode::getNeighbourCount() {
    return neighbours.size();
}

void BGNode::drawFace(ofShader & mEyeShader) {

/*
    int n = neighbours.size();
    ofVec2f toVector(1,0);
    if(n > 0)
        toVector = (neighbours[0]->position - position).normalize();


    ofVec2f perpVec(-toVector.y, toVector.x);
*/

    ofSetColor(255);

    float offset = .05 * nodeRadius;
    float perpOffset = .6 * nodeRadius;
    float eyeRadius = .6 * nodeRadius;

    ofVec2f toVector = mSurfaceNormal;
    ofVec2f perpVec(-toVector.y, toVector.x);

    ofVec2f goalPos = position;
    if(neighbours.size() > 0)
        goalPos = neighbours[0]->position;

    float texFactor = .6 * nodeRadius;

    for(int i=0; i<2; ++i) {

        int factor = i == 0 ? 1 : -1;
        ofVec2f pos = position + toVector * offset + factor * perpVec * perpOffset;

        ofMesh eyeMesh;
        eyeMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

        for(int j=0; j<4; ++j) {

            ofVec2f texPos(j % 2, j / 2);

            ofVec2f vPos = pos + (texPos.x - .5) * texFactor * perpVec + (texPos.y - .5) * texFactor * toVector;

            eyeMesh.addVertex(ofVec3f(vPos.x, vPos.y, 0));
            eyeMesh.addTexCoord(texPos);
        }

        ofVec2f goalPos = position;
        if(neighbours.size() > 0)
            goalPos = neighbours[0]->position;

        ofVec2f toGoal = goalPos - pos;
        float toGoalLength = toGoal.length();
        ofVec2f pupilPos = pos;
        if(toGoalLength > 0.01)
            pupilPos = pos + .25 * eyeRadius / toGoalLength * toGoal;

        mEyeShader.begin();
        mEyeShader.setUniform2f("uCenter", pos.x, pos.y);
        mEyeShader.setUniform2f("uFocusPoint", goalPos.x, goalPos.y);
        mEyeShader.setUniform2f("uPupilLoc", pupilPos.x, pupilPos.y);
        mEyeShader.setUniform1i("uFlipHorizontally", i > 0);
        
        eyeMesh.draw();
        mEyeShader.end();
    }

    ofMesh mouthMesh;
    mouthMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    for(int i=0; i<21; ++i) {
        float t = i / 20.0;
        float x = .6 * (t - .5) * nodeRadius;
        float yVar = 2 * abs(t - .5);
        float y = (-yVar * yVar * (1 - yVar) * .6 - .3) * nodeRadius;

        for(int it=0; it<2; ++it) {
            mouthMesh.addVertex(position + y * toVector + x * perpVec);
            mouthMesh.addColor(ofColor(0));
            y -= 3;
        }

    }

    mouthMesh.draw();
}

ofRectangle 
BGNode::traverseGetBounds() {
    return traverseGetBounds(NULL);
}

ofRectangle 
BGNode::traverseGetBounds(BGNode* parentNode) {

    ofRectangle bounds(position.x - nodeRadius, position.y - nodeRadius, 2 * nodeRadius, 2 * nodeRadius);

    int n = neighbours.size();
    for(int i=0; i<n; ++i) {
        BGNode* neighbour = neighbours[i];
        if(neighbour != parentNode)
            bounds.growToInclude(neighbour->traverseGetBounds(this));
    }

    return bounds;
}

void BGNode::traverseBeginDraw(BGGraphics & graphics) {
    traverseBeginDraw(graphics, NULL);
}

ofVec2f BGNode::calculateEdgePoint(int neighbourIndex) {

    BGNode* n = neighbours[neighbourIndex];
    //return (position + n->position) / 2.0;

    ofVec2f to = n->position - position;
    float toLength = to.length();

    float r1 = getNeighbourCount() <= 1 ? nodeRadius : 0.0;
    float r2 = n->getNeighbourCount() <= 1 ? n->nodeRadius : 0.0;

    float offset = r1 + .5 * (toLength - r1 - r2);
    return position + to * (offset / toLength);
}

void BGNode::traverseBeginDraw(BGGraphics & graphics, BGNode* parentNode) {

    //gather meshes:
    mNodeMesh.clear();

     int n = neighbours.size();
     
     if(n == 0)
        graphics.pushSeparateNode(mNodeMesh, position, nodeRadius);
     else if(n == 1) {
         ofVec2f edgePt = calculateEdgePoint(0);
         graphics.pushSingleConnectedNode(mNodeMesh, position, nodeRadius, edgePt, parentNode == NULL);
     }
     else if(n == 2 || n == 3) {

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

        int startIndex = 0;
        for(int i=0; i<n; ++i) {
            if(neighbours[indices[i]] == parentNode)
                startIndex = i;
        }

        ofVec2f n1 = calculateEdgePoint(indices[startIndex]);// (position + neighbours[indices[startIndex]]->position) / 2.0;
        ofVec2f n2 = calculateEdgePoint(indices[(startIndex + 1) % n]);// (position + neighbours[indices[(startIndex + 1) % n]]->position) / 2.0;

        if(n == 2) {
            graphics.pushDoubleConnectedNode(mNodeMesh, position, n1, n2);
        }
        else { //n == 3
            ofVec2f n3 = calculateEdgePoint(indices[(startIndex + 2) % n]);//(position + neighbours[indices[(startIndex + 2) % n]]->position) / 2.0;
            graphics.pushTripleConnectedNode(mNodeMesh, position, n1, n2, n3);
        }
     }
    //traverse draw:
    for(int i=0; i<n; ++i) {
        BGNode* neighbour = neighbours[i];
        if(neighbour != parentNode)
            neighbour->traverseBeginDraw(graphics, this);
    }
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
    graphics.maxDepth = traverseGetDepth(this, 0.0);
    traverseDraw(graphics, NULL, 0);
}

void BGNode::traverseDraw(BGGraphics & graphics, BGNode* parentNode, float depth) {

    int n = neighbours.size();

    graphics.drawMesh(mNodeMesh, position, nodeRadius, depth, n <= 1, mHasSurface, mSurfaceNormal);

    //traverse draw:
    for(int i=0; i<n; ++i) {
        BGNode* neighbour = neighbours[i];
        if(neighbour != parentNode)
            neighbour->traverseDraw(graphics, this, depth + 1);
    }
}