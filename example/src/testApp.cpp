#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    float w = ofGetWidth();
    float h = ofGetHeight();
    float t = ofGetElapsedTimef();
    
    vector<ofVec3f> positions; vector<float> opacities;
    const int c = 15;
    for(int i=0; i<c; ++i) {
        positions.push_back(ofVec3f(ofRandom(w), ofRandom(h), 0));
        opacities.push_back(ofNoise(i*0.01 + t * 0.3));
    }
    vector< vector<size_t> > curves;
    for(int i=0; i<200; ++i) {
        vector<size_t> curve;
        int j = ofRandom(3,7);
        for(int k=0; k<j; ++k) {
            curve.push_back(ofRandom(c));
        }
        curves.push_back(curve);
    }

    positions.push_back(ofVec3f(mouseX, mouseY, 0));
    opacities.push_back(1.0);
    size_t mouseIdx = positions.size() - 1;
    for(int i=0; i<c; ++i) {
        for(int j=i+1; j<c; ++j) {
            vector<size_t> curve;
            curve.push_back(i); curve.push_back(mouseIdx); curve.push_back(j);
        }
    }
    
    m_curves.setup(positions, opacities, curves);
}

//--------------------------------------------------------------
void testApp::update(){
    ofEnableAntiAliasing();
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0);
    ofSetColor(255,255,255,255);
    m_curves.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
