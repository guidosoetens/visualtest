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

    //add color buttons:
    for(int i=0; i<6; ++i) {
        ColorPropertyButton cpb = { ofColor(255), string("CLR ") + ofToString(i + 1), ofVec2f(50, 200 + 25 * i) };
        mColorButtons.push_back(cpb);
    }
}

BGMenu::~BGMenu() {
    
}

void BGMenu::valueChanged(BGSlider * slider) {
    //...
}

void BGMenu::colorPickerClosed(BGColorPicker * colorPicker) {
    //...
}

void BGMenu::render(ofTrueTypeFont & font) {
    ofSetColor(0);
    if(mIsOpen) {
        ofRect(10,10,250,400);
        if(mColorPicker.isOpen()) {
            mColorPicker.render(font);
        }
        else {
            for(int i=0; i<mSliders.size(); ++i)
                mSliders[i].render(font);

            for(int i=0; i<mColorButtons.size(); ++i) {

                ofSetColor(255);
                ofPushMatrix();
                ofTranslate(mColorButtons[i].position.x, mColorButtons[i].position.y);
                font.drawString(mColorButtons[i].name, -30, 15);
                ofRect(50, 5, 150, 10);
                ofSetColor(mColorButtons[i].color);
                ofRect(51, 6, 148, 8);
                ofPopMatrix();
            }

            ofSetColor(255);
        }
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
    mColorPicker.update(dt);
}

void BGMenu::mouseDown(ofVec2f p) {
    if(p.x < 40 && p.y < 40)
        mIsOpen = !mIsOpen;

    if(mIsOpen) {
        if(mColorPicker.isOpen()) {
            mColorPicker.mouseDown(p);
        }
        else {
            for(int i=0; i<mSliders.size(); ++i)
                mSliders[i].mouseDown(p);
        }
    }
}

void BGMenu::mouseMove(ofVec2f p) {
    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i].mouseMove(p);
    mColorPicker.mouseMove(p);
}

void BGMenu::mouseUp(ofVec2f p) {
    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i].mouseUp(p);
    mColorPicker.mouseUp(p);
}