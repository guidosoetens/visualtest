#import "BGColorItem.h"

BGColorItem::~BGColorItem() {

}

void BGColorItem::render(ofTrueTypeFont & font) {

    ofSetColor(255);
    ofRect(mPosition.x, mPosition.y, CONTROL_WIDTH, CONTROL_HEIGHT);
    ofSetColor(mColorSetting->value);
    ofRect(mPosition.x + 2, mPosition.y + 2, CONTROL_WIDTH - 4, CONTROL_HEIGHT - 4);
    ofSetColor(255);
}

void BGColorItem::update(float dt) {
    
}

void BGColorItem::mouseDown(ofVec2f p) {
    p = p - mPosition;
    if(p.x > 0 && p.x < CONTROL_WIDTH && p.y > 0 && p.y < CONTROL_HEIGHT) {
        //activate:
        mColorPicker->open(mColorSetting);
    }
}

void BGColorItem::mouseMove(ofVec2f p) {
    
}

void BGColorItem::mouseUp(ofVec2f p) {
    
}
