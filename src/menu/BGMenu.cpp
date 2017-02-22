#include "BGMenu.h"
#include "BGColorItem.h"
#include "BGImageItem.h"
#include "BGTextItem.h"

ofVec2f pushLoc(int & y, int inc) {
    ofVec2f res(25, y);
    y += inc;
    return res;
}

BGMenu::BGMenu()
:   mIsOpen(false)
{
    
    for(int i=0; i<NUM_STYLES; ++i) {

        BGStyle* style = bgResources.getStyle(i);

        int y = 50;
        //int x = 20;

        //load colors:
        mUserControls[i].push_back(new BGTextItem(pushLoc(y, 30), "Colors:"));
        for(int keyIdx=(BGResourceKey_Colors + 1); keyIdx<BGResourceKey_Images; ++keyIdx)
            mUserControls[i].push_back(new BGColorItem(pushLoc(y, 22), &style->colors[(BGResourceKey)keyIdx], &mColorPicker));
        y += 20;

        //load images:
        mUserControls[i].push_back(new BGTextItem(pushLoc(y, 30), "Images:"));
        for(int keyIdx=(BGResourceKey_Images + 1); keyIdx<BGResourceKey_Integers; ++keyIdx)
            mUserControls[i].push_back(new BGImageItem(pushLoc(y, 52), &style->images[(BGResourceKey)keyIdx], &mImagePicker));
        y += 20;

        //load integers:
        mUserControls[i].push_back(new BGTextItem(pushLoc(y, 30), "Integers:"));
        for(int keyIdx=(BGResourceKey_Integers + 1); keyIdx<BGResourceKey_Floats; ++keyIdx)
            mUserControls[i].push_back(new BGSlider(pushLoc(y, 22), &style->integers[(BGResourceKey)keyIdx], 0, 100, this));
        y += 20;

        //load floats:
        mUserControls[i].push_back(new BGTextItem(pushLoc(y, 30), "Floats:"));
        for(int keyIdx=(BGResourceKey_Floats + 1); keyIdx<BGResourceKeyCount; ++keyIdx)
            mUserControls[i].push_back(new BGSlider(pushLoc(y, 22), &style->floats[(BGResourceKey)keyIdx], 0, 1, 0.1, this));
    }

    mArrowImage.loadImage("arrow.png");
    mCrossImage.loadImage("cross.png");
}

BGMenu::~BGMenu() {
    for(int i=0; i<NUM_STYLES; ++i) {
        for(int j=0; j<mUserControls[i].size(); ++j)
            delete mUserControls[i][j];
    }
}

void BGMenu::valueChanged(BGSlider * slider) {
    
}

void BGMenu::renderButton(ofVec4f bounds, bool isArrow, bool isFlipped) {

    ofSetColor(255);
    ofRect(bounds.x, bounds.y, bounds.z, bounds.w);
    ofSetColor(0);
    ofRect(bounds.x + 2, bounds.y + 2, bounds.z - 4, bounds.w - 4);

    float scale = .6 * bounds.w / 256;

    ofSetColor(255);
    ofPushMatrix();
    ofTranslate(bounds.x + bounds.z / 2, bounds.y + bounds.w / 2);
    ofScale(scale * (isFlipped ? -1 : 1), scale);
    ofTranslate(-128, -128);
    (isArrow ? mArrowImage : mCrossImage).draw(0,0);
    ofPopMatrix();
}

void BGMenu::render(ofTrueTypeFont & font) {

    int styleIndex = bgResources.currentStyleIndex;

    ofSetColor(0);
    if(mIsOpen) {

        ofRect(10, 10, MENU_WIDTH, 500);
        
        if(mColorPicker.isOpen()) {
            mColorPicker.render(font);
        }
        else if(mImagePicker.isOpen()) {
            mImagePicker.render(font);
        }
        else {
            for(int i=0; i<mUserControls[styleIndex].size(); ++i)
                mUserControls[styleIndex][i]->render(font);

            //ofSetColor(255);
            // for(int i=0; i<2; ++i) {
            //     ofVec4f rect = i == 0 ? BTN_PREV_RECT : BTN_NEXT_RECT;
            //     ofRect(rect.x, rect.y, rect.z, rect.w);
            // }

            renderButton(BTN_PREV_RECT, true, true);
            renderButton(BTN_NEXT_RECT, true, false);

            ofPushMatrix();
            ofTranslate((BTN_PREV_RECT.x + BTN_PREV_RECT.z + BTN_NEXT_RECT.x) / 2.0, BTN_NEXT_RECT.y + BTN_NEXT_RECT.w / 2);
            drawCenteredText(font, ofToString(bgResources.currentStyleIndex));
            ofPopMatrix();

            //void drawCenteredText(ofTrueTypeFont & font, string text);
            //font.drawString(ofToString(bgResources.currentStyleIndex), BTN_NEXT_RECT.x - 50, BTN_NEXT_RECT.y);
        }

        ofSetColor(255);
    }
    else {
        ofRect(10,10,35,35);
    }

    renderButton(ofVec4f(15,15,25,25), false, false);

    // ofSetColor(255);
    // ofRect(15,15,20,20);
    // ofSetColor(0);
    // ofRect(17,17,16,16);
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