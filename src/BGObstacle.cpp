#import "BGObstacle.h"
#import "BGResources.h"

BGObstacle::BGObstacle(ofVec2f pos, float rad, int reps, bool stretch)
:   mPosition(pos)
,   mObstacleTimeParameter(0)
{
    mMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

    mMesh.addVertex(ofVec3f(0,0,0));
    mMesh.addNormal(ofVec3f(0,0,1));
    mMesh.addTexCoord(ofVec2f(1,0));

    ofVec2f center(0,0);

    for(int i=0; i<reps; ++i) {

        float ang1 = i / (float)reps * 2 * M_PI;
        ofVec2f to1 = ofVec2f(cosf(ang1), sinf(ang1));

        float ang2 = (i + 1) / (float)reps * 2 * M_PI;
        ofVec2f to2 = ofVec2f(cosf(ang2), sinf(ang2));


        ofVec2f p0 = rad * to1;
        ofVec2f p3 = rad * to2;

        ofVec2f p1 = p0 + .2 * rad * ofVec2f(-to1.y, to1.x);
        ofVec2f p2 = p3 - .2 * rad * ofVec2f(-to2.y, to2.x);

        if(i < reps / 2 && stretch) {
            p0.y *= 2.0;
            p1.y *= 2.0;
            p2.y *= 2.0;
            p3.y *= 2.0;
        }

        center = center + p0;

        for(int it=0; it<20; ++it) {
            
            float t = it / 19.0;
            float tMin = 1.0 - t;
            float tt = t * t;
            float ttMin = tMin * tMin;

            ofVec2f p =     tMin * ttMin * p0 
                        +   3 * t * ttMin * p1 
                        +   3 * tt * tMin * p2 
                        +   t * tt * p3;

            mMesh.addVertex(ofVec3f(p.x, p.y,0));

            p = p.normalize();
            mMesh.addNormal(ofVec3f(p.x,p.y,0));
            mMesh.addTexCoord(ofVec2f(-5 / rad,0));
        }
    }

    mMesh.setVertex(0, center / reps);

    //https://www.shutterstock.com/g/CurlyPat/sets/569483
    //https://www.shutterstock.com/image-vector/vector-seamless-pattern-horizontal-spots-monochrome-203315401
}

BGObstacle::~BGObstacle() {

}
        
void BGObstacle::render(ofShader & mObstacleShader, ofImage & mBumpMap, int width, int height) {

    ofImage* spotImage = bgResources.getImageReference(ObstacleImageKey);
    // spotImage->draw(-100,-100,10,10);

    ofFloatColor darkColor = bgResources.getColorSetting(ObstacleDarkColorKey)->value;
    ofFloatColor lightColor = bgResources.getColorSetting(ObstacleLightColorKey)->value;
    ofFloatColor highlightColor = bgResources.getColorSetting(ObstacleHighlightColorKey)->value;
    // uniform vec3 uDarkColor;
    // uniform vec3 uLightColor;
    // uniform vec3 uHighlightColor;

    mObstacleShader.begin();
    mObstacleShader.setUniformTexture("uTexture", mBumpMap.getTextureReference(), 0);
    mObstacleShader.setUniform2f("uResolution", width, height);
    mObstacleShader.setUniform1f("uTime", mObstacleTimeParameter);
    mObstacleShader.setUniform4f("uBaseColor", 1, 0, .4, 1);
    mObstacleShader.setUniformTexture("uSpotTexture", spotImage->getTextureReference(), 1);
    mObstacleShader.setUniform3f("uDarkColor", darkColor.r, darkColor.g, darkColor.b);
    mObstacleShader.setUniform3f("uLightColor", lightColor.r, lightColor.g, lightColor.b);
    mObstacleShader.setUniform3f("uHighlightColor", highlightColor.r, highlightColor.g, highlightColor.b);
    ofPushMatrix();
    ofTranslate(mPosition.x, mPosition.y);
    mMesh.draw();
    ofPopMatrix();
    mObstacleShader.end();
}

void BGObstacle::update(float dt) {
     mObstacleTimeParameter = fmodf(mObstacleTimeParameter + dt / 100.0, 1.0);
}