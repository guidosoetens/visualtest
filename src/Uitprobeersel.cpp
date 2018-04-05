#include "Uitprobeersel.h"

const int texDimension = 256;

Uitprobeersel::Uitprobeersel() {
    mShader.load("shaders/general/borderGlowShader");
    mTexImage.allocate(texDimension, texDimension);
    unsigned char* pixels = mTexImage.getPixels();
    for(int i=0; i<texDimension; ++i) {
        for(int j=0; j<texDimension; ++j) {
            float x = (j + 0.5) / texDimension;
            float y = (i + 0.5) / texDimension;
            int idx = i * texDimension + j;
            float value = .5 - .5 * cosf(y * 2 * M_PI);
            pixels[idx] = 255 * value;
        }
    }
    mTexImage.setFromPixels(pixels, texDimension, texDimension);
    mTexImage.updateTexture();

    mTimeParameter = 0;
}

Uitprobeersel::~Uitprobeersel() {
    
}

void Uitprobeersel::reload() {
    mShader.load("shaders/general/borderGlowShader");
}

void Uitprobeersel::update(float dt) {
    mTimeParameter = fmodf(mTimeParameter + dt / 10.0, 1.0);
}

void Uitprobeersel::draw() {

    float revealParam = 0.0;
    float winParam = -1.0;

    float flowStartTime = .1;
    float assimilateStartTime = .4;
    float burstStartTime = .8;

    if(mTimeParameter < flowStartTime) {
        revealParam = 0.0;
        winParam = -1;
    }
    else if(mTimeParameter < assimilateStartTime) {
        float t = (mTimeParameter - flowStartTime) / (assimilateStartTime - flowStartTime);
        revealParam = .5 - .5 * cos(t * M_PI);
        winParam = - 1;
    }
    else if(mTimeParameter < burstStartTime) {
        float t = (mTimeParameter - assimilateStartTime) / (burstStartTime - assimilateStartTime);
        revealParam = 1.0;
        winParam = -1 + t;
    }
    else {
        float t = (mTimeParameter - burstStartTime) / (1. - burstStartTime);
        revealParam = 1.0;
        winParam = t;
    }

    // revealParam = mTimeParameter;
    // winParam = -1;

    int maxSamples = 30;

    ofMesh quad;
    quad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    for(int x=0; x<maxSamples; ++x) {
        for(int y=0; y<2; ++y) {
            quad.addVertex(ofVec2f(x, 0));
            quad.addTexCoord(ofVec2f(x, y));
            quad.addNormal(ofVec3f(0,1,0));
        }
    }

    float offset = 20.0 + 20.0 * revealParam + 40.0 * (1. + winParam);

    ofPushMatrix();
    ofTranslate(400,200);
    ofScale(500 / maxSamples, 1, 1);
    mShader.begin();
    mShader.setUniformTexture("uTexImage", mTexImage.getTextureReference(), 0);
    mShader.setUniform1f("uTexDimension", texDimension);

    mShader.setUniform1f("uBoundOffset", offset);
    mShader.setUniform1f("uMaxDepth", maxSamples);
    mShader.setUniform1f("uRevealParameter", revealParam);
    mShader.setUniform1f("uWinAnimParameter", winParam);

    quad.draw();
    mShader.end();
    ofPopMatrix();

    // mTexImage.draw(0,0);
}