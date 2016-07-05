#ifndef BGTOUCHNODE_H
#define BGTOUCHNODE_H

#include "ofMain.h"
#include "BGNode.h"

class BGTouchNode : public BGNode {
	public:
        BGTouchNode() : BGTouchNode(40) {};
		BGTouchNode(float width);
		virtual ~BGTouchNode();

        void render();
        void update(float dt);

		bool hasInput;

        virtual void pushToMesh(ofMesh & mesh);
        void setNeighbour(BGNode* neighbor);
        float nodeRadius;

    private:
        BGNode* mNeighbor;
        void traverseSpline(ofMesh & mesh, int centerOffset, ofVec2f a1, ofVec2f c, ofVec2f a2);

};

#endif //BGTOUCHNODE_H
