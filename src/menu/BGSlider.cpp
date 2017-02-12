#import "BGSlider.h"

BGSlider::BGSlider() 
:   mSliding(false)
,   mVariable(NULL)
,   mCallbackListener(NULL)
{

}

BGSlider::~BGSlider() {

}

void BGSlider::render(ofTrueTypeFont & font) {

    if(mVariable == NULL)
        return;

    ofSetColor(255);

    float center_y = SLIDER_HEIGHT/2;
    float barWidth = SLIDER_WIDTH - 2 * BUTTON_WIDTH;

    ofPushMatrix();
    ofTranslate(mPosition.x, mPosition.y);
    font.drawString(mTitle, -50, 15);
    ofRect(BUTTON_WIDTH, center_y - 1, barWidth, 2);

    float t = (*mVariable - mMinValue) / (float)(mMaxValue - mMinValue);
    ofCircle(BUTTON_WIDTH + t * barWidth, center_y, 4);

    font.drawString(ofToString(*mVariable), SLIDER_WIDTH, 15);

    ofCircle(BUTTON_WIDTH / 2, center_y, 5);
    ofCircle(SLIDER_WIDTH - BUTTON_WIDTH / 2, center_y, 5);

    ofPopMatrix();
}

void BGSlider::update(float dt) {
    
}


void BGSlider::mouseDown(ofVec2f p) {

    if(mVariable == NULL)
        return;
    
    ofVec2f off_p = p - mPosition;
    if(off_p.x > 0 && off_p.y > 0 && off_p.x < SLIDER_WIDTH && off_p.y < SLIDER_HEIGHT) {
        if(off_p.x < BUTTON_WIDTH)
            *mVariable = max(*mVariable - 1, mMinValue);
        else if(off_p.x > SLIDER_WIDTH - BUTTON_WIDTH)
            *mVariable = min(*mVariable + 1, mMaxValue);
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

        int prevVal = *mVariable;

        float xFrac = (p.x - mPosition.x - BUTTON_WIDTH) / (float)(SLIDER_WIDTH - 2 * BUTTON_WIDTH);
        int delta = mMaxValue - mMinValue;
        *mVariable = mMinValue + max(0, min(delta, (int)round(xFrac * delta)));

        if(prevVal != *mVariable && mCallbackListener != NULL)
            mCallbackListener->valueChanged(this);
    }
}

void BGSlider::mouseUp(ofVec2f p) {
    mSliding = false;
}
