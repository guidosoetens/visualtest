#include "BGEntrance.h"
#include "BGResources.h"

BGEntrance::BGEntrance(ofVec2f pos, float orientation, bool mask)
:   mPosition(pos)
,   mOrientation(orientation)
,   mWobbleParam(0)
,   mHasOverrideColor(false)
,   globalScale(1.0)
,   darkenBrim(false)
{
    mFrontImage.loadImage("front.png");
    mBackImage.loadImage("back.png");
    mMaskImage.loadImage("mask.png");
    m3DImage.loadImage("endpoint3d.png");
    mSpotsImage.loadImage("spots.jpg");
    mHasMask = mask;
}

BGEntrance::~BGEntrance() {

}

void 
BGEntrance::update(float dt) {
    mWobbleParam = fmodf(mWobbleParam + dt / 4.0, 1.0);
    mFlowParam = fmodf(mFlowParam + dt / 30.0, 1.0);

    //mOrientation += .01;
}

void BGEntrance::prepareDraw(ofShader & mEntranceShader) {

    float scale = (mHasMask) ? .3 : .25;

    float scaleX = 1 + .03 * sinf(mWobbleParam * 2 * M_PI);
    ofPushMatrix();
    ofTranslate(mPosition);
    ofScale(scale, scale, scale);
    ofRotate(180 * mOrientation / M_PI);
    ofScale(scaleX, 2 - scaleX, 1);


    mEntranceShader.begin();
    ofFloatColor entranceColor = bgResources.getColorSetting(NetworkLightColorKey)->value;
    if(mHasOverrideColor)
        entranceColor = mColor;
    mEntranceShader.setUniform3f("uColor", entranceColor.r, entranceColor.g, entranceColor.b);
}

void BGEntrance::endDraw(ofShader & mEntranceShader) {
    ofPopMatrix();
    mEntranceShader.end();
}

void 
BGEntrance::renderBack(ofShader & mEntranceShader) {
    // prepareDraw(mEntranceShader);
    // ofTranslate(0, -150);
    // mEntranceShader.setUniform1f("uDarken", 1);
    // mEntranceShader.setUniform1i("uDarkenBrim", darkenBrim ? 0 : 0);
    // mBackImage.draw(-mBackImage.width / 2, -mBackImage.height / 2);
    // endDraw(mEntranceShader);
}

void 
BGEntrance::render(ofShader & mEntranceShader) {

    prepareDraw(mEntranceShader);
    mEntranceShader.setUniform1f("uDarken", 1);
    mEntranceShader.setUniform1f("uDarkenBrim", darkenBrim ? .0 : 0);
    mEntranceShader.setUniformTexture("uSpotsTexture", mSpotsImage.getTextureReference(), 1);
    mEntranceShader.setUniform1f("uTimeParam", mFlowParam);
    ofPushMatrix();
    float gScale = 1.1;

    if(mOrientation == 0)
        gScale *= 2.0;


    ofScale(gScale, gScale, gScale);//0.6, 0.6, 0.6);
    m3DImage.draw(-m3DImage.width / 2, -m3DImage.height / 2);
    ofPopMatrix();
    ofTranslate(0,-30);
    // mEntranceShader.setUniform1f("uDarken", 0);
    // if(mHasMask && false)
    //     mMaskImage.draw(-mMaskImage.width / 2, -mMaskImage.height / 2);
    endDraw(mEntranceShader);

     if(mHasMask) {
        float scale = (mHasMask) ? .3 : .25;
        float scaleX = 1 + .03 * sinf(mWobbleParam * 2 * M_PI);
        ofPushMatrix();
        ofTranslate(mPosition);
        ofScale(scale, scale, scale);
        ofRotate(180 * mOrientation / M_PI);
        ofScale(scaleX, 2 - scaleX, 1);
        mMaskImage.draw(-mMaskImage.width / 2, -mMaskImage.height / 2);
        ofPopMatrix();
     }

    // prepareDraw(mEntranceShader);
    // mEntranceShader.setUniform1f("uDarken", 1);
    // mEntranceShader.setUniform1f("uDarkenBrim", darkenBrim ? .0 : 0);
    // ofPushMatrix();
    // ofScale(0.7, 0.7);
    // m3DImage.draw(-m3DImage.width / 2, -m3DImage.height / 2);
    // ofPopMatrix();
    // ofTranslate(0,-30);
    // mEntranceShader.setUniform1f("uDarken", 0);
    // if(mHasMask)
    //     mMaskImage.draw(-mMaskImage.width / 2, -mMaskImage.height / 2);
    // endDraw(mEntranceShader);
}