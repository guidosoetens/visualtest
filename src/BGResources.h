#ifndef BGRESOURCES_H
#define BGRESOURCES_H

#include "ofMain.h"

typedef struct {
    string name;
    ofColor value;
} BGColorSetting;

typedef struct {
    string name;
    int value;
    ofImage* imageReference;
} BGImageSetting;

typedef struct {
    string name;
    int value;
} BGIntegerSetting;

typedef struct {
    string name;
    float value;
} BGFloatSetting;

typedef struct {
    BGColorSetting boogerColor;
    BGImageSetting obstacleBrimImage;
    BGIntegerSetting someNumber;
    BGFloatSetting backgroundScale;
} BGStyle;

#define NUM_STYLES 4

class BGResources {
	public:
		BGResources();
		virtual ~BGResources();
        static BGResources& Instance();

        BGStyle* getCurrentStyle();

        void reload();
        void save();

        int currentStyleIndex;

        int getImageCount();
        ofImage* getImageReference(int imageIndex);

    private:
        vector<ofImage> mImages;
        BGStyle mStyles[NUM_STYLES];
};

extern BGResources& bgResources;

#endif //BGMENU_H
