#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    ofSetVerticalSync(false);
    m_w = ofGetWidth();
    m_h = ofGetHeight();
    float t = ofGetElapsedTimef();
    
    vector<glm::vec3> positions; vector<glm::vec4> colors;
    const int c = 15;
    for(int i=0; i<c; ++i) {
        positions.push_back(glm::vec3(ofRandom(m_w), ofRandom(m_h), ofRandom(-150,150)));
        colors.push_back(ofVec4f(1.0,1.0,1.0,ofNoise(i*0.01 + t * 0.3)));
    }
    vector< vector<size_t> > curves;

    // up to 20000 works fine at 60 FPS on my GTX680
    // after that we get fewer FPS regardless of curve resolution
    // which suggests that performance is bound by fill rate.
    for(int i=0; i<500; ++i) { 
        vector<size_t> curve;
        int j = ofRandom(3,7);
        for(int k=0; k<j; ++k) {
            curve.push_back(ofRandom(c));
        }
        curves.push_back(curve);
    }

    positions.push_back(glm::vec3(mouseX, mouseY, 0));
    colors.push_back(glm::vec4(1.0,1.0,1.0,1.0));
    m_mouseIdx = positions.size() - 1;
    for(int i=0; i<c; ++i) {
        for(int j=i+1; j<c; ++j) {
            vector<size_t> curve;
            curve.push_back(i); curve.push_back(m_mouseIdx); curve.push_back(j);
            curves.push_back(curve);
        }
    }

    printf("Got %d curves\n", curves.size());
    m_curves.setup(positions, colors, curves);
}

//--------------------------------------------------------------
void ofApp::update(){
    m_curves.beginUpdates();
    m_curves.updatePosition(m_mouseIdx, glm::vec3(mouseX, mouseY, 0));
    m_curves.endUpdates();

    // FPS
    {
        std::stringstream strm;
        strm << "fps: " << ofGetFrameRate();
        ofSetWindowTitle(strm.str());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofSetColor(255,50,10,255);
    ofEnableBlendMode(OF_BLENDMODE_SCREEN); {        
        m_cam.begin();
        ofPushMatrix();
        ofScale(1.0,-1.0,1.0);
        ofTranslate(-m_w / 2, -m_h / 2);
        {
            m_curves.draw();
        }
        ofPopMatrix();
        m_cam.end();
    } ofDisableBlendMode();
}

void ofApp::exit() {
    m_cam.disableMouseInput(); // prevents segfault
}
    

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
