#ifndef BGINTERNALNODE_H
#define BGINTERNALNODE_H

#include "BGNode.h"
#include "ofMain.h"
#include "BGTouchNode.h"

class BGInternalNode : public BGNode {
	public:
		BGInternalNode();
		virtual ~BGInternalNode();

        void pushNeighbor(BGNode * n);

        virtual void render();
        virtual void update(float dt);
        virtual void pushToMesh(ofMesh & mesh);
        
        bool lockPosition;

    private:
        std::vector<BGNode*> neighbours;
};

#endif //BGINTERNALNODE_H
