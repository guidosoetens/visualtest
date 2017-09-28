#ifndef BGNETWORK_H
#define BGNETWORK_H

#include "ofMain.h"

#include "BGTouchNode.h"
#include "BGInternalNode.h"

#define TOUCHNODES_COUNT 6
#define INTERNALNODES_COUNT 3

class BGNetwork {
	public:
		BGNetwork();
		virtual ~BGNetwork();

        void setup(int width, int height);
        void render(BGGraphics & graphics);
        void update(float dt);

        bool LockVertices;

        void mouseDown(ofVec2f pos);
        void mouseMove(ofVec2f pos);
        void mouseUp(ofVec2f pos);

    private:

        ofFbo mNetworkTarget;

        vector<BGNode*> allNodes;
		BGTouchNode touchNodes[TOUCHNODES_COUNT];
		BGInternalNode internalNodes[INTERNALNODES_COUNT];

};

#endif //BGNETWORK_H
