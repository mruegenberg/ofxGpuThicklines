#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofEnableAntiAliasing();
    m_w = ofGetWidth();
    m_h = ofGetHeight();
    float t = ofGetElapsedTimef();
    
    vector<ofVec3f> positions; vector<float> opacities;
    const int c = 15;
    for(int i=0; i<c; ++i) {
        positions.push_back(ofVec3f(ofRandom(m_w), ofRandom(m_h), ofRandom(-150,150)));
        opacities.push_back(ofNoise(i*0.01 + t * 0.3));
    }
    vector< vector<size_t> > curves;

    // up to 20000 works fine at 60 FPS on my GTX680
    // after that we get fewer FPS regardless of curve resolution
    // which suggests that performance is bound by fill rate.
    for(int i=0; i<20000; ++i) { 
        vector<size_t> curve;
        int j = ofRandom(3,7);
        for(int k=0; k<j; ++k) {
            curve.push_back(ofRandom(c));
        }
        curves.push_back(curve);
    }

    positions.push_back(ofVec3f(mouseX, mouseY, 0));
    opacities.push_back(1.0);
    m_mouseIdx = positions.size() - 1;
    for(int i=0; i<c; ++i) {
        for(int j=i+1; j<c; ++j) {
            vector<size_t> curve;
            curve.push_back(i); curve.push_back(m_mouseIdx); curve.push_back(j);
            curves.push_back(curve);
        }
    }

    printf("Got %d curves\n", curves.size());
    m_curves.setup(positions, opacities, curves);
}

//--------------------------------------------------------------
void testApp::update(){
    m_curves.beginUpdates();
    m_curves.updatePosition(m_mouseIdx, ofVec3f(mouseX, mouseY, 0));
    m_curves.endUpdates();

    // FPS
    {
        std::stringstream strm;
        strm << "fps: " << ofGetFrameRate();
        ofSetWindowTitle(strm.str());
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0);
    ofSetColor(255,50,10,255);
    ofEnableBlendMode(OF_BLENDMODE_SCREEN); {        
        m_cam.begin();
        ofPushMatrix();
        ofScale(1.0,-1.0,1.0);
        ofTranslate(-m_w / 2, -m_h / 2);
        {
            m_curves.draw();
            m_curves.drawVertices();
        }
        ofPopMatrix();
        m_cam.end();
    } ofDisableBlendMode();
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
