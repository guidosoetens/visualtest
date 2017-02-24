#include <string>
#import "BGResources.h"
#include "json.h"
#include <sys/stat.h>

#define DATA_FILE_LOC "data/output/data.json"
#define BACKUP_DATA_FILE_LOC(i) (string("data/output/backup/data_") + i + ".json")

BGResources& bgResources = BGResources::Instance();

const string keyToString(BGResourceKey key) {

    return keysList[(int)key];

    // switch(key) {

    //     //colors:
    //     case NetworkColorKey:
    //         return "networkColor";
    //     case NetworkDarkColorKey:
    //         return "networkDarkColor";
    //     case NetworkLightColorKey:
    //         return "networkLightColor";

    //     //images:
    //     case BackgroundImageKey:
    //         return "backgroundImage";
    //     case ObstacleImageKey:
    //         return "obstacleImage";

    //     //integers:
    //     case FooIntegerKey:
    //         return "fooInteger";

    //     //floats:
    //     case FooFloatKey:
    //         return "fooFloat";
    // }

    // return "<NULL>";
};

ofColor parseColor(string hexString) {
    int hex = ofHexToInt(hexString);
    ofColor clr;
    clr.r = ((hex >> 16) & 0xFF);
    clr.g = ((hex >> 8) & 0xFF);
    clr.b = ((hex) & 0xFF);   
    return clr;
}

void reverse(char str[], int length)
{
    int start = 0;
    int end = length -1;
    while (start < end)
    {
        swap(*(str+start), *(str+end));
        start++;
        end--;
    }
}

char* itoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;
 
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str, i);
 
    return str;
}

string colorToString(ofColor color) {

    static char buffer[10];
    string result = itoa(color.getHex(), buffer, 16);
    while(result.size() < 6)
        result = string("0") + result;
    return result;
}

BGResources&
BGResources::Instance() {
    static BGResources instance;
    return instance;
}

BGResources::BGResources() {
    
    //push default styles:
    for(int i=0; i<NUM_STYLES; ++i) {

        BGStyle* style = mStyles + i;

        //load colors:
        for(int i=(BGResourceKey_Colors + 1); i<BGResourceKey_Images; ++i) {
            BGResourceKey key = (BGResourceKey)i;
            string keyStr = keyToString(key);
            BGColorSetting setting = { keyStr, ofColor(255, 255, 255) };
            style->colors[key] = setting;
        }

        //load images:
        for(int i=(BGResourceKey_Images + 1); i<BGResourceKey_Integers; ++i) {
            BGResourceKey key = (BGResourceKey)i;
            string keyStr = keyToString(key);
            BGImageSetting setting = { keyStr, 0 };
            style->images[key] = setting;
        }

        //load integers:
        for(int i=(BGResourceKey_Integers + 1); i<BGResourceKey_Floats; ++i) {
            BGResourceKey key = (BGResourceKey)i;
            string keyStr = keyToString(key);
            BGIntegerSetting setting = { keyStr, 0 };
            style->integers[key] = setting;
        }

        //load floats:
        for(int i=(BGResourceKey_Floats + 1); i<BGResourceKeyCount; ++i) {
            BGResourceKey key = (BGResourceKey)i;
            string keyStr = keyToString(key);
            BGFloatSetting setting = { keyStr, 0 };
            style->floats[key] = setting;
        }
    }
}

BGResources::~BGResources() {

}

int BGResources::getImageCount() {
    return mImages.size();
}

void 
BGResources::getImageResources(vector<BGImageResource*> & result) {
    for(int i=0; i<mImages.size(); ++i) 
        result.push_back(&mImages[i]);
}

void BGResources::loadImages() {

    mImages.clear();

    ofDirectory dir("images");
    dir.listDir();

    int numFiles = dir.size();
    for(int i=0; i<numFiles; ++i) {
        mImages.push_back(BGImageResource());
        string path = dir.getPath(i);
        mImages[i].filepath = path;
        mImages[i].image.loadImage(path);
        mImages[i].textureIndex = -1;
    }
}

BGColorSetting* BGResources::getColorSetting(BGResourceKey key) {
    BGStyle* style = getCurrentStyle();
    return &style->colors[key];
}

BGImageSetting* BGResources::getImageSetting(BGResourceKey key) {
    BGStyle* style = getCurrentStyle();
    return &style->images[key];
}

BGIntegerSetting* BGResources::getIntegerSetting(BGResourceKey key) {
    BGStyle* style = getCurrentStyle();
    return &style->integers[key];
}

BGFloatSetting* BGResources::getFloatSetting(BGResourceKey key) {
    BGStyle* style = getCurrentStyle();
    return &style->floats[key];
}

 ofImage* BGResources::getImageReference(BGResourceKey key) {
     BGImageSetting* setting = getImageSetting(key);
     return getImageReference(setting->value);
 }

ofImage* BGResources::getImageReference(int imageIndex) {
    int n = getImageCount();
    int idx = max(0, min(n - 1, imageIndex));
    return &mImages[idx].image;
}

BGStyle* BGResources::getCurrentStyle() {
    return mStyles + max(0, min(NUM_STYLES - 1, currentStyleIndex));
}

BGStyle* BGResources::getStyle(int styleIndex) {
    return mStyles + max(0, min(NUM_STYLES - 1, styleIndex));
}

bool readJSONFromFile(Json::Value& root, const char* filename) {

    //read file to string:
    string raw_json = "";
    ifstream t(filename);
    if( t.is_open() ) {
        t.seekg(0, ios::end);
        raw_json.reserve(t.tellg());
        t.seekg(0, ios::beg);
        raw_json.assign((istreambuf_iterator<char>(t)),
                istreambuf_iterator<char>());
    }
    else
        return false;

    Json::Reader reader;
    return reader.parse(raw_json, root);
}

inline bool fexists (const char* filename) {
  struct stat buffer;   
  return (stat(filename, &buffer) == 0); 
}

char* lltoa(long long val, int base){

    static char buf[64] = {0};

    int i = 62;
    int sign = (val < 0);
    if(sign) val = -val;

    if(val == 0) return "0";

    for(; val && i ; --i, val /= base) {
        buf[i] = "0123456789abcdef"[val % base];
    }

    if(sign) {
        buf[i--] = '-';
    }
    return &buf[i+1];

}

bool writeJSONToFile(Json::Value& root, const char* filename) {


    if(fexists(filename)) {
        //create backup:
        long long currenttime = (long long)time(NULL);
        system((string("mv ") + filename + " " + BACKUP_DATA_FILE_LOC(lltoa(currenttime, 10))).c_str());
    }

    /*
    int prefixSize = TIMESTAMP_PREFIX.size();

    std::size_t pos = str.find("\n");
    std::string valStr = str.substr(prefixSize, pos - prefixSize);

    long long stamp_time = atoll(valStr.c_str());
    long long currenttime = (long long)time(NULL);
    long long delta_time = currenttime - stamp_time;

    return delta_time > EXPIRATION_INTERVAL_IN_SECONDS;
    */

    string s = root.toStyledString();
    FILE* file = fopen(filename, "w");
    if( file != NULL ) {
        fputs(s.c_str(), file);
        fclose(file);
        return true;
    }
    else
        return false;
}

void BGResources::reload() {

    loadImages();

    Json::Value root;
    if(readJSONFromFile(root, DATA_FILE_LOC)) {

        //process data...
        int nextFreeIndex = 0;
        Json::Value images = root["images"];
        for(int i=0; i<images.size(); ++i) {
            Json::Value image = images[i];
            int index = image["index"].asInt();
            string path = image["filepath"].asString();

            for(int j=0; j<mImages.size(); ++j) {
                if(path.compare(mImages[j].filepath) == 0) {
                    mImages[j].textureIndex = index;
                    nextFreeIndex = max(nextFreeIndex, index + 1);
                }
            }
        }

        //distribute indices over remaining images:
        for(int i=0; i<mImages.size(); ++i) {
            if(mImages[i].textureIndex < 0)
                mImages[i].textureIndex = nextFreeIndex++;
        }

        //load in styles:
        Json::Value styles = root["styles"];
        int n = min(NUM_STYLES, (int)styles.size());
        for(int i=0; i<n; ++i) {
            Json::Value styleData = styles[i];
            BGStyle* style = mStyles + i;

            //load colors:
            for(int i=(BGResourceKey_Colors + 1); i<BGResourceKey_Images; ++i) {
                BGResourceKey key = (BGResourceKey)i;
                string keyStr = keyToString(key);
                BGColorSetting setting = { keyStr, parseColor(styleData[keyStr].asString()) };
                style->colors[key] = setting;
            }

            //load images:
            for(int i=(BGResourceKey_Images + 1); i<BGResourceKey_Integers; ++i) {
                BGResourceKey key = (BGResourceKey)i;
                string keyStr = keyToString(key);
                BGImageSetting setting = { keyStr, styleData[keyStr].asInt() };
                style->images[key] = setting;
            }

            //load integers:
            for(int i=(BGResourceKey_Integers + 1); i<BGResourceKey_Floats; ++i) {
                BGResourceKey key = (BGResourceKey)i;
                string keyStr = keyToString(key);
                BGIntegerSetting setting = { keyStr, styleData[keyStr].asInt() };
                style->integers[key] = setting;
            }

            //load floats:
            for(int i=(BGResourceKey_Floats + 1); i<BGResourceKeyCount; ++i) {
                BGResourceKey key = (BGResourceKey)i;
                string keyStr = keyToString(key);
                BGFloatSetting setting = { keyStr, styleData[keyStr].asFloat() };
                style->floats[key] = setting;
            }
        }

    }
    else {
         cout << "ERROR: COULD NOT READ STYLES.JSON" << endl;
    }
}

void BGResources::save() {
    
    Json::Value root;
    Json::Value imagesData;
    Json::Value stylesData;

    for(int i=0; i<mImages.size(); ++i) {
        Json::Value imageData;
        imageData["index"] = mImages[i].textureIndex;
        imageData["filepath"] = mImages[i].filepath;
        imagesData.append(imageData);
    }

    for(int i=0; i<NUM_STYLES; ++i) {
        Json::Value styleData;

        BGStyle* style = mStyles + i;

        for (map<BGResourceKey, BGColorSetting>::iterator it = style->colors.begin(); it != style->colors.end(); ++it)
            styleData[it->second.name] = colorToString(it->second.value);

        for (map<BGResourceKey, BGImageSetting>::iterator it = style->images.begin(); it != style->images.end(); ++it)
            styleData[it->second.name] = it->second.value;

        for (map<BGResourceKey, BGIntegerSetting>::iterator it = style->integers.begin(); it != style->integers.end(); ++it)
            styleData[it->second.name] = it->second.value;

        for (map<BGResourceKey, BGFloatSetting>::iterator it = style->floats.begin(); it != style->floats.end(); ++it)
            styleData[it->second.name] = it->second.value;

        stylesData.append(styleData);
    }

    root["images"] = imagesData;
    root["styles"] = stylesData;

    if(!writeJSONToFile(root, DATA_FILE_LOC))
        cout << "ERROR: COULD NOT WRITE STYLES.JSON" << endl;
}