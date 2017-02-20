#import "BGImagePicker.h"

#define TILE_WIDTH 100
#define MARGIN_LEFT 50
#define MARGIN_TOP 50

BGImagePicker::BGImagePicker() {

}

BGImagePicker::~BGImagePicker(){

}


void BGImagePicker::render(ofTrueTypeFont & font) {

    vector<BGImageResource*> images;
    bgResources.getImageResources(images);

    ofSetColor(255);

    for(int i=0; i<images.size(); ++i) {
        ofImage* img = bgResources.getImageReference(images[i]->textureIndex);

        int x = MARGIN_LEFT + (i % 2) * TILE_WIDTH;
        int y = MARGIN_TOP + (i / 2) * TILE_WIDTH;

        if(images[i]->textureIndex == mSetting->value) {
            ofRect(x, y, TILE_WIDTH, TILE_WIDTH);
            img->draw(x + 2, y + 2, TILE_WIDTH - 4, TILE_WIDTH - 4);
        }
        else
            img->draw(x, y, TILE_WIDTH, TILE_WIDTH);
    }
}

void BGImagePicker::update(float dt) {

}

void BGImagePicker::mouseDown(ofVec2f p) {
    p.x -= MARGIN_LEFT;
    p.y -= MARGIN_TOP;

    vector<BGImageResource*> images;
    bgResources.getImageResources(images);
    int rows = ((images.size() + 1) / 2);

    if(p.x > 0 && p.x < 2 * TILE_WIDTH && p.y > 0 && p.y < rows * TILE_WIDTH) {
        int idx = 2 * (int)round(p.y / TILE_WIDTH);
        if(p.x > TILE_WIDTH)
            ++idx;

        idx = max(0, min((int)images.size() - 1, idx));
        mSetting->value = images[idx]->textureIndex;
    }
}

void BGImagePicker::mouseMove(ofVec2f p) {

}

void BGImagePicker::mouseUp(ofVec2f p) {

}

void BGImagePicker::open(BGImageSetting* setting) {
    mIsOpen = true;
    mSetting = setting;
}

bool BGImagePicker::isOpen() {
    return mIsOpen;
}

void BGImagePicker::close() {
    mIsOpen = false;
}
