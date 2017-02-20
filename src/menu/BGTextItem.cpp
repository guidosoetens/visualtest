#import "BGTextItem.h"

BGTextItem::~BGTextItem() {

}

void BGTextItem::render(ofTrueTypeFont & font) {

    ofSetColor(50);
    ofRect(mPosition.x, mPosition.y, CONTROL_WIDTH, CONTROL_HEIGHT);
    
    ofSetColor(255);
    ofPushMatrix();
    ofTranslate(mPosition.x + CONTROL_WIDTH / 2, mPosition.y + CONTROL_HEIGHT / 2);
    drawCenteredText(font, mText);
    ofPopMatrix();
}

void BGTextItem::update(float dt) {
    
}

void BGTextItem::mouseDown(ofVec2f p) {
    
}

void BGTextItem::mouseMove(ofVec2f p) {
    
}

void BGTextItem::mouseUp(ofVec2f p) {
    
}
