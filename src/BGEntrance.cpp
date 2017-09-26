#include "BGEntrance.h"
#include "BGResources.h"

BGEntrance::BGEntrance(ofVec2f pos, float orientation, bool mask)
:   mPosition(pos)
,   mOrientation(orientation)
,   mWobbleParam(0)
{
    mFrontImage.loadImage("front.png");
    mBackImage.loadImage("back.png");
    mMaskImage.loadImage("mask.png");
    mHasMask = mask;
}

BGEntrance::~BGEntrance() {

}

void 
BGEntrance::update(float dt) {
    mWobbleParam = fmodf(mWobbleParam + dt / 4.0, 1.0);
}

void BGEntrance::prepareDraw(ofShader & mEntranceShader) {

    float scale = .3;//mHasMask ? .3 : .25;

    float scaleX = 1 + .03 * sinf(mWobbleParam * 2 * M_PI);
    ofPushMatrix();
    ofTranslate(mPosition);
    ofScale(scale, scale);
    ofRotate(180 * mOrientation / M_PI);
    ofScale(scaleX, 2 - scaleX);


    mEntranceShader.begin();
    ofFloatColor entranceColor = bgResources.getColorSetting(NetworkLightColorKey)->value;
    mEntranceShader.setUniform3f("uColor", entranceColor.r, entranceColor.g, entranceColor.b);
}

void BGEntrance::endDraw(ofShader & mEntranceShader) {
    ofPopMatrix();
    mEntranceShader.end();
}

void 
BGEntrance::renderBack(ofShader & mEntranceShader) {
    prepareDraw(mEntranceShader);
    ofTranslate(0, -150);
    mEntranceShader.setUniform1f("uDarken", 1);
    mBackImage.draw(-mBackImage.width / 2, -mBackImage.height / 2);
    endDraw(mEntranceShader);
}

void 
BGEntrance::render(ofShader & mEntranceShader) {
    prepareDraw(mEntranceShader);
    mEntranceShader.setUniform1f("uDarken", 1);
    mFrontImage.draw(-mFrontImage.width / 2, -mFrontImage.height / 2);
    ofTranslate(0,-30);
    mEntranceShader.setUniform1f("uDarken", 0);
    if(mHasMask || true)
        mMaskImage.draw(-mMaskImage.width / 2, -mMaskImage.height / 2);
    endDraw(mEntranceShader);
}