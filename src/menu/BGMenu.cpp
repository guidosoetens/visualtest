#include "BGMenu.h"

BGMenu::BGMenu()
:   mIsOpen(false)
{
    for(int i=0; i<6; ++i) {
        mSliderValues.push_back(1);
        BGSlider slider;
        slider.setup("RED", ofVec2f(50, 40 + 25 * i), &mSliderValues[i], 0, 255, this);
        mSliders.push_back(slider);
    }
}

BGMenu::~BGMenu() {
    
}

void BGMenu::valueChanged(BGSlider * slider) {
    //...
}

void BGMenu::render(ofTrueTypeFont & font) {
    ofSetColor(0);
    if(mIsOpen) {
        ofRect(10,10,200,400);

        for(int i=0; i<mSliders.size(); ++i)
            mSliders[i].render(font);
    }
    else {
        ofRect(10,10,30,30);
    }

    ofSetColor(255);
    ofRect(15,15,20,20);
    ofSetColor(0);
    ofRect(17,17,16,16);
}

void BGMenu::update(float dt) {
    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i].update(dt);
}

void BGMenu::mouseDown(ofVec2f p) {
    if(p.x < 40 && p.y < 40)
        mIsOpen = !mIsOpen;

    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i].mouseDown(p);
}

void BGMenu::mouseMove(ofVec2f p) {
    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i].mouseMove(p);
}

void BGMenu::mouseUp(ofVec2f p) {
    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i].mouseUp(p);
}