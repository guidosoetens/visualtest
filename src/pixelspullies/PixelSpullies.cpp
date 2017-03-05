#import "PixelSpullies.h"

#define SCALE 20
#define PRINT_SCALE 100

PixelSpullies::PixelSpullies() {
}

PixelSpullies::~PixelSpullies() {
}

void PixelSpullies::setup() {
    mMario.loadImage("pixelspullies/mario.png");
    mLink.loadImage("pixelspullies/link.png");
    mPainterShader.load("shaders/pixelSpulliesShader");

    mMario.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    mLink.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
}

void PixelSpullies::process() {

    const char* names[2] = { "pixelspullies/outputMario.png", "pixelspullies/outputLink.png" };

    for(int i=0; i<2; ++i) {
        ofImage & img = i == 0 ? mMario : mLink;
        ofFbo fbo;
        fbo.allocate(PRINT_SCALE * img.width, PRINT_SCALE * img.height);

        fbo.begin();
        ofPushMatrix();
        ofScale(PRINT_SCALE, PRINT_SCALE);
        mPainterShader.begin();
        mPainterShader.setUniform2f("uTexSize", img.width, img.height);
        mPainterShader.setUniform2f("uTexStep", 1.0 / img.width, 1.0 / img.height);
        img.draw(0,0);
        mPainterShader.end();
        ofPopMatrix();
        fbo.end();

        ofPixels pixels; 
        fbo.readToPixels(pixels);

        ofImage outputImg;
        outputImg.setFromPixels(pixels);
        outputImg.update();
        outputImg.saveImage(names[i]);
    }
}

void PixelSpullies::render() {
    ofSetColor(255,0,0);
    ofRect(0,0,1024,768);
    ofSetColor(0,0,0);
    ofRect(5,5,1024 - 10,768 - 10);

    ofSetColor(255);

    ofPushMatrix();
    ofTranslate(10,10);
    ofScale(SCALE, SCALE);

    mMario.draw(0,0);
    mPainterShader.begin();
    mPainterShader.setUniform2f("uTexSize", mMario.width, mMario.height);
    mPainterShader.setUniform2f("uTexStep", 1.0 / mMario.width, 1.0 / mMario.height);
    mMario.draw(mMario.width + 1,0);
    mPainterShader.end();

    mLink.draw(0, mMario.height + 1);
    mPainterShader.begin();
    mPainterShader.setUniform2f("uTexSize", mLink.width, mLink.height);
    mPainterShader.setUniform2f("uTexStep", 1.0 / mLink.width, 1.0 / mLink.height);
    mLink.draw(mLink.width + 1, mMario.height + 1);
    mPainterShader.end();

    ofPopMatrix();


    // //make target:
    // mMarioTarget.begin();
    // ofPushMatrix();
    // ofScale(SCALE, SCALE);
    // mPainterShader.begin();
    // mPainterShader.setUniform2f("uTexSize", mMario.width, mMario.height);
    // mPainterShader.setUniform2f("uTexStep", 1.0 / mMario.width, 1.0 / mMario.height);
    // mMario.draw(0,0);
    // mPainterShader.end();
    // ofPopMatrix();
    // mMarioTarget.end();

    // mMarioTarget.draw(100,0);
}