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

}

BGResources::~BGResources() {

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
    Json::Value root;
    if(!readJSONFromFile(root, DATA_FILE_LOC))
         cout << "ERROR: COULD NOT READ STYLES.JSON" << endl;
}

void BGResources::save() {
    
    Json::Value root;
    Json::Value stylesData;
    for(int i=0; i<NUM_STYLES; ++i) {
        Json::Value styleData;
        styleData["obstacleTexture"] = 0;
        styleData["boogerColor"] = colorToString(mStyles[i].boogerColor);
        stylesData.append(styleData);
    }

    root["styles"] = stylesData;

    if(!writeJSONToFile(root, DATA_FILE_LOC))
        cout << "ERROR: COULD NOT WRITE STYLES.JSON" << endl;
}