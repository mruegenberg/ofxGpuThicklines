#pragma once

#include "ofMain.h"
#include "ofxGpuThicklines.h"

class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void exit();
		
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofxGpuThicklines m_curves;
    size_t m_mouseIdx;
    ofEasyCam m_cam;

    float m_w;
    float m_h;
};
