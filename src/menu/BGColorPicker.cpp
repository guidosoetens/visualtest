#import "BGColorPicker.h"

BGColorPicker::BGColorPicker() 
:   mIsOpen(false)
{
    BGIntegerSetting* vals[6] = { &mRed, &mGreen, &mBlue, &mHue, &mSaturation, &mValue };
    const char* titles[6] = { "RED", "GREEN", "BLUE", "HUE", "SATUR.", "VALUE" };
    for(int i=0; i<6; ++i) {
        vals[i]->name = string(titles[i]);
        vals[i]->value = 0;
        int offset = i < 3 ? 40 : 50;
        mSliders.push_back(new BGSlider(ofVec2f(50, offset + 25 * i), vals[i], 0, 255, this));
    }
}

BGColorPicker::~BGColorPicker() {
    for(int i=0; i<mSliders.size(); ++i)
        delete mSliders[i];
}

void BGColorPicker::valueChanged(BGSlider * slider) {

    int idx = 0;
    for(idx = 0; idx<mSliders.size(); ++idx) {
        if(slider == mSliders[idx]) 
            break;
    }

    if(idx < 3) {
        //RGB has changed, so update HSV
        ofColor color(mRed.value, mGreen.value, mBlue.value);
        float h, s, v;
        color.getHsb(h, s, v);
        mHue.value = (int)round(h);
        mSaturation.value = (int)round(s);
        mValue.value = (int)round(v);
    }
    else {
        //HSV has changed, so update RGB
        ofColor color;
        color.setHsb(mHue.value, mSaturation.value, mValue.value);
        mRed.value = color.r;
        mGreen.value = color.g;
        mBlue.value = color.b;
    }

    mColorSetting->value = getColor();

    //*mColor = ofColor(mRed, mGreen, mBlue);
}

void BGColorPicker::open(BGColorSetting* colorSetting) {
    mIsOpen = true;

    mColorSetting = colorSetting;

    mRed.value = (int)round(colorSetting->value.r);
    mGreen.value = (int)round(colorSetting->value.g);
    mBlue.value = (int)round(colorSetting->value.b);

    valueChanged(mSliders[0]);
}

ofColor BGColorPicker::getColor() {
    return ofColor(mRed.value, mGreen.value, mBlue.value);
}

bool BGColorPicker::isOpen() {
    return mIsOpen;
}

void BGColorPicker::close() {
    mIsOpen = false;
}

void BGColorPicker::render(ofTrueTypeFont & font) {
    if(!mIsOpen)
        return;

    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i]->render(font);

    ofSetColor(mRed.value, mGreen.value, mBlue.value);
    ofRect(50, 200, 100, 20);
    ofSetColor(255);
}

void BGColorPicker::update(float dt) {
    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i]->update(dt);
}

void BGColorPicker::mouseDown(ofVec2f p) {
    if(!mIsOpen)
        return;

    // if(p.x > 50 && p.y > 200 && p.x < 150 && p.y < 220) {
    //     mIsOpen = false;
    //     return;
    // }

    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i]->mouseDown(p);
}

void BGColorPicker::mouseMove(ofVec2f p) {
    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i]->mouseMove(p);
}

void BGColorPicker::mouseUp(ofVec2f p) {
    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i]->mouseUp(p);
}
