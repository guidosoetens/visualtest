#import "BGUserControl.h"

BGUserControl::BGUserControl() {

}

BGUserControl::~BGUserControl() {

}

void BGUserControl::render(ofTrueTypeFont & font) {
    cout << "BGUserControl::render -- dit is de fecking parent call..." << endl;
}

void BGUserControl::update(float dt) {
    cout << "BGUserControl::update -- dit is de fecking parent call..." << endl;
}

void BGUserControl::mouseDown(ofVec2f p) {
    cout << "BGUserControl::mouseDown -- dit is de fecking parent call..." << endl;
}

void BGUserControl::mouseMove(ofVec2f p) {
    cout << "BGUserControl::mouseMove -- dit is de fecking parent call..." << endl;
}

void BGUserControl::mouseUp(ofVec2f p) {
    cout << "BGUserControl::mouseUp -- dit is de fecking parent call..." << endl;
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