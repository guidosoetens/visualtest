#ifndef BGINTERNALNODE_H
#define BGINTERNALNODE_H

#include "BGNode.h"
#include "ofMain.h"
#include "BGTouchNode.h"

class BGInternalNode : public BGNode {
	public:
		BGInternalNode();
		virtual ~BGInternalNode();

        void setNeighbours(BGNode * n1, BGNode * n2, BGNode * n3);
        void renderGradient();

        virtual void render();
        virtual void update(float dt);

        virtual void pushToMesh(ofMesh & mesh);

    private:
        BGNode* neighbours[3];
        void renderGradientBetween(ofVec2f p1, ofVec2f p2);
};

#endif //BGINTERNALNODE_H
