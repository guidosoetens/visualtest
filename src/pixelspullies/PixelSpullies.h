#ifndef PIXELSPULLIES_H
#define PIXELSPULLIES_H

#include "ofMain.h"

class PixelSpullies {
	public:
		PixelSpullies();
		virtual ~PixelSpullies();
        void render();
        void setup();
        void process();

    private:
        ofShader mPainterShader;
        ofImage mMario;
        ofImage mLink;
};

#endif //PIXELSPULLIES_H
