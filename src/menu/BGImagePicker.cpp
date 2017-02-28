#import "BGImagePicker.h"
#import "BGUserControl.h"

#define TILE_WIDTH (CONTROL_WIDTH / 2 - 5)
#define MARGIN_LEFT 30
#define MARGIN_TOP 60

BGImagePicker::BGImagePicker() {
    mTotalHeight = TILE_WIDTH;
    scrollOffset = 0;
}

BGImagePicker::~BGImagePicker(){

}

float BGImagePicker::getTotalHeight() {
    return mTotalHeight;
}

void BGImagePicker::render(ofTrueTypeFont & font) {

    vector<BGImageResource*> images;
    bgResources.getImageResources(images);

    ofSetColor(255);

    for(int i=0; i<images.size(); ++i) {
        ofImage* img = bgResources.getImageReference(images[i]->textureIndex);

        //calculate the topleft pivot offset of the tile:
        int x = MARGIN_LEFT + (i % 2) * TILE_WIDTH;
        int y = MARGIN_TOP + (i / 2) * TILE_WIDTH;

        //draw bounds around the center image in the menu panel
        if(images[i]->textureIndex == mSetting->value) {
            ofRect(x, y, TILE_WIDTH, TILE_WIDTH);
            ofSetColor(0);
            ofRect(x + 2, y + 2, TILE_WIDTH - 4, TILE_WIDTH - 4);
            ofSetColor(255);
        }

        //draw the actual image itsels:
        img->draw(x + 5, y + 5, TILE_WIDTH - 10, TILE_WIDTH - 10);
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

        int idx = 2 * (int)floor(p.y / TILE_WIDTH);
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

    vector<BGImageResource*> images;
    bgResources.getImageResources(images);
    mTotalHeight = (int)ceil(images.size() / 2.0 + 1) * mTotalHeight;
}

bool BGImagePicker::isOpen() {
    return mIsOpen;
}

void BGImagePicker::close() {
    mIsOpen = false;
}
