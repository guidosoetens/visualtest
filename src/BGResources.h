#ifndef BGRESOURCES_H
#define BGRESOURCES_H

#include "ofMain.h"

typedef enum {

    //colors:
    BGResourceKey_Colors = 0,
    NetworkColorKey,
    NetworkDarkColorKey,
    NetworkLightColorKey,
    BackgroundColorKey,
    BackgroundColor2Key,
    BackgroundColor3Key,
    BackgroundColor4Key,
    BackgroundBorderColorKey,
    ObstacleDarkColorKey,
    ObstacleLightColorKey,
    ObstacleHighlightColorKey,

    //images:
    BGResourceKey_Images,
    BackgroundImageKey,
    ObstacleImageKey,
    ObstacleImage2Key,
    ObstacleImage3Key,
    ObstacleImage4Key,
    
    //integers:
    BGResourceKey_Integers,
    BackgroundModeKey,
    BorderRepsUKey,
    BorderRepsVKey,
    FooIntegerKey,

    //floats:
    BGResourceKey_Floats,
    BackgroundZoomKey,
    BackgroundWeightKey,
    BackgroundCellHueShiftKey,
    EntranceHueShiftKey,
    FooFloatKey,
    AnotherFloat,

    //misc.
    BGResourceKeyCount
} BGResourceKey;

const static string keysList[BGResourceKeyCount] = {
    "[COLORS]",
    "networkColor",
    "networkDarkColor",
    "networkLightColor",
    "backgroundColor",
    "backgroundColor2",
    "backgroundColor3",
    "backgroundColor4",
    "backgroundBorderColor",
    "obstacleDark",
    "obstacleLight",
    "obstacleHighlight",

    "[IMAGES]",
    "backgroundImage",
    "obstacleImage",
    "obstacleImage2",
    "obstacleImage3",
    "obstacleImage4",

    "[INTEGERS]",
    "backgroundMode",
    "borderRepsU",
    "borderRepsV",
    "fooInteger",

    "[FLOATS]",
    "bgZoom",
    "bgWeight",
    "bgCellHueShift",
    "entranceHueShift",
    "fooFloat",
    "anotherFloat",
};

const string keyToString(BGResourceKey key);

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

#define NUM_STYLES 8

class BGResources {
	public:
		BGResources();
		virtual ~BGResources();
        static BGResources& Instance();

        BGStyle* getCurrentStyle();
        BGStyle* getStyle(int styleIndex);

        void reload();
        void save();

        int currentStyleIndex;

        int getImageCount();
        ofImage* getImageReference(BGResourceKey key);
        ofImage* getImageReference(int imageIndex);
        void getImageResources(vector<BGImageResource*> & result);

        BGColorSetting* getColorSetting(BGResourceKey key);
        BGImageSetting* getImageSetting(BGResourceKey key);
        BGIntegerSetting* getIntegerSetting(BGResourceKey key);
        BGFloatSetting* getFloatSetting(BGResourceKey key);

    private:
        vector<BGImageResource> mImages;
        BGStyle mStyles[NUM_STYLES];

        void loadImages();
};

extern BGResources& bgResources;

#endif //BGMENU_H
