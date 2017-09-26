#include "BGFooObject.h"
#include "BGResources.h"

BGFooObject::BGFooObject()
:   mTimeParam(0)
{
    mShader.load("shaders/revealShader");
    mImage.loadImage("star.png");
    mPerlinImage.loadImage("perlin.jpg");

    ofShader mShader;
    ofImage mImage;
    ofImage mPerlinImage;
    float mTimeParam;
}

BGFooObject::~BGFooObject() {

}

void 
BGFooObject::update(float dt) {
    mTimeParam = fmodf(mTimeParam + dt / 3.0, 1.0);
}

void 
BGFooObject::render() {

    ofPushMatrix();
    ofScale(1.5, 1.0);
    mPerlinImage.draw(0,0);
    ofPopMatrix();

    float reveal = 0;
    if(mTimeParam > .25 && mTimeParam < .75)
        reveal = (mTimeParam - .25) / .5;
    else if(mTimeParam >= .75)
        reveal = 1.0;

    ofPushMatrix();
    mShader.begin();
    mShader.setUniformTexture("uPerlinMask", mPerlinImage.getTextureReference(), 1);
    mShader.setUniform1f("uRevealParam", reveal);
    mShader.setUniform2f("uTextureResolution", mImage.width, mImage.height);
    mShader.setUniform2f("uPerlinResolution", mPerlinImage.width, mPerlinImage.height);
    mImage.draw(0,0);
    mShader.end();
    ofPopMatrix();
}