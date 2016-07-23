#include "BGNode.h"

BGNode::BGNode()
:   position(0, 0)
,   nodeRadius(30)
{

}

BGNode::~BGNode() {

}

void BGNode::traverseBeginDraw(BGGraphics & graphics) {
    traverseBeginDraw(graphics, NULL);
}

void BGNode::traverseBeginDraw(BGGraphics & graphics, BGNode* parentNode) {

    //gather meshes:
    mNodeMesh.clear();

     int n = neighbours.size();
     
     if(n == 0)
        graphics.pushSeparateNode(mNodeMesh, position, nodeRadius);
     else if(n == 1) 
        graphics.pushSingleConnectedNode(mNodeMesh, position, nodeRadius, (position + neighbours[0]->position) / 2.0, parentNode == NULL);
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

        ofVec2f n1 = (position + neighbours[indices[startIndex]]->position) / 2.0;
        ofVec2f n2 = (position + neighbours[indices[(startIndex + 1) % n]]->position) / 2.0;

        if(n == 2) {
            graphics.pushDoubleConnectedNode(mNodeMesh, position, n1, n2);
        }
        else { //n == 3
            ofVec2f n3 = (position + neighbours[indices[(startIndex + 2) % n]]->position) / 2.0;
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
    
        graphics.drawMesh(mNodeMesh, depth);

        int n = neighbours.size();
        /*
     
     if(n == 0)
        graphics.renderSeparateNode(position, nodeRadius, depth);
     else if(n == 1) 
        graphics.renderSingleConnectedNode(position, nodeRadius, (position + neighbours[0]->position) / 2.0, depth);
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

        ofVec2f n1 = (position + neighbours[indices[startIndex]]->position) / 2.0;
        ofVec2f n2 = (position + neighbours[indices[(startIndex + 1) % n]]->position) / 2.0;

        if(n == 2) {
            graphics.renderDoubleConnectedNode(position, n1, n2, depth);
        }
        else { //n == 3
            ofVec2f n3 = (position + neighbours[indices[(startIndex + 2) % n]]->position) / 2.0;
            graphics.renderTripleConnectedNode(position, n1, n2, n3, depth);
        }
     }
     */

    //traverse draw:
    for(int i=0; i<n; ++i) {
        BGNode* neighbour = neighbours[i];
        if(neighbour != parentNode)
            neighbour->traverseDraw(graphics, this, depth + 1);
    }
}