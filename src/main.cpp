#include "ofMain.h"
#include "ofApp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/input.h>
#include <fcntl.h>

//========================================================================
int main( ){
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context
	ofSetWindowPosition(1920 + (1920 - 1024) / 2, (1080 - 768) / 2);
	ofSetWindowPosition(1920, 0);
	ofSetWindowPosition(0,0);
 	//ofSetWindowPosition(1920 + (1920 - 1024) / 2, (1080 - 768) / 2);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:

	ofRunApp(new ofApp());

}
