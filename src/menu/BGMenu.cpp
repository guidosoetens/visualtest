#include "BGMenu.h"
#include "BGColorItem.h"
#include "BGImageItem.h"
#include "BGTextItem.h"

ofVec2f pushLoc(int & y, int inc) {
    ofVec2f res(MENU_OUT_MARGIN + MENU_INNER_MARGIN, y);
    y += inc;
    return res;
}

BGMenu::BGMenu()
:   mIsOpen(false)
,   mControlsHeight(0)
,   mScrolling(false)
,   mPreviousScrollPosition(0,0)
{
    
    for(int i=0; i<NUM_STYLES; ++i) {

        BGStyle* style = bgResources.getStyle(i);

        int y = CONTROLS_TOP_OFFSET;
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

        mControlsHeight = max(mControlsHeight, y);
    }

    mArrowImage.loadImage("arrow.png");
    mCrossImage.loadImage("cross.png");
    mCheckImage.loadImage("check.png");

    mPanelTarget.allocate(CONTROL_WIDTH, SUB_PANEL_HEIGHT, GL_RGBA, 8);
}

BGMenu::~BGMenu() {
    for(int i=0; i<NUM_STYLES; ++i) {
        for(int j=0; j<mUserControls[i].size(); ++j)
            delete mUserControls[i][j];
    }
}

void BGMenu::valueChanged(BGSlider * slider) {
    
}

float BGMenu::getCurrentScrollOffset() {
    if(mImagePicker.isOpen())
        return mImagePicker.scrollOffset;
    else if(mColorPicker.isOpen())
        return 0;
    return mScrollValue;
}

float BGMenu::getCurrentControlsHeight() {
    if(mImagePicker.isOpen())
        return mImagePicker.getTotalHeight();
    else if(mColorPicker.isOpen())
        return 100;
    return mControlsHeight;
}

void BGMenu::renderButton(ofVec4f bounds, ofImage & image, bool isFlipped) {

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
    image.draw(0,0);
    ofPopMatrix();
}

void BGMenu::render(ofTrueTypeFont & font) {

    int styleIndex = bgResources.currentStyleIndex;

    ofVec2f offset(MENU_OUT_MARGIN + MENU_INNER_MARGIN, CONTROLS_TOP_OFFSET);


    bool checkImg = false;

    ofSetColor(0);
    if(mIsOpen) {

        ofRect(MENU_OUT_MARGIN, MENU_OUT_MARGIN, MENU_WIDTH, SUB_PANEL_HEIGHT + CONTROLS_TOP_OFFSET - MENU_OUT_MARGIN + MENU_INNER_MARGIN);

        if(mColorPicker.isOpen()) {
            mColorPicker.render(font);
            checkImg = true;
        }
        else if(mImagePicker.isOpen()) {
            mPanelTarget.begin();
            ofSetColor(0);
            ofRect(0,0,1000,1000);
            ofPushMatrix();
            ofTranslate(-offset.x, -offset.y - mImagePicker.scrollOffset);
            mImagePicker.render(font);
            ofPopMatrix();
            mPanelTarget.end();
            mPanelTarget.draw(offset.x, offset.y);
            checkImg = true;
        }
        else {

            mPanelTarget.begin();
            ofSetColor(0);
            ofRect(0,0,1000,1000);
            ofPushMatrix();
            ofTranslate(-offset.x, -offset.y - mScrollValue);
            for(int i=0; i<mUserControls[styleIndex].size(); ++i)
                mUserControls[styleIndex][i]->render(font);
            ofPopMatrix();
            mPanelTarget.end();
            mPanelTarget.draw(offset.x, offset.y);

            renderButton(BTN_PREV_RECT, mArrowImage, true);
            renderButton(BTN_NEXT_RECT, mArrowImage, false);

            ofPushMatrix();
            ofTranslate((BTN_PREV_RECT.x + BTN_PREV_RECT.z + BTN_NEXT_RECT.x) / 2.0, BTN_NEXT_RECT.y + BTN_NEXT_RECT.w / 2);
            drawCenteredText(font, ofToString(bgResources.currentStyleIndex));
            ofPopMatrix();
        }

        //render scrollbar...
        ofSetColor(100);

        ofVec2f lineOffset(MENU_OUT_MARGIN + MENU_WIDTH - SCROLL_WIDTH / 2, CONTROLS_TOP_OFFSET + SCROLL_WIDTH / 2);
        float lineLength = SUB_PANEL_HEIGHT - SCROLL_WIDTH;

        //float scrollx = MENU_OUT_MARGIN + MENU_WIDTH - SCROLL_WIDTH + 2;
        float w = SCROLL_WIDTH / 2;
        ofRectRounded(lineOffset.x - w / 2, lineOffset.y - w / 2, w, lineLength + w, 5);
        w = w - 4;
        ofSetColor(0);
        ofRectRounded(lineOffset.x - w / 2, lineOffset.y - w / 2, w, lineLength + w, 5);

        float panelHeight = getCurrentControlsHeight();

        if(panelHeight > SUB_PANEL_HEIGHT) {

            float panelOffset = getCurrentScrollOffset();

            float frac = SUB_PANEL_HEIGHT / panelHeight;
            float offsetFrac = panelOffset / panelHeight;

            w = w - 4;
            ofSetColor(150);
            ofRectRounded(lineOffset.x - w / 2, lineOffset.y - w / 2 + offsetFrac * lineLength, w, frac * lineLength + w, 5);

        }

        ofSetColor(255);
    }
    else {
        ofRect(MENU_OUT_MARGIN, MENU_OUT_MARGIN, 35, 35);
    }

    ofVec4f pos = ofVec4f(MENU_OUT_MARGIN + 5, MENU_OUT_MARGIN + 5, 25, 25);
    if(mIsOpen) {
        if(checkImg)
            renderButton(pos, mCheckImage, false);
        else
            renderButton(pos, mCrossImage, false);
    }
    else
        renderButton(pos, mArrowImage, false);

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

    if(p.y < MENU_OUT_MARGIN + MENU_INNER_MARGIN + 25) {

        for(int i=0; i<2; ++i) {
            ofVec4f rect = i == 0 ? BTN_PREV_RECT : BTN_NEXT_RECT;
            if(p.x > rect.x && p.x < rect.x + rect.z && p.y > rect.y && p.y < rect.y + rect.w) {
                if(i == 0)
                    bgResources.currentStyleIndex = bgResources.currentStyleIndex == 0 ? NUM_STYLES - 1 : bgResources.currentStyleIndex - 1;
                else
                    bgResources.currentStyleIndex = (bgResources.currentStyleIndex + 1) % NUM_STYLES;
            }
        }

        if(p.x < 60) {

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
    }
    else if(p.x < MENU_OUT_MARGIN + MENU_WIDTH) {

        mScrolling = p.x > (MENU_OUT_MARGIN + MENU_WIDTH - SCROLL_WIDTH) && p.y > CONTROLS_TOP_OFFSET;
        if(mScrolling) {
            mPreviousScrollPosition = p;
        }
        else {

            if(mColorPicker.isOpen()) {
                mColorPicker.mouseDown(p);
            }
            else if(mImagePicker.isOpen()) {
                p.y += mImagePicker.scrollOffset;
                mImagePicker.mouseDown(p);
            }
            else {

                // for(int i=0; i<2; ++i) {
                //     ofVec4f rect = i == 0 ? BTN_PREV_RECT : BTN_NEXT_RECT;
                //     if(p.x > rect.x && p.x < rect.x + rect.z && p.y > rect.y && p.y < rect.y + rect.w) {
                //         if(i == 0)
                //             bgResources.currentStyleIndex = bgResources.currentStyleIndex == 0 ? NUM_STYLES - 1 : bgResources.currentStyleIndex - 1;
                //         else
                //             bgResources.currentStyleIndex = (bgResources.currentStyleIndex + 1) % NUM_STYLES;
                //     }
                // }

                int styleIndex = bgResources.currentStyleIndex;
                if(mIsOpen) {
                    p.y += mScrollValue;
                    for(int i=0; i<mUserControls[styleIndex].size(); ++i)
                        mUserControls[styleIndex][i]->mouseDown(p);
                }
            }
        }
    }
}

void BGMenu::mouseMove(ofVec2f p) {

    if(mScrolling) {

        float panelHeight = getCurrentControlsHeight();
        if(panelHeight > SUB_PANEL_HEIGHT) {
            float factor = panelHeight / SUB_PANEL_HEIGHT;
            float deltaY = mPreviousScrollPosition.y - p.y;
            float scrollY = factor * deltaY / 10.0;
            mouseScrolled(ofVec2f(MENU_OUT_MARGIN + MENU_WIDTH / 2, 0), scrollY);
            mPreviousScrollPosition = p;
        }

    }
    else {
        if(mColorPicker.isOpen()) {
            mColorPicker.mouseMove(p);
        }
        else if(mImagePicker.isOpen()) {
            p.y += mImagePicker.scrollOffset;
            mImagePicker.mouseMove(p);
        }
        else {
            p.y += mScrollValue;
            int styleIndex = bgResources.currentStyleIndex;
            for(int i=0; i<mUserControls[styleIndex].size(); ++i)
                mUserControls[styleIndex][i]->mouseMove(p);
        }
    }
}

void BGMenu::mouseScrolled(ofVec2f p, float scrollY) {
    if(mIsOpen && p.x < MENU_OUT_MARGIN + MENU_WIDTH) {

        if(mImagePicker.isOpen()) {
            //scroll
            mImagePicker.scrollOffset -= 10 * scrollY;
            mImagePicker.scrollOffset = fmaxf(0, fminf(mImagePicker.scrollOffset, mImagePicker.getTotalHeight() - SUB_PANEL_HEIGHT));
        }
        else if(!mColorPicker.isOpen()) {
            //scroll
            mScrollValue -= 10 * scrollY;
            mScrollValue = fmaxf(0, fminf(mScrollValue, mControlsHeight - SUB_PANEL_HEIGHT));
        }
    }
}

void BGMenu::mouseUp(ofVec2f p) {
    mScrolling = false;
    mColorPicker.mouseUp(p);
    mImagePicker.mouseUp(p);
    int styleIndex = bgResources.currentStyleIndex;
    for(int i=0; i<mUserControls[styleIndex].size(); ++i)
        mUserControls[styleIndex][i]->mouseUp(p);
}