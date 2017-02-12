#ifndef BGRESOURCES_H
#define BGRESOURCES_H

#include "ofMain.h"

typedef struct {
    ofImage* obstacleTexture;
    ofColor boogerColor;
} BGStyle;

#define NUM_STYLES 4

class BGResources {
	public:
		BGResources();
		virtual ~BGResources();
        static BGResources& Instance();

        BGStyle* currentStyle;

        void reload();
        void save();
    private:

        BGStyle mStyles[NUM_STYLES];
};

extern BGResources& bgResources;

#endif //BGMENU_H
