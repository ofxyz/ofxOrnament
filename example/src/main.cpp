#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

	
	// Create OpenGL window with the following parameters openGL 3.2, 1024x768, windowed
	ofGLFWWindowSettings settings;
	settings.setGLVersion(4, 6);
	settings.setSize(1024, 768);

	settings.windowMode = OF_WINDOW;
	ofCreateWindow(settings);
	ofSetWindowTitle("ofxOrnament");
	// Run the application
	ofRunApp( new ofApp());

}
