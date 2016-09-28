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

        void setNeighbour(BGNode* neighbor);
};

#endif //BGTOUCHNODE_H
