#ifndef BGNODE_H
#define BGNODE_H

#include "ofMain.h"

#define EDGE_WIDTH 30.0

class BGNode {
	public:
		BGNode();
		virtual ~BGNode();

        virtual void render() = 0;
        virtual void update(float dt) = 0;

        ofVec2f position;
        int traversalId;

        virtual void pushToMesh(ofMesh & mesh);
};

#endif //BGNODE_H
