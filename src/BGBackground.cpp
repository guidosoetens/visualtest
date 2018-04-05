#include "BGBackground.h"
#include "BGResources.h"

BGBackground::BGBackground() {
    mTimeParameter = 0;
}

BGBackground::~BGBackground() {
    
}

void 
BGBackground::render(ofShader & mBackgroundShader, ofImage & mCellImage, ofImage & mBubbleImage, ofImage & mMembraneImage, ofImage & mStringsImage, float width, float height) {
    ofClear(255, 185, 185, 255);

    //ofImage* spotImage = bgResources.getImageReference(BackgroundImageKey);
    ofImage* spotImage = bgResources.getImageReference(ObstacleImageKey);

    //BackgroundColorKey
    ofColor color = bgResources.getColorSetting(BackgroundColorKey)->value;
    ofColor borderColor = bgResources.getColorSetting(BackgroundBorderColorKey)->value;


    mBackgroundShader.begin();

    for(int i=0; i<4; ++i) {
        ofColor c = bgResources.getColorSetting(BGResourceKey(BackgroundColorKey + i))->value;
        mBackgroundShader.setUniform3f("uColor" + ofToString(i + 1), c.r / 255.0, c.g / 255.0, c.b / 255.0);
    }

    //BackgroundZoomKey


    mBackgroundShader.setUniformTexture("uTexture", mCellImage.getTextureReference(), 0);
    mBackgroundShader.setUniform1i("uMode", bgResources.getIntegerSetting(BackgroundModeKey)->value);
    mBackgroundShader.setUniform1f("uZoom", 5 * bgResources.getFloatSetting(BackgroundZoomKey)->value);
    mBackgroundShader.setUniform1f("uWeight", .1 * bgResources.getFloatSetting(BackgroundWeightKey)->value);
    
    //mBackgroundShader.setUniformTexture("uBubbleTexture", mBubbleImage.getTextureReference(), 1); 
    mBackgroundShader.setUniformTexture("uSpotTexture", spotImage->getTextureReference(), 1);
    mBackgroundShader.setUniformTexture("uMembraneTexture", mMembraneImage.getTextureReference(), 2);
    mBackgroundShader.setUniform2f("uResolution", width, height);
    mBackgroundShader.setUniform1f("uTime", mTimeParameter);
    mBackgroundShader.setUniform3f("uBackgroundColor", color.r / 255.0, color.g / 255.0, color.b / 255.0);
    mBackgroundShader.setUniform1f("uCellHueShift", bgResources.getFloatSetting(BackgroundCellHueShiftKey)->value);
    mBackgroundShader.setUniform3f("uBorderParticleColor", borderColor.r / 255.0, borderColor.g / 255.0, borderColor.b / 255.0);
    mBackgroundShader.setUniform1f("uBorderRepsU", (float)bgResources.getIntegerSetting(BorderRepsUKey)->value);
    mBackgroundShader.setUniform1f("uBorderRepsV", (float)bgResources.getIntegerSetting(BorderRepsVKey)->value);

    mBackgroundShader.setUniformTexture("uBubbleTexture", mStringsImage.getTextureReference(), 3); 

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