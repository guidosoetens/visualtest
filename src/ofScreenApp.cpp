#include "ofScreenApp.h"


//--------------------------------------------------------------
void ofScreenApp::setup(){
    
    ofDisableArbTex();

    mCurrMenuIdx = 0;

    gettimeofday(&mLastSampledTime, NULL);

    mLoaderShader.load("shaders/loaderShader");
    mScreenShader.load("shaders/screenShader");
    mSettingsImage.loadImage("settings.png");
    mBackgroundImage.loadImage("screenshot.png");
    mStarImage.loadImage("star.png");
    mFrontGuiTarget.allocate(1024, 768);

    mFont.loadFont("sofachrome rg.ttf", 32);
    mLevelButtonFont.loadFont("sofachrome rg.ttf", 24);
    mSymbolFont.loadFont("Arial", 60, true, true);
    mReadableFont.loadFont("sofachrome rg.ttf", 16);

    mTimeParameter = 0;
}

//--------------------------------------------------------------
void ofScreenApp::update(){

    struct timeval currtime;
    gettimeofday(&currtime, NULL);

    long seconds = currtime.tv_sec - mLastSampledTime.tv_sec;
    long useconds = currtime.tv_usec - mLastSampledTime.tv_usec;
    float dt = round(seconds * 1000 + useconds / 1000.0) / 1000.0;
    mLastSampledTime = currtime;

    mTimeParameter = fmodf(mTimeParameter + .5 * dt, 1.0);

}

void 
ofScreenApp::drawBrim(bool drawLeft, float bottomHeight) {

    float width = 1024.0;
    float height = 768.0;
    ofVec2f center(width / 2.0, height / 2.0);

    //draw corner unit:
    ofMesh cornerMesh;
    cornerMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    for(int i=0; i<4; ++i) {
        ofVec2f focus(width / 2, height / 2);
        if(i == 0 || i == 3)
            focus.x -= .385 * width;
        else
            focus.x += .385 * width;

        if(i < 2)
            focus.y -= .4 * height;
        else
            focus.y += .4 * height;
        
        float startAngle = (-1 + .5 * i) * M_PI;
        int samples = 20;
        for(int it=0; it<samples; ++it) {
            float t = it / (float)samples;
            float angle = startAngle + t * .5 * M_PI;
            ofVec2f to(cosf(angle), sinf(angle));
            cornerMesh.addVertex(ofVec3f(focus.x + to.x * 30.0, focus.y + to.y * 30.0, 0));
            cornerMesh.addVertex(ofVec3f(focus.x + to.x * 70.0, focus.y + to.y * 40.0, 0));
        }
    }

    cornerMesh.addVertex(cornerMesh.getVertex(0));
    cornerMesh.addVertex(cornerMesh.getVertex(1));

    ofSetColor(200);
    cornerMesh.draw();

    float w1 = 150;
    float w2 = 500;
    float w3 = 30;
    float h = bottomHeight;


    ofVec2f corner(width / 2 + .385 * width + w3, height / 2 + .4 * height + w3);

    ofMesh bottomMesh;
    bottomMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

    if(drawLeft)
        bottomMesh.addVertex(ofVec3f(width - corner.x, corner.y, 0));
    else 
        bottomMesh.addVertex(ofVec3f(corner.x, corner.y, 0));

    //sample left curve:
    for(int i=0; i<30; ++i) {
        float t = i / 29.0;
        float x = corner.x - (w1 + w2 + w3) + t * w1;
        float y = corner.y - (1 - cosf(t * M_PI)) / 2.0 * h;

        if(drawLeft)
            x = width - x;

        bottomMesh.addVertex(ofVec3f(x, y, 0));
    }

    //sample right part:
    ofVec2f curveSrc = corner - ofVec2f(30, h + w3);
    for(int i=0; i<20; ++i) {
        float t = i / 19.0;
        float angle = (.5 - .5 * t) * M_PI;
        ofVec2f to(cosf(angle), sinf(angle));
        ofVec2f sample = curveSrc + w3 * to;

        if(drawLeft)
            sample.x = width - sample.x;

        bottomMesh.addVertex(ofVec3f(sample.x, sample.y, 0));
    }

    bottomMesh.draw();

    float loaderWidth = w2 + w3;
    float loaderHeight = h - 10;
    drawLoader(drawLeft ? (width / 2 - .385 * width - w3) : (corner.x - loaderWidth), corner.y - loaderHeight, loaderWidth, loaderHeight, w3);

    h = 80;

    float topWidth = 300;
    float topBrimY = .1 * height - w3;

    //top mesh:
    ofMesh topMesh;
    topMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    topMesh.addVertex(ofVec3f(width / 2, topBrimY, 0));

    //sample left curve:
    for(int i=0; i<30; ++i) {
        float t = i / 29.0;
        float x = center.x - (w1 + topWidth / 2) + t * w1;
        float y = topBrimY + (1 - cosf(t * M_PI)) / 2.0 * h;
        topMesh.addVertex(ofVec3f(x, y, 0));
    }

    //sample right curve:
    for(int i=0; i<30; ++i) {
        float t = i / 29.0;
        float x = center.x + topWidth / 2 + t * w1;
        float y = topBrimY + (1 + cosf(t * M_PI)) / 2.0 * h;
        topMesh.addVertex(ofVec3f(x, y, 0));
    }

    topMesh.draw();
    
    drawLoader(center.x - topWidth / 2, topBrimY, topWidth, h - 10, w3);
}

void 
ofScreenApp::drawWinLevel() {
    drawBrim(true, 120);

    float width = 1024.0;
    float height = 768.0;

    ofSetColor(255);
    string txt = "LEVEL 12";
    float txtWidth = mReadableFont.stringWidth(txt);
    mReadableFont.drawString(txt, width / 2 - txtWidth / 2, 330);

    txt = "COMPLETE!";
    txtWidth = mFont.stringWidth(txt);
    mFont.drawString(txt, width / 2 - txtWidth / 2, 385);

    for(int i=0; i<3; ++i) {
        
        ofVec2f loc(width / 2 + (i - 1) * 160, i == 1 ? 220 : 260 );
        float angle = (i - 1) * 25;
        
        ofPushMatrix();
        ofTranslate(loc.x, loc.y);
        ofRotate(angle);

        //layers:
        for(int j=0; j<3; ++j) {

            ofSetColor(j == 1 ? 0 : 255);
            float scale = .3 - .05 * j;
            if(i == 1)
                scale += .05;

            ofPushMatrix();
            ofScale(scale, scale);
            ofTranslate(-mStarImage.width/2, -mStarImage.height/2);
            mStarImage.draw(0,0);
            ofPopMatrix();

        }

        ofPopMatrix();
    }

    string titles[3] = {"TIME:", "ALARM:", "SIZE:"};
    string vals[3] = {"0:03.5", "0.0", "3"};
    string thresholds[3] = {"/ 0:10", "/ 1 sec.", "/ 3 blobs"};

    for(int i=0; i<3; ++i) {
        float y = 450 + i * 40;
        txtWidth = mReadableFont.stringWidth(vals[i]);
        mReadableFont.drawString(titles[i], 200, y);
        mReadableFont.drawString(vals[i], 500 - txtWidth - 20, y);
        mReadableFont.drawString(thresholds[i], 500, y);
    }

    drawButton("<", ofVec2f(width - 200, height - 420), 110, false);
    drawButton("..", ofVec2f(width - 200, height - 290), 110, false);
    drawButton(">", ofVec2f(width - 200, height - 160), 110, false);

}

void 
ofScreenApp::drawNotification() {
    drawBrim(true, 120);

    float width = 1024.0;
    float height = 768.0;

    ofSetColor(255);
    string txt = "MESSAGE";
    float txtWidth = mFont.stringWidth(txt);
    mFont.drawString(txt, width / 2 - txtWidth / 2, 190);

    string lines[4] = {"For the best gaming", "experience, please make sure", "'Multitasking Gestures' is", "DISABLED on your iPad."}; 

    for(int i=0; i<4; ++i) {
        float txtWidth = mReadableFont.stringWidth(lines[i]);
        mReadableFont.drawString(lines[i], width / 2 - txtWidth / 2, 280 + 30 * i);
    }

    mSettingsImage.draw(130, height - 350, 150, 150);
    string pathLines[4] = {"SETTINGS", "> GENERAL", "> MULTITASKING GESTURES", "> OFF"}; 
    for(int i=0; i<4; ++i) 
        mReadableFont.drawString(pathLines[i], 300 + i * 20, height - 320 + 30 * i);

    drawButton("OK", ofVec2f(width - 220, height - 160), 120, false);
}

void 
ofScreenApp::drawMenuElements() {

    drawBrim(false, 80);

    float width = 1024.0;
    float height = 768.0;

    /*
    ofNoFill();
    ofSetColor(100);
    ofSetLineWidth(5);
    ofRect(40, 30, width - 80, height - 60);
    ofFill();
    */

    ofSetColor(255);
    string txt = "WORLD 5";
    float txtWidth = mFont.stringWidth(txt);
    mFont.drawString(txt, width / 2 - txtWidth / 2, 190);

    ofVec2f size(width, height);
    ofVec2f center(width / 2.0, height / 2.0);
    float hexWidth = 120;
    float hexStepYFactor = 0.86602540378;

    int idx = 0;
    for(int i=0; i<3; ++i) {
        int cols = (i % 2 == 0) ? 5 : 6;
        for(int j=0; j<cols; ++j) {
            
            int x = center.x + (.5 * (1 - i % 2) + j - 2.5) * hexWidth;
            int y = center.y + hexStepYFactor * (i - 1) * hexWidth;

            std::stringstream ss;
            ss << (idx + 1);
            drawLevelButton(ss.str(), ofVec2f(x, y), .9 * hexWidth);

            ++idx;
        }
    }

    //https://www.shadertoy.com/view/XsfGDS

    txt = "SCORE: 50382090";
    txtWidth = mLevelButtonFont.stringWidth(txt);
    mLevelButtonFont.drawString(txt, width / 2 - txtWidth / 2 + 60, height - 170);

    float topOffset = 0.0;
    ofVec2f topOffsetVector(topOffset * cosf(M_PI / 6.0), topOffset * sinf(M_PI / 6.0));
    float topY = center.y - 2 * hexStepYFactor * hexWidth - topOffsetVector.y;
    drawButton("<", ofVec2f(center.x - 2.5 * hexWidth - topOffsetVector.x, topY), .9 * hexWidth);
    drawButton(">", ofVec2f(center.x + 2.5 * hexWidth + topOffsetVector.x, topY), .9 * hexWidth);

    float bottomY = center.y + 2 * hexStepYFactor * hexWidth + topOffset;
    drawButton(">>|", ofVec2f(center.x - 2.5 * hexWidth, bottomY), .9 * hexWidth);

}

void 
ofScreenApp::drawLoader(float x, float y, float width, float height, float cornerRadius) {

    ofMesh quad;
    quad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    quad.addVertex(ofVec3f(x,y,0));
    quad.addVertex(ofVec3f(x + width, y, 0));
    quad.addVertex(ofVec3f(x, y + height, 0));
    quad.addVertex(ofVec3f(x + width, y + height, 0));

    quad.addTexCoord(ofVec2f(0,0));
    quad.addTexCoord(ofVec2f(1,0));
    quad.addTexCoord(ofVec2f(0,1));
    quad.addTexCoord(ofVec2f(1,1));
    
    mLoaderShader.begin();
    mLoaderShader.setUniform2f("uResolution", width, height);
    mLoaderShader.setUniform1f("uTimeParameter", mTimeParameter);
    mLoaderShader.setUniform1f("uCornerRadius", cornerRadius);
    quad.draw();
    mLoaderShader.end();
}

void 
ofScreenApp::drawLevelButton(string txt, ofVec2f loc, float width) {
    float rad = .5 * width / cosf(M_PI / 6.0);
    float angOffset = .5 * M_PI;
    drawHexagon(loc, rad, 0, angOffset, 7, 255, 70);
    float txtWidth = mLevelButtonFont.stringWidth(txt);
    mLevelButtonFont.drawString(txt, loc.x - txtWidth / 2, loc.y - 5);

    string starTxt = "***";
    txtWidth = mSymbolFont.stringWidth(starTxt);
    mSymbolFont.drawString(starTxt, loc.x - txtWidth / 2, loc.y + 60);
}

void 
ofScreenApp::drawButton(string txt, ofVec2f loc, float width, bool flip) {
    float rad = .5 * width / cosf(M_PI / 6.0);
    float angOffset = flip ? .5 * M_PI : 0;
    drawHexagon(loc, rad, 0, angOffset, 7, 255, 70);
    float txtWidth = mLevelButtonFont.stringWidth(txt);
    mLevelButtonFont.drawString(txt, loc.x - txtWidth / 2, loc.y + 10);
}

void 
ofScreenApp::drawHexagon(ofVec2f pt, float radius, float baseWidth, float angleOffset, float lineThickness, float lineOpacity, float fillOpacity) {
    baseWidth = max(0.0f, baseWidth);
    ofVec2f baseVector(cosf(angleOffset), sinf(angleOffset));// = CGPointMake(cosf(angleOffset), sinf(angleOffset));
    
    float rad1 = radius + .5 * lineThickness;
    float rad2 = MAX(0, rad1 - lineThickness);
    
    ofMesh hexMesh;
    ofMesh hexFillMesh;
    hexMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    hexFillMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

    for(int i=0; i<6; ++i) {
        
        float baseFactor = (i < 3) ? baseWidth : -baseWidth;
        ofVec2f center(pt.x + baseFactor * baseVector.x, pt.y + baseFactor * baseVector.y);
        
        float angle = angleOffset + ((i - 1) / 6.0) * 2.0 * M_PI;
        ofVec2f to(cosf(angle), sinf(angle));

        hexMesh.addVertex(ofVec3f(center.x + rad1 * to.x, center.y + rad1 * to.y, 0));
        hexMesh.addVertex(ofVec3f(center.x + rad2 * to.x, center.y + rad2 * to.y, 0));
        hexFillMesh.addVertex(hexMesh.getVertex(2 * i + 1));
    }

    //close meshes:
    hexMesh.addVertex(hexMesh.getVertex(0));
    hexMesh.addVertex(hexMesh.getVertex(1));
    //hexFillMesh.addVertex(hexFillMesh.getVertex(0));

    ofSetColor(fillOpacity);
    hexFillMesh.draw();
    ofSetColor(lineOpacity);
    hexMesh.draw();
    ofSetColor(255);

    /*
    //set colors:
    GLKVector4 colorsLine[14];
    GLKVector4 colorsFill[8];
    for(int i=0; i<7; ++i) {
        colorsLine[2 * i] = lineColor;
        colorsLine[2 * i + 1] = lineColor;
        colorsFill[i] = fillColor;
    }
    colorsFill[7] = fillColor;
    
    [self drawMesh:ptsFill withColors:colorsFill andVertexCount:8 withRenderMode:BGGraphics_MeshRenderMode_Fan];
    [self drawMesh:pts withColors:colorsLine andVertexCount:14 withRenderMode:BGGraphics_MeshRenderMode_Strip];
    */
}

//--------------------------------------------------------------
void ofScreenApp::draw(){

    mFrontGuiTarget.begin();
    ofClear(0);
    if(mCurrMenuIdx == 0)
        drawNotification();
    else if(mCurrMenuIdx == 1)
        drawMenuElements();
    else if(mCurrMenuIdx == 2)
        drawWinLevel();
    mFrontGuiTarget.end();

    ofMesh quad;
    quad.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    quad.addVertex(ofVec3f(0,0,0));
    quad.addVertex(ofVec3f(1024,0,0));
    quad.addVertex(ofVec3f(0,768,0));
    quad.addVertex(ofVec3f(1024,768,0));

    quad.addTexCoord(ofVec2f(0,0));
    quad.addTexCoord(ofVec2f(1,0));
    quad.addTexCoord(ofVec2f(0,1));
    quad.addTexCoord(ofVec2f(1,1));

    mScreenShader.begin();
    mScreenShader.setUniform2f("uResolution", 1024, 768);
    mScreenShader.setUniform1f("uTime", mTimeParameter);
    mScreenShader.setUniform1f("uZoomStrength", 1);
    mScreenShader.setUniform3f("uTerminalColor", 1, .7, .4); //orange
    //mScreenShader.setUniform3f("uTerminalColor", .3, .85, 1); //blue
    //ec3(0.3, 0.85, 1.0);

    mScreenShader.setUniformTexture("uBackTexture", mBackgroundImage.getTextureReference(), 0);
    mScreenShader.setUniformTexture("uFrontTexture", mFrontGuiTarget.getTextureReference(), 1);

    quad.draw();

    mScreenShader.end();
}

//--------------------------------------------------------------
void ofScreenApp::keyPressed(int key){

    int totalOptions = 3;

    if(key == OF_KEY_LEFT) {
        mCurrMenuIdx--;
        if(mCurrMenuIdx < 0)
            mCurrMenuIdx = totalOptions - 1;
    }
    else if(key == OF_KEY_RIGHT) {
        mCurrMenuIdx = (mCurrMenuIdx + 1) % totalOptions;
    }
}

//--------------------------------------------------------------
void ofScreenApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofScreenApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofScreenApp::mouseDragged(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofScreenApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofScreenApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofScreenApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofScreenApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofScreenApp::dragEvent(ofDragInfo dragInfo){

}
