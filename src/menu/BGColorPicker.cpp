#import "BGColorPicker.h"

BGColorPicker::BGColorPicker() 
:   mRed(0)
,   mGreen(0)
,   mBlue(0)
,   mHue(0)
,   mSaturation(0)
,   mValue(0)
,   mIsOpen(false)
{
    int* vals[6] = { &mRed, &mGreen, &mBlue, &mHue, &mSaturation, &mValue };
    const char* titles[6] = { "R", "G", "B", "H", "S", "V" };
    for(int i=0; i<6; ++i) {
        int offset = i < 3 ? 40 : 50;
        BGSlider slider;
        slider.setup(titles[i], ofVec2f(50, offset + 25 * i), vals[i], 0, 255, this);
        mSliders.push_back(slider);
    }
}

BGColorPicker::~BGColorPicker() {

}

void BGColorPicker::valueChanged(BGSlider * slider) {

    int idx = 0;
    for(idx = 0; idx<mSliders.size(); ++idx) {
        if(slider == &mSliders[idx])
            break;
    }

    if(idx < 3) {
        //RGB has changed, so update HSV
        ofColor color(mRed, mGreen, mBlue);
        float h, s, v;
        color.getHsb(h, s, v);
        mHue = (int)round(h);
        mSaturation = (int)round(s);
        mValue = (int)round(v);
    }
    else {
        //HSV has changed, so update RGB
        ofColor color;
        color.setHsb(mHue, mSaturation, mValue);
        mRed = color.r;
        mGreen = color.g;
        mBlue = color.b;
    }
}

void BGColorPicker::open(ofColor color) {
    mIsOpen = true;

    mRed = (int)round(color.r);
    mGreen = (int)round(color.g);
    mBlue = (int)round(color.b);

    valueChanged(&mSliders[0]);
}

ofColor BGColorPicker::getColor() {
    return ofColor(mRed, mGreen, mBlue);
}

bool BGColorPicker::isOpen() {
    return mIsOpen;
}

void BGColorPicker::render(ofTrueTypeFont & font) {
    if(!mIsOpen)
        return;

    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i].render(font);

    ofSetColor(mRed, mGreen, mBlue);
    ofRect(50, 200, 100, 20);
    ofSetColor(255);
}

void BGColorPicker::update(float dt) {
    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i].update(dt);
}

void BGColorPicker::mouseDown(ofVec2f p) {
    if(!mIsOpen)
        return;

    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i].mouseDown(p);
}

void BGColorPicker::mouseMove(ofVec2f p) {
    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i].mouseMove(p);
}

void BGColorPicker::mouseUp(ofVec2f p) {
    for(int i=0; i<mSliders.size(); ++i)
        mSliders[i].mouseUp(p);
}
