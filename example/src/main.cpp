#include "testApp.h"
#include "ofAppGLFWWindow.h"

//--------------------------------------------------------------
int main(){
    ofGLWindowSettings settings;
    settings.setGLVersion(3,2); // higher versions are of course ok as well
    ofCreateWindow(settings); 
    ofRunApp(new testApp());
}
