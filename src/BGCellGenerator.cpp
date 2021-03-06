#include "BGCellGenerator.h"

#define TEX_WIDTH 256
#define MAX_SAMPLE_HEIGHT .1
//0.15
// 0.5
#define TEX_BUFFER_SIZE (TEX_WIDTH * TEX_WIDTH)

BGCellGenerator::BGCellGenerator() {

    float scale = .5 * tanf(M_PI / 3.0);


    unsigned char heightMap[TEX_BUFFER_SIZE];
    mImage.allocate(TEX_WIDTH, TEX_WIDTH);
    unsigned char* pixels = mImage.getPixels();

    mHeightImage.allocate(TEX_WIDTH, TEX_WIDTH);

    //create height map:
    for(int i=0; i<TEX_WIDTH; ++i) {
        float y = 2.0 * (i / (float)(TEX_WIDTH - 1) - .5);
        for(int j=0; j<TEX_WIDTH; ++j) {

            float x = 2.0 * scale * (j / (float)(TEX_WIDTH - 1) - .5);

            float deltaSample = .001;

            float h = sampleHeight(x, y);
            float dhx = sampleHeight(x + deltaSample, y) - sampleHeight(x - deltaSample, y);
            float dhy = sampleHeight(x, y + deltaSample) - sampleHeight(x, y - deltaSample);

            ofVec3f u(2 * deltaSample, 0, dhx);
            ofVec3f v(0, 2 * deltaSample, dhy);
            ofVec3f normal = u.cross(v);
            normal.normalize();

            int idx = 3 * (i * TEX_WIDTH + j);
            float rgb[3] = { .5 + .5 * normal.x, .5 - .5 * normal.y, normal.z };
            for(int c=0; c<3; ++c)
                pixels[idx + c] = rgb[c] * 255;

            idx = i * TEX_WIDTH + j;
            heightMap[idx] = 255 * sampleHeight(x, y) / MAX_SAMPLE_HEIGHT;
        }
    }

    mImage.setFromPixels(pixels, TEX_WIDTH, TEX_WIDTH);
    mImage.updateTexture();

    mHeightImage.setFromPixels(heightMap, TEX_WIDTH, TEX_WIDTH);
    mHeightImage.updateTexture();

}

BGCellGenerator::~BGCellGenerator() {

}

void BGCellGenerator::copyToImage(ofImage & goalImage) {
    goalImage.allocate(mImage.getWidth(), mImage.getHeight(), OF_IMAGE_COLOR);
    goalImage.setFromPixels(mImage.getPixels(), mImage.getWidth(), mImage.getHeight(), OF_IMAGE_COLOR);
}

float 
BGCellGenerator::sampleHeight(float x, float y) {

    ofVec2f to(x, y);
    float len = to.length();
    float ang = atan2f(y, x);

    float base = .8;

    //make blob star:
    //base = .8 - 0.05 * cosf(6 * ang);

    //make regular star:
    base = .8 - 0.02 * cosf(6 * ang);

    //make circle:
    //base = .8;

    float t = fminf(1.0, powf(len / base, 1.0));

    if(t < .5)
        t = 0;
    else t = (t - .5) / .5;

    return (.5 + .5 * cosf(t * M_PI)) * MAX_SAMPLE_HEIGHT;
}

void BGCellGenerator::draw() {

    float halfTexWidth = TEX_WIDTH / 2.0;

    ofVec2f size(1024, 768);
    ofVec2f center = size * 0.5;

    float scale = .5 * tanf(M_PI / 3.0);


    ofSetColor(255);
    mImage.draw(center.x - scale * halfTexWidth, center.y - halfTexWidth, scale * TEX_WIDTH, TEX_WIDTH);

    mHeightImage.draw(center.x - 4 * scale * halfTexWidth - 10, center.y - halfTexWidth, scale * TEX_WIDTH, TEX_WIDTH);



    //render hex:
    ofSetColor(255,0,0);
    ofSetLineWidth(1);
    ofVec2f prevPos = center + ofVec2f(0, halfTexWidth);
    for(int i=0; i<6; ++i) {
        float nextAng = (.5 + (i + 1) / 3.0) * M_PI;
        ofVec2f currPos = center + halfTexWidth * ofVec2f(cosf(nextAng), sinf(nextAng));
        ofLine(prevPos, currPos);
        prevPos = currPos;
    }

}

void BGCellGenerator::update(float dt) {

}