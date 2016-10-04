#include "BGInternalNode.h"

BGInternalNode::BGInternalNode()
:   lockPosition(false)
{
    //nodeRadius = BRIM_WIDTH + 5;
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
