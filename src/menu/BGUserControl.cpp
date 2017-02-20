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