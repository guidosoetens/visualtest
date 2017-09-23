#include "BGCog.h"
#import "BGResources.h"

#define TEETH 12
#define BASE_RADIUS 130
#define TOOTH_TOP_OFFSET 5
#define BAR_WIDTH 20
#define BAR_CORNER_RADIUS 5
#define BAR_TEETH_CW 2
#define BAR_TEETH_CCW 7
#define TOOTH_HEIGHT 20
#define OUTER_RADIUS (BASE_RADIUS + TOOTH_HEIGHT)
#define BAR_ANGLE (mOrientation)
#define BAR_TEETH (BAR_TEETH_CW + BAR_TEETH_CCW + 1)
#define BAR_OFFSET (BASE_RADIUS + TOOTH_HEIGHT + .5 * BAR_WIDTH + 2 * TOOTH_TOP_OFFSET + 5)

BGCog::BGCog(ofVec2f pos, float _angle) {

    mPosition = pos;
    mOrientation = _angle;
    mTimeAnimParam = 0;
    mCellsAnimParam = 0;

    mBumpImage.loadImage("bump.jpg");

    mCenterMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    mCenterMesh.addVertex(ofVec2f(0,0));
    mCenterMesh.addNormal(ofVec3f(0,0,1));

    mTeethMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    int reps = TEETH;

    float toothTopOffset = TOOTH_TOP_OFFSET;

    for(int i=0; i<reps; ++i) {
        float ang = i / (float)reps * 2 * M_PI;

        for(int j=0; j<30; ++j) {
            float t = j / 30.0;
            float locAng = ang + t * 2 * M_PI / (float)reps;
            ofVec2f to(cosf(locAng), sinf(locAng));

            float offset = BASE_RADIUS + getToothOffset(t);

            ofVec2f p = offset * to;

            mCenterMesh.addVertex(p);
            mCenterMesh.addNormal(ofVec3f(0,0,1));

            mTeethMesh.addVertex(p);
            mTeethMesh.addNormal(ofVec3f(0,0,1));
            mTeethMesh.addVertex(p + toothTopOffset * to);
            mTeethMesh.addNormal(ofVec3f(to.x, to.y, 0));
        }
    }

    //calculate normals:
    int n = mTeethMesh.getVertices().size();
    for(int i=0; i<n; i+=2) {
        ofVec2f prev = mTeethMesh.getVertex(i == 0 ? (n - 1) : (i - 1));
        ofVec2f next = mTeethMesh.getVertex((i + 3) % n);

        ofVec2f to = next - prev;
        to = to.normalize();

        mTeethMesh.setNormal(i + 1, ofVec3f(to.y, -to.x, 0));
    }


    mCenterMesh.addVertex(mCenterMesh.getVertex(1));
    mCenterMesh.addNormal(ofVec3f(0,0,1));

    mTeethMesh.addVertex(mTeethMesh.getVertex(0));
    mTeethMesh.addNormal(mTeethMesh.getNormal(0));
    mTeethMesh.addVertex(mTeethMesh.getVertex(1));
    mTeethMesh.addNormal(mTeethMesh.getNormal(1));

    mBarTeethMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    mBarCenterMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    float barWidth = BAR_WIDTH;
    float cornerRadius = BAR_CORNER_RADIUS;

    vector<ofVec2f> bottomRidge;

    //left:
    float cornerSamples = 10;
    for(int i=0; i<cornerSamples; ++i) {
        float t = i / (float)cornerSamples;
        float angle = t * .5 * M_PI;
        ofVec2f dir(-cosf(angle), sinf(angle));
        ofVec2f p1(dir.x * cornerRadius, .5 * barWidth - (1 - dir.y) * cornerRadius);
        ofVec2f p2 = p1 + dir * toothTopOffset;

        ofVec2f q1(p1.x, -.5 * barWidth + (1 - dir.y) * cornerRadius);
        ofVec2f q2 = q1 + ofVec2f(dir.x, -dir.y) * toothTopOffset;

        mBarCenterMesh.addVertex(q1);
        mBarCenterMesh.addVertex(p1);
        mBarCenterMesh.addNormal(ofVec3f(0,0,1));
        mBarCenterMesh.addNormal(ofVec3f(0,0,1));

        mBarTeethMesh.addVertex(p1);
        mBarTeethMesh.addVertex(p2);
        mBarTeethMesh.addNormal(ofVec3f(0,0,1));
        mBarTeethMesh.addNormal(ofVec3f(0,0,1));

        bottomRidge.push_back(q2);
        bottomRidge.push_back(q1);
    }

    //push bottom two vertices to bottomRidge:
    bottomRidge.push_back(ofVec2f(0, -.5 * barWidth - toothTopOffset));
    bottomRidge.push_back(ofVec2f(0, -.5 * barWidth));

    //calculate tooth width:
    mToothWidth = 2 * M_PI * BASE_RADIUS / (float)TEETH;

    /*
    #define TEETH 12
#define BASE_RADIUS 130
    */
    


    //center:
    int barTeeth = BAR_TEETH;
    float toothWidth = mToothWidth;
    for(int i=0; i<barTeeth; ++i) {
        float startX = i * toothWidth;

        for(int j=0; j<30; ++j) {
            float t = j / 30.0;

            float offset = .5 * barWidth + getToothOffset(t);

            ofVec2f p(startX + t * toothWidth, offset);

            mBarCenterMesh.addVertex(ofVec2f(p.x, -.5 * barWidth));
            mBarCenterMesh.addVertex(p);

            mBarTeethMesh.addVertex(p);
            mBarTeethMesh.addVertex(p + ofVec2f(0, toothTopOffset));
        }
    }

    //right:
    float rightOffset = barTeeth * toothWidth;
    for(int i=0; i<cornerSamples; ++i) {
        float t = i / (float)(cornerSamples - 1);
        float angle = (1 - t) * .5 * M_PI;
        ofVec2f dir(cosf(angle), sinf(angle));
        ofVec2f p1(rightOffset + dir.x * cornerRadius, .5 * barWidth - (1 - dir.y) * cornerRadius);
        ofVec2f p2 = p1 + dir * toothTopOffset;

        ofVec2f q1(p1.x, -.5 * barWidth + (1 - dir.y) * cornerRadius);
        ofVec2f q2 = q1 + ofVec2f(dir.x, -dir.y) * toothTopOffset;

        mBarCenterMesh.addVertex(q1);
        mBarCenterMesh.addVertex(p1);

        mBarTeethMesh.addVertex(p1);
        mBarTeethMesh.addVertex(p2);

        bottomRidge.push_back(q2);
        bottomRidge.push_back(q1);
    }

    //append bottomRidge to teethmesh:
    for(int i=0; i<bottomRidge.size(); ++i) {
        mBarTeethMesh.addVertex(bottomRidge[bottomRidge.size() - 1 - i]);
        mBarTeethMesh.addNormal(ofVec3f(0,0,1));
    }

    //close meshes:
    mBarTeethMesh.addVertex(mBarTeethMesh.getVertex(0));
    mBarTeethMesh.addVertex(mBarTeethMesh.getVertex(1));

    //add normals:
    for(int i=0; i<mBarCenterMesh.getVertices().size(); ++i)
        mBarCenterMesh.addNormal(ofVec3f(0,0,1));
    for(int i=0; i<mBarTeethMesh.getVertices().size(); ++i)
        mBarTeethMesh.addNormal(ofVec3f(0,0,1));

    //calculate normals:
    n = mBarTeethMesh.getVertices().size();
    for(int i=0; i<n; i+=2) {
        ofVec2f prev = mBarTeethMesh.getVertex(i == 0 ? (n - 1) : (i - 1));
        ofVec2f next = mBarTeethMesh.getVertex((i + 3) % n);

        ofVec2f to = next - prev;
        to = to.normalize();

        mBarTeethMesh.setNormal(i + 1, ofVec3f(-to.y, to.x, 0));
    }

    // for(int i=0; i<mBarTeethMesh.getVertices().size(); ++i) {
    //     float t = i / (float)(mBarTeethMesh.getVertices().size() - 1);
    //     mBarTeethMesh.addColor(ofFloatColor(t, 0, 0));
    // }

    // for(int i=0; i<mBarCenterMesh.getVertices().size(); ++i) {
    //     float t = i / (float)(mBarCenterMesh.getVertices().size() - 1);
    //     mBarCenterMesh.addColor(ofFloatColor(0, t, 0));
    // }

}

BGCog::~BGCog() {
    
}

float BGCog::getToothOffset(float t) {

    float flat_t = 0.1;
    float curve_t = 0.35;
    float off_t = 1 - flat_t - curve_t;

    float offset = 0;
    if(t < flat_t || t > 1 - flat_t)
        offset = 0;
    else if(t < (flat_t + curve_t)) {
        t = (t - flat_t) / curve_t;
        offset = 0 + TOOTH_HEIGHT * (.5 - .5 * cosf(t * M_PI));
    }
    else if(t < off_t)
        offset = TOOTH_HEIGHT;
    else {
        t = (t - off_t) / curve_t;
        offset = 0 + TOOTH_HEIGHT * (.5 + .5 * cosf(t * M_PI));
    }
    return offset;
}

void BGCog::render(ofShader & mCogShader) {

    float barAngle = BAR_ANGLE;
    ofVec2f barTo(cosf(barAngle), sinf(barAngle));
    float calcWheelRotation = mWheelRotation + barAngle;

    mCogShader.begin();
    mCogShader.setUniform3f("uLightPosition", mPosition.x + 2 * OUTER_RADIUS - 1024 / 2.0, mPosition.y - 2 * OUTER_RADIUS - 768 / 2.0, 1.5 * OUTER_RADIUS);
    mCogShader.setUniform3f("uLightPosition", 300, -300, 300);
    mCogShader.setUniform1f("uRotation", calcWheelRotation);
    mCogShader.setUniformTexture("uBumpMap", mBumpImage.getTextureReference(), 0);
    mCogShader.setUniformTexture("uCellsTexture", bgResources.getImageReference(ObstacleImage2Key)->getTextureReference(), 1);
    mCogShader.setUniform1f("uRadius", OUTER_RADIUS);
    mCogShader.setUniform1f("uTime", mCellsAnimParam);
    mCogShader.setUniform1i("uBarMode", 0);
    ofPushMatrix();
    ofTranslate(mPosition);
    ofRotate(180 * calcWheelRotation / M_PI);

    mCogShader.setUniform1i("uShadowMode", 1);
    mTeethMesh.draw();
    mCogShader.setUniform1i("uShadowMode", 0);

    mCenterMesh.draw();
    mTeethMesh.draw();
    ofPopMatrix();


    ofPushMatrix();
    ofTranslate(mPosition + barTo * BAR_OFFSET);
    ofRotate(180 * barAngle / M_PI + 90);
    ofTranslate(mBarOffset, 0);
    mCogShader.setUniform1i("uBarMode", 1);
    mCogShader.setUniform1f("uRotation", barAngle + .5 * M_PI);
    mCogShader.setUniform1i("uShadowMode", 1);
    mBarTeethMesh.draw();
    mCogShader.setUniform1i("uShadowMode", 0);
    mBarTeethMesh.draw();
    mBarCenterMesh.draw();
    ofPopMatrix();

    mCogShader.end();
}

void BGCog::update(float dt) {
    mTimeAnimParam = fmodf(mTimeAnimParam + dt / 30.0, 1.0);
    mCellsAnimParam = fmodf(mCellsAnimParam + dt / 50.0, 1.0);

    mWheelRotation = 4 * sinf(3 * mTimeAnimParam * 2 * M_PI);
    float toothAngle = 2 * M_PI / (float)TEETH;
    float maxCw = toothAngle * (BAR_TEETH_CW);
    float maxCCw = -toothAngle * (BAR_TEETH_CCW);

    //clamp rotation:
    if(mWheelRotation < maxCCw)
        mWheelRotation = maxCCw;
    else if(mWheelRotation > maxCw)
        mWheelRotation = maxCw;

    //derive bar offset from rotation:
    mBarOffset = mToothWidth * (-BAR_TEETH_CW + mWheelRotation / toothAngle - .5);
}