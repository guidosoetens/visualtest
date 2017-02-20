#import "BGImageItem.h"

BGImageItem::~BGImageItem() {

}

void BGImageItem::render(ofTrueTypeFont & font) {

    ofImage* img = bgResources.getImageReference(mImageSetting->value);

    ofSetColor(255);
    ofRect(mPosition.x, mPosition.y, CONTROL_WIDTH, CONTROL_HEIGHT);
    img->draw(mPosition.x + 2, mPosition.y + 2, CONTROL_WIDTH - 4, CONTROL_HEIGHT - 4);
    ofSetColor(255);
}

void BGImageItem::update(float dt) {
    
}

void BGImageItem::mouseDown(ofVec2f p) {
    p = p - mPosition;
    if(p.x > 0 && p.x < CONTROL_WIDTH && p.y > 0 && p.y < CONTROL_HEIGHT) {
        //activate:
        mImagePicker->open(mImageSetting);
    }
}

void BGImageItem::mouseMove(ofVec2f p) {
    
}

void BGImageItem::mouseUp(ofVec2f p) {
    
}
