#import "BGSlider.h"

// BGSlider::BGSlider() 
// :   mSliding(false)
// ,   mVariable(NULL)
// ,   mCallbackListener(NULL)
// {

// }

BGSlider::~BGSlider() {

}

void BGSlider::render(ofTrueTypeFont & font) {

    if(mIntegerSetting == NULL && mFloatSetting == NULL)
        return;

    string name = "";
    string valueAsString = "";
    float t = 0;

    if(mIntegerSetting != NULL) {
        int value = mIntegerSetting->value;
        t = (value - mMinValue) / (float)(mMaxValue - mMinValue);
        valueAsString = ofToString(value);
        name = mIntegerSetting->name;
    }
    else if(mFloatSetting != NULL) {
        float value = mFloatSetting->value;
        t = (value - mMinValuef) / (float)(mMaxValuef - mMinValuef);
        valueAsString = ofToString(value);
        name = mFloatSetting->name;
    }

    //int value = mIntegerSetting->value;

    ofSetColor(255);

    float center_y = SLIDER_HEIGHT/2;
    float barWidth = SLIDER_WIDTH - 2 * BUTTON_WIDTH;

    ofPushMatrix();
    ofTranslate(mPosition.x, mPosition.y);
    font.drawString(name, -30, 15);
    ofRect(BUTTON_WIDTH, center_y - 1, barWidth, 2);

    //float t = (value - mMinValue) / (float)(mMaxValue - mMinValue);
    ofCircle(BUTTON_WIDTH + t * barWidth, center_y, 4);

    font.drawString(valueAsString, SLIDER_WIDTH, 15);

    ofCircle(BUTTON_WIDTH / 2, center_y, 5);
    ofCircle(SLIDER_WIDTH - BUTTON_WIDTH / 2, center_y, 5);

    ofPopMatrix();
}

void BGSlider::update(float dt) {
    
}


void BGSlider::mouseDown(ofVec2f p) {

    if(mIntegerSetting == NULL && mFloatSetting == NULL)
        return;
    
    ofVec2f off_p = p - mPosition;
    if(off_p.x > 0 && off_p.y > 0 && off_p.x < SLIDER_WIDTH && off_p.y < SLIDER_HEIGHT) {
        if(off_p.x < BUTTON_WIDTH) {
            if(mIntegerSetting != NULL)
                mIntegerSetting->value = max(mIntegerSetting->value - 1, mMinValue);
            else
                mFloatSetting->value = fmaxf(mFloatSetting->value - mStepf, mMinValuef);
        }
        else if(off_p.x > SLIDER_WIDTH - BUTTON_WIDTH) {
            if(mIntegerSetting != NULL)
                mIntegerSetting->value = min(mIntegerSetting->value + 1, mMaxValue);
            else
                mFloatSetting->value = fminf(mFloatSetting->value + mStepf, mMaxValuef);
        }
        else {
            mSliding = true;
            mouseMove(p);
        }

        if(mCallbackListener != NULL)
            mCallbackListener->valueChanged(this);
    }
}

void BGSlider::mouseMove(ofVec2f p) {
    if(mSliding) {

        if(mIntegerSetting != NULL) {
            int prevVal = mIntegerSetting->value;

            float xFrac = (p.x - mPosition.x - BUTTON_WIDTH) / (float)(SLIDER_WIDTH - 2 * BUTTON_WIDTH);
            int delta = mMaxValue - mMinValue;
            mIntegerSetting->value = mMinValue + max(0, min(delta, (int)round(xFrac * delta)));

            if(prevVal != mIntegerSetting->value && mCallbackListener != NULL)
                mCallbackListener->valueChanged(this);
        }
        else if(mFloatSetting != NULL) {
            float prevVal = mFloatSetting->value;

            float xFrac = (p.x - mPosition.x - BUTTON_WIDTH) / (float)(SLIDER_WIDTH - 2 * BUTTON_WIDTH);
            float delta = mMaxValuef - mMinValuef;
            mFloatSetting->value = mMinValuef + fmaxf(0, fminf(delta, xFrac * delta));

            if(prevVal != mFloatSetting->value && mCallbackListener != NULL)
                mCallbackListener->valueChanged(this);
        }
    }
}

void BGSlider::mouseUp(ofVec2f p) {
    mSliding = false;
}
