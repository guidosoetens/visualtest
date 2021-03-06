#ifndef BGCELLGENERATOR_H
#define BGCELLGENERATOR_H

#include "ofMain.h"
#include "ofxOpenCv.h"

class BGCellGenerator {
	public:
    	BGCellGenerator();
		virtual ~BGCellGenerator();

        void draw();
        void update(float dt);

        void copyToImage(ofImage & goalImage);

    private:
        ofxCvColorImage mImage;
        ofxCvGrayscaleImage mHeightImage;

        float sampleHeight(float x, float y);
};

#endif