#include "BGMenu.h"
#include "BGColorItem.h"
#include "BGImageItem.h"

BGMenu::BGMenu()
:   mIsOpen(false)
{
    
    for(int i=0; i<NUM_STYLES; ++i) {

        BGStyle* style = bgResources.getStyle(i);

        mUserControls[i].push_back(new BGSlider(ofVec2f(0, 60), &style->integers[FooIntegerKey], 0, 100, this));
        mUserControls[i].push_back(new BGSlider(ofVec2f(0, 80), &style->floats[FooFloatKey], 0, 1, 0.1, this));
        mUserControls[i].push_back(new BGColorItem(ofVec2f(0, 100), &style->colors[NetworkColorKey], &mColorPicker));
        mUserControls[i].push_back(new BGColorItem(ofVec2f(0, 120), &style->colors[NetworkDarkColorKey], &mColorPicker));
        mUserControls[i].push_back(new BGColorItem(ofVec2f(0, 140), &style->colors[NetworkLightColorKey], &mColorPicker));
        mUserControls[i].push_back(new BGImageItem(ofVec2f(0, 160), &style->images[BackgroundImageKey], &mImagePicker));
        mUserControls[i].push_back(new BGImageItem(ofVec2f(0, 180), &style->images[ObstacleImageKey], &mImagePicker));
        //BGColorItem
    }

}

BGMenu::~BGMenu() {
    for(int i=0; i<NUM_STYLES; ++i) {
        for(int j=0; j<mUserControls[i].size(); ++j)
            delete mUserControls[i][j];
    }
}

void BGMenu::valueChanged(BGSlider * slider) {
    
}

void BGMenu::render(ofTrueTypeFont & font) {

    int styleIndex = bgResources.currentStyleIndex;

    ofSetColor(0);
    if(mIsOpen) {

        ofRect(10,10,200,500);
        
        if(mColorPicker.isOpen()) {
            mColorPicker.render(font);
        }
        else if(mImagePicker.isOpen()) {
            mImagePicker.render(font);
        }
        else {
            for(int i=0; i<mUserControls[styleIndex].size(); ++i)
                mUserControls[styleIndex][i]->render(font);

            ofSetColor(255);
            for(int i=0; i<2; ++i) {
                ofVec4f rect = i == 0 ? BTN_PREV_RECT : BTN_NEXT_RECT;
                ofRect(rect.x, rect.y, rect.z, rect.w);
            }
            font.drawString(ofToString(bgResources.currentStyleIndex), BTN_NEXT_RECT.x - 50, BTN_NEXT_RECT.y);
        }

        ofSetColor(255);
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

    if(mColorPicker.isOpen()) {
        mColorPicker.update(dt);
    }
    else if(mImagePicker.isOpen()) {
        mImagePicker.update(dt);
    }
    else {
        int styleIndex = bgResources.currentStyleIndex;
        for(int i=0; i<mUserControls[styleIndex].size(); ++i)
            mUserControls[styleIndex][i]->update(dt);
    }
}

void BGMenu::mouseDown(ofVec2f p) {

    if(p.x < 40 && p.y < 40) {

        if(mIsOpen) {
            if(mColorPicker.isOpen())
                mColorPicker.close();
            else if(mImagePicker.isOpen())
                mImagePicker.close();
            else
                mIsOpen = false;
        }
        else
            mIsOpen = true;
    }

    if(mColorPicker.isOpen()) {
        mColorPicker.mouseDown(p);
    }
    else if(mImagePicker.isOpen()) {
        mImagePicker.mouseDown(p);
    }
    else {

        for(int i=0; i<2; ++i) {
            ofVec4f rect = i == 0 ? BTN_PREV_RECT : BTN_NEXT_RECT;
            if(p.x > rect.x && p.x < rect.x + rect.z && p.y > rect.y && p.y < rect.y + rect.w) {
                if(i == 0)
                    bgResources.currentStyleIndex = bgResources.currentStyleIndex == 0 ? NUM_STYLES - 1 : bgResources.currentStyleIndex - 1;
                else
                    bgResources.currentStyleIndex = (bgResources.currentStyleIndex + 1) % NUM_STYLES;
            }
        }

        int styleIndex = bgResources.currentStyleIndex;
        if(mIsOpen) {
            for(int i=0; i<mUserControls[styleIndex].size(); ++i)
                mUserControls[styleIndex][i]->mouseDown(p);
        }
    }
}

void BGMenu::mouseMove(ofVec2f p) {
    if(mColorPicker.isOpen()) {
        mColorPicker.mouseMove(p);
    }
    else if(mImagePicker.isOpen()) {
        mImagePicker.mouseMove(p);
    }
    else {
        int styleIndex = bgResources.currentStyleIndex;
        for(int i=0; i<mUserControls[styleIndex].size(); ++i)
            mUserControls[styleIndex][i]->mouseMove(p);
    }
}

void BGMenu::mouseUp(ofVec2f p) {
    mColorPicker.mouseUp(p);
    mImagePicker.mouseUp(p);
    int styleIndex = bgResources.currentStyleIndex;
    for(int i=0; i<mUserControls[styleIndex].size(); ++i)
        mUserControls[styleIndex][i]->mouseUp(p);
}