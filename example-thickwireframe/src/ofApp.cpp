#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    ofSetVerticalSync(true);
    m_w = ofGetWidth();
    m_h = ofGetHeight();

    sphere = ofMesh::sphere(500, 12, OF_PRIMITIVE_TRIANGLES); // already has tex coords.

    string fragShader = ("#version 150\n"
                         "\n"
                         "float rand(vec2 co){\n"
                         "    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);\n"
                         "}"
                         "\n"
                         "flat in int edgeID;\n"
                         "in vec4 fColorVarying;\n"
                         "in vec2 fTexCoordVarying;\n"
                         "in vec2 flocalTexCoord;\n"
                         "uniform vec4 globalColor;\n"
                         "uniform float time;\n"
                         "\n"
                         "out vec4 outputColor;\n"
                         "\n"
                        "float sawtooth(float x) { return (x - floor(x)); }"
                         "void main()\n"
                         "{\n"
                         "    float m = (sin(time * 1.5 + 64332.23 * rand( vec2(float(edgeID),44322.123) )) * 0.5 + 0.5);\n"
                              " m = sawtooth(time * 0.5); "
                         " float sparksize = 0.1;"
                         " float f = (flocalTexCoord.x - (m - sparksize)) / (sparksize);"
                         " float n = clamp(f > 1 ? 0 : f, 0, 1);"
                         "float movingspark = n;"
                         "    outputColor = globalColor * fColorVarying * movingspark;\n"
                         "}\n");
                         
    m_curves.setup(sphere, fragShader, true);
}

//--------------------------------------------------------------
void ofApp::update(){
    auto mousePos = glm::vec3( (float)ofGetMouseX(), (float)ofGetMouseY(), -1000);
    m_curves.beginUpdates();
    m_curves.updatePosition(m_mouseIdx, mousePos);
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
    ofBackgroundGradient(ofColor(60), ofColor(10));
    ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    {
        m_cam.begin();
        {
            ofSetColor(255,50,10,255);

            ofShader s = m_curves.prepareDraw();
            s.setUniform1f("time", ofGetElapsedTimef());
            
            m_curves.draw();
            //  m_curves.drawVertices();
            ofColor c = ofColor::white;
            c.a = 10;
            ofSetColor(c);
            // sphere.drawWireframe();
        }
        m_cam.end();
    }
    ofDisableBlendMode();
    ofDisableAlphaBlending();
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
