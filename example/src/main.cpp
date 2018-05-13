#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//--------------------------------------------------------------
int main(){
    // ofGLFWWindowSettings settings;
    ofGLWindowSettings settings;
    settings.setSize(1280, 960);
    settings.windowMode = OF_WINDOW;
    settings.setGLVersion(3,2); // use this for native buffer objects etc
    ofCreateWindow(settings);
    

    ofRunApp(new ofApp());
}
