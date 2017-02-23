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
        valueAsString = ofToString(value, 3);
        name = mFloatSetting->name;
    }

    //int value = mIntegerSetting->value;

    /*
    #define SLIDER_MARGIN_LEFT 200
    #define SLIDER_MARGIN_RIGHT 100
    #define BUTTON_WIDTH (SLIDER_HEIGHT)
    */

    ofSetColor(255);

    float barWidth = CONTROL_WIDTH - SLIDER_MARGIN_LEFT - SLIDER_MARGIN_RIGHT;

    /*
    #define SLIDER_MARGIN_LEFT 200
    #define SLIDER_MARGIN_RIGHT 100
    */

    ofPushMatrix();
    ofTranslate(mPosition.x, mPosition.y + CONTROL_HEIGHT / 2);
    drawTextLeft(font, name);

    ofRect(SLIDER_MARGIN_LEFT, -1, barWidth, 2);

    //float t = (value - mMinValue) / (float)(mMaxValue - mMinValue);
    ofCircle(SLIDER_MARGIN_LEFT + t * barWidth, 0, 4);

    ofPushMatrix();
    ofTranslate(CONTROL_WIDTH - SLIDER_MARGIN_RIGHT + BUTTON_WIDTH + 2, 0);
    drawTextLeft(font, valueAsString);
    ofPopMatrix();

    //font.drawString(valueAsString, SLIDER_WIDTH, 15);

    ofCircle(SLIDER_MARGIN_LEFT - BUTTON_WIDTH / 2, 0, 5);
    ofCircle(CONTROL_WIDTH - SLIDER_MARGIN_RIGHT + BUTTON_WIDTH / 2, 0, 5);

    ofPopMatrix();
}

void BGSlider::update(float dt) {
    
}


void BGSlider::mouseDown(ofVec2f p) {

    if(mIntegerSetting == NULL && mFloatSetting == NULL)
        return;
    
    ofVec2f off_p = p - mPosition;
    if(off_p.x > SLIDER_MARGIN_LEFT - BUTTON_WIDTH && off_p.y > 0 && off_p.x < CONTROL_WIDTH - SLIDER_MARGIN_RIGHT + BUTTON_WIDTH && off_p.y < CONTROL_HEIGHT) {
        if(off_p.x < SLIDER_MARGIN_LEFT) {
            //Decrease:
            if(mIntegerSetting != NULL)
                mIntegerSetting->value = max(mIntegerSetting->value - 1, mMinValue);
            else
                mFloatSetting->value = fmaxf(mFloatSetting->value - mStepf, mMinValuef);
        }
        else if(off_p.x > CONTROL_WIDTH - SLIDER_MARGIN_RIGHT) {
            //Increase:
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

    float sliderWidth = CONTROL_WIDTH - (SLIDER_MARGIN_LEFT + SLIDER_MARGIN_RIGHT);

    if(mSliding) {

        float xFrac = (p.x - mPosition.x - SLIDER_MARGIN_LEFT) / sliderWidth;

        if(mIntegerSetting != NULL) {

            int prevVal = mIntegerSetting->value;
            int delta = mMaxValue - mMinValue;
            mIntegerSetting->value = mMinValue + max(0, min(delta, (int)round(xFrac * delta)));

            if(prevVal != mIntegerSetting->value && mCallbackListener != NULL)
                mCallbackListener->valueChanged(this);
        }
        else if(mFloatSetting != NULL) {

            float prevVal = mFloatSetting->value;
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
