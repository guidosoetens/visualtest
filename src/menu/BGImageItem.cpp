#import "BGImageItem.h"

BGImageItem::~BGImageItem() {

}

void BGImageItem::render(ofTrueTypeFont & font) {

    ofPushMatrix();
    ofTranslate(mPosition.x, mPosition.y + IMAGE_ITEM_HEIGHT / 2);
    drawTextLeft(font, mImageSetting->name);
    ofPopMatrix();

    ofImage* img = bgResources.getImageReference(mImageSetting->value);

    float blockWidth = IMAGE_ITEM_HEIGHT;
    float offset = CONTROL_WIDTH - blockWidth;

    ofSetColor(255);
    ofRect(mPosition.x + offset, mPosition.y, blockWidth, blockWidth);
    img->draw(mPosition.x + offset + 2, mPosition.y + 2, blockWidth - 4, blockWidth - 4);
    ofSetColor(255);
}

void BGImageItem::update(float dt) {
    
}

void BGImageItem::mouseDown(ofVec2f p) {
    p = p - mPosition;
    if(p.x > 0 && p.x < CONTROL_WIDTH && p.y > 0 && p.y < IMAGE_ITEM_HEIGHT) {
        //activate:
        mImagePicker->open(mImageSetting);
    }
}

void BGImageItem::mouseMove(ofVec2f p) {
    
}

void BGImageItem::mouseUp(ofVec2f p) {
    
}
