#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    ofSetVerticalSync(true);
    m_w = ofGetWidth();
    m_h = ofGetHeight();
    float t = ofGetElapsedTimef();

    sphere = ofMesh::sphere(500, 12, OF_PRIMITIVE_TRIANGLES);
    m_curves.setup(sphere);
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
    ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_SCREEN); {        
        m_cam.begin();
        {
            ofSetColor(255,50,10,255);
            m_curves.draw();
            //  m_curves.drawVertices();
            ofColor c = ofColor::white;
            c.a = 10;
            ofSetColor(c);
            // sphere.drawWireframe();
        }
        m_cam.end();
    } ofDisableBlendMode();
    ofDisableAlphaBlending();
}

void testApp::exit() {
    m_curves.exit();
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
