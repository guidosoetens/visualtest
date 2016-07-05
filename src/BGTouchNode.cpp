#include "BGTouchNode.h"

BGTouchNode::BGTouchNode(float radius)
:   hasInput(false)
{
    nodeRadius = radius;
}

BGTouchNode::~BGTouchNode() {

}

void BGTouchNode::setNeighbour(BGNode* neighbor) {
    neighbours.clear();
    neighbours.push_back(neighbor);
}

void BGTouchNode::render() {
    ofSetColor(0, 255, 0, 50);
    ofCircle(position, nodeRadius);
}

void BGTouchNode::update(float dt) {

}