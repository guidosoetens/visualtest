#include "BGBackground.h"

BGBackground::BGBackground() {
    mTimeParameter = 0;
    mSpotsImage.loadImage("spots.jpg");
}

BGBackground::~BGBackground() {
    
}

void 
BGBackground::render(ofShader & mBackgroundShader, ofImage & mCellImage, ofImage & mBubbleImage, ofImage & mMembraneImage, float width, float height) {
    ofClear(255, 185, 185, 255);

    mBackgroundShader.begin();
    mBackgroundShader.setUniformTexture("uTexture", mCellImage.getTextureReference(), 0); //uCellTexture   uTexture
    //mBackgroundShader.setUniformTexture("uBubbleTexture", mBubbleImage.getTextureReference(), 1); //uCellTexture   uTexture
    mBackgroundShader.setUniformTexture("uMembraneTexture", mMembraneImage.getTextureReference(), 2);
    mBackgroundShader.setUniform2f("uResolution", width, height);
    mBackgroundShader.setUniform1f("uTime", mTimeParameter);
    mBackgroundShader.setUniformTexture("uSpotTexture", mSpotsImage.getTextureReference(),1); //uCellTexture   uTexture

    ofMesh quad;
    quad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    quad.addVertex(ofVec3f(0,0,0));
    quad.addVertex(ofVec3f(width,0,0));
    quad.addVertex(ofVec3f(0,height,0));
    quad.addVertex(ofVec3f(width,height,0));

    quad.addTexCoord(ofVec2f(0,0));
    quad.addTexCoord(ofVec2f(1,0));
    quad.addTexCoord(ofVec2f(0,1));
    quad.addTexCoord(ofVec2f(1,1));
    quad.draw();

    mBackgroundShader.end();
}

void 
BGBackground::update(float dt) {
    mTimeParameter = fmodf(mTimeParameter + .01 * dt, 1.0);
}