/* Copyright (C) Active Cues - All Rights Reserved
  * Unauthorized copying of this file, via any medium is strictly prohibited
  * Proprietary and confidential
  *
  * Original author <Unknown> <**@activecues.com>, May 2016
*/

#ifndef Uitprobeersel_h
#define Uitprobeersel_h

#include "ofMain.h"
#include "ofxCvGrayscaleImage.h"

class Uitprobeersel {
public:

    Uitprobeersel();
    ~Uitprobeersel();

    void update(float dt);
    void draw();
    void reload();
private:
    ofShader mShader;
    ofxCvGrayscaleImage mTexImage;
    float mTimeParameter;
};

#endif
