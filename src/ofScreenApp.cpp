#include "ofScreenApp.h"


//--------------------------------------------------------------
void ofScreenApp::setup(){
    
    ofDisableArbTex();

    gettimeofday(&mLastSampledTime, NULL);

    mScreenShader.load("shaders/screenShader");
    mBackgroundImage.loadImage("screenshot.png");
    mFrontGuiTarget.allocate(1024, 768);

    mFont.loadFont("sofachrome rg.ttf", 40);
    mLevelButtonFont.loadFont("sofachrome rg.ttf", 30);
    mSymbolFont.loadFont("Arial", 60, true, true);

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

    mTimeParameter = fmodf(mTimeParameter + dt, 1.0);

}

void 
ofScreenApp::drawMenuElements() {

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
    string txt = "WORLD 1";
    float txtWidth = mFont.stringWidth(txt);
    mFont.drawString(txt, width / 2 - txtWidth / 2, 170);

    ofVec2f size(width, height);
    ofVec2f center(width / 2.0, height / 2.0);
    float hexWidth = 125;
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

    txt = "SCORE: 5038209";
    txtWidth = mLevelButtonFont.stringWidth(txt);
    mLevelButtonFont.drawString(txt, width / 2 - txtWidth / 2 + 80, height - 140);

    float topOffset = 10.0;
    ofVec2f topOffsetVector(topOffset * cosf(M_PI / 6.0), topOffset * sinf(M_PI / 6.0));
    float topY = center.y - 2 * hexStepYFactor * hexWidth - topOffsetVector.y;
    drawButton("<", ofVec2f(center.x - 2.5 * hexWidth - topOffsetVector.x, topY), .9 * hexWidth);
    drawButton(">", ofVec2f(center.x + 2.5 * hexWidth + topOffsetVector.x, topY), .9 * hexWidth);

    float bottomY = center.y + 2 * hexStepYFactor * hexWidth + topOffset;
    drawButton(">>|", ofVec2f(center.x - 2.5 * hexWidth, bottomY), .9 * hexWidth);

    //draw corner unit:
    ofMesh cornerMesh;
    cornerMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    for(int i=0; i<4; ++i) {
        ofVec2f focus = center;
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
ofScreenApp::drawButton(string txt, ofVec2f loc, float width) {
    float rad = .5 * width / cosf(M_PI / 6.0);
    float angOffset = .5 * M_PI;
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
    drawMenuElements();
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
    mScreenShader.setUniform3f("uTerminalColor", 1, .7, .4);

    mScreenShader.setUniformTexture("uBackTexture", mBackgroundImage.getTextureReference(), 0);
    mScreenShader.setUniformTexture("uFrontTexture", mFrontGuiTarget.getTextureReference(), 1);

    quad.draw();

    mScreenShader.end();
}

//--------------------------------------------------------------
void ofScreenApp::keyPressed(int key){
    
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
