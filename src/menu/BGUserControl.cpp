#import "BGUserControl.h"

BGUserControl::BGUserControl() {

}

BGUserControl::~BGUserControl() {

}

void BGUserControl::render(ofTrueTypeFont & font) {
    
}

void BGUserControl::update(float dt) {
    
}

void BGUserControl::mouseDown(ofVec2f p) {
    
}

void BGUserControl::mouseMove(ofVec2f p) {
    
}

void BGUserControl::mouseUp(ofVec2f p) {
    
}

void drawTextLeft(ofTrueTypeFont & font, string text) {
    float scale = 1.0;//height / FONT_SIZE; //font size is 10

    ofPushMatrix();
    ofScale(scale, scale);
    ofTranslate(0, font.getSize() / 2);
    font.drawString(text, 0, 0);
    ofPopMatrix();
}

void drawCenteredText(ofTrueTypeFont & font, string text) {

    float scale = 1.0;//height / FONT_SIZE; //font size is 10

    ofPushMatrix();
    ofScale(scale, scale);
    ofTranslate(- font.stringWidth(text) / 2, font.getSize() / 2);
    font.drawString(text, 0, 0);
    ofPopMatrix();
}