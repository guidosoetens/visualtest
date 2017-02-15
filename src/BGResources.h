#ifndef BGRESOURCES_H
#define BGRESOURCES_H

#include "ofMain.h"

typedef enum {
    NetworkColorKey = 0
} BGResourceKey;

typedef struct {
    string name;
    ofColor value;
} BGColorSetting;

typedef struct {
    string name;
    int value;
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
    int textureIndex;
    string filepath;
    ofImage image;
} BGImageResource;

typedef struct {
    std::map<BGResourceKey, BGColorSetting> colors;
    std::map<BGResourceKey, BGImageSetting> images;
    std::map<BGResourceKey, BGIntegerSetting> integers;
    std::map<BGResourceKey, BGFloatSetting> floats;
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

        BGColorSetting* getColorSetting(const char* key);
        BGImageSetting* getImageSetting(const char* key);
        BGIntegerSetting* getIntegerSetting(const char* key);
        BGFloatSetting* getFloatSetting(const char* key);

    private:
        vector<BGImageResource> mImages;
        BGStyle mStyles[NUM_STYLES];

        void loadImages();
};

extern BGResources& bgResources;

#endif //BGMENU_H
