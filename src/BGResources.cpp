#include <string>
#import "BGResources.h"
#include "json.h"

#define DATA_FILE_LOC "data/output/data.json"

BGResources& bgResources = BGResources::Instance();

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
    //loadImages();
}

BGResources::~BGResources() {

}

int BGResources::getImageCount() {
    return mImages.size();
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

BGColorSetting* BGResources::getColorSetting(const char* key) {
    
}

BGImageSetting* BGResources::getImageSetting(const char* key) {

}

BGIntegerSetting* BGResources::getIntegerSetting(const char* key) {

}

BGFloatSetting* BGResources::getFloatSetting(const char* key) {

}

ofImage* BGResources::getImageReference(int imageIndex) {
    int n = getImageCount();
    int idx = max(0, min(n - 1, imageIndex));
    return &mImages[idx].image;
}

BGStyle* BGResources::getCurrentStyle() {
    currentStyleIndex = max(0, min(NUM_STYLES - 1, currentStyleIndex));
    return mStyles + currentStyleIndex;
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

bool writeJSONToFile(Json::Value& root, const char* filename) {
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
        styleData[mStyles[i].someNumber.name] = mStyles[i].someNumber.value;
        styleData[mStyles[i].boogerColor.name] = colorToString(mStyles[i].boogerColor.value);
        stylesData.append(styleData);
    }

    root["images"] = imagesData;
    root["styles"] = stylesData;

    if(!writeJSONToFile(root, DATA_FILE_LOC))
        cout << "ERROR: COULD NOT WRITE STYLES.JSON" << endl;
}