#include "BGEntrance.h"
#include "BGResources.h"

BGEntrance::BGEntrance(ofVec2f pos, float orientation)
:   mPosition(pos)
,   mOrientation(orientation)
,   mWobbleParam(0)
{
    mEntanceImage.loadImage("new_endpoint.png");
}

BGEntrance::~BGEntrance() {

}

void 
BGEntrance::update(float dt) {
    mWobbleParam = fmodf(mWobbleParam + dt / 4.0, 1.0);
}

void 
BGEntrance::renderBack(ofShader & mEntranceShader) {

}

void 
BGEntrance::render(ofShader & mEntranceShader) {

    float scaleX = 1 + .03 * sinf(mWobbleParam * 2 * M_PI);

    ofFloatColor entranceColor = bgResources.getColorSetting(NetworkLightColorKey)->value;
    ofPushMatrix();
    ofTranslate(mPosition);
    ofScale(.15, .15);
    ofRotate(180 * mOrientation / M_PI);
    ofScale(scaleX, 2 - scaleX);
    ofTranslate(-mEntanceImage.width / 2, -mEntanceImage.height / 2);
    mEntranceShader.begin();
    mEntranceShader.setUniform3f("uColor", entranceColor.r, entranceColor.g, entranceColor.b);
    mEntranceShader.setUniform2f("uResolution", mEntanceImage.width, mEntanceImage.height);
    mEntanceImage.draw(0, 0);
    mEntranceShader.end();
    ofPopMatrix();
}