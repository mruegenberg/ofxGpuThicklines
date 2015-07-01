#include "ofxGPUCurves.h"
#include <cassert>

#define STRINGIFY(A) #A

void ofxGPUCurves::setup(vector<ofVec3f> positions,
                         vector<float> opacities,
                         vector< vector<size_t> > curves,
                         ofShader pointShader) {
    // curve shader
    {
        // generate a Catmull-Rom spline from line adjacency
        // adapted from OpenFrameworks
        string geomShader = ("#version 150 core\n"
                             "#define RESOLUTION 10\n" // will rarely need more than 10
                             "\n"
                             "in float pOpacity[];\n"
                             "layout(lines_adjacency) in;\n"
                             "layout(line_strip, max_vertices = RESOLUTION) out;\n"
                             "\n"
                             "out float fOpacity;\n"
                             "\n"
                             "void main() {"
                             "    fOpacity = pOpacity[0];\n"
                             "    float x0 = gl_in[0].gl_Position.x;\n"
                             "    float y0 = gl_in[0].gl_Position.y;\n"
                             "    float x1 = gl_in[1].gl_Position.x;\n"
                             "    float y1 = gl_in[1].gl_Position.y;\n"
                             "    float x2 = gl_in[2].gl_Position.x;\n"
                             "    float y2 = gl_in[2].gl_Position.y;\n"
                             "    float x3 = gl_in[3].gl_Position.x;\n"
                             "    float y3 = gl_in[3].gl_Position.y;\n"
                             "\n"
                             "    float z1 = gl_in[1].gl_Position.z;\n"
                             "    float z2 = gl_in[2].gl_Position.z;\n"
                             "\n"
                             "    float w1 = gl_in[1].gl_Position.w;\n"
                             "\n"
                             "    float t,t2,t3;\n"
                             "    float x,y,z;\n"
                             "\n"
                             "    for(int i=0; i<RESOLUTION; ++i) {\n"
                             "        float j = i;\n"
                             "        float t = j / (RESOLUTION - 1);\n"
                             "        t2 = t * t;\n"
                             "        t3 = t2 * t;\n"
                             "\n"
                             "        x = 0.5f * ( ( 2.0f * x1 ) +"
                             "                      ( -x0 + x2 ) * t +"
                             "                      ( 2.0f * x0 - 5.0f * x1 + 4 * x2 - x3 ) * t2 +\n"
                             "                      ( -x0 + 3.0f * x1 - 3.0f * x2 + x3 ) * t3 );\n"
                             "        y = 0.5f * ( ( 2.0f * y1 ) +\n"
                             "                     ( -y0 + y2 ) * t +\n"
                             "                     ( 2.0f * y0 - 5.0f * y1 + 4 * y2 - y3 ) * t2 +\n"
                             "                     ( -y0 + 3.0f * y1 - 3.0f * y2 + y3 ) * t3 );\n"
                             "        z = mix(z1,z2,t);\n"
                             ""
                             "        gl_Position = vec4(x,y,z,w1);\n"
                             "        EmitVertex();\n"
                             "    }\n"
                             "\n"
                             "    EndPrimitive();\n"
                             "}");
        
        string vertShader = ("#version 150\n"
                             "\n"
                             "uniform mat4 modelViewProjectionMatrix;\n"
                             "in vec4 position;\n"
                             "in float opacity;\n"
                             "\n"
                             "out float pOpacity;\n"
                             "\n"
                             "void main()\n"
                             "{\n"
                             "    gl_Position = modelViewProjectionMatrix * position;\n"
                             "    pOpacity = opacity;\n"
                             "}\n"
            );
        
        string fragShader = ("#version 150\n"
                             "\n"
                             "in float fOpacity;\n"
                             "uniform vec4 globalColor;\n"
                             "\n"
                             "out vec4 outputColor;\n"
                             "\n"
                             "void main()\n"
                             "{\n"
                             "    outputColor = globalColor * fOpacity;\n"
                             "}\n"
            );

        m_curvesShader.unload();
        m_curvesShader.setupShaderFromSource(GL_GEOMETRY_SHADER, geomShader);
        m_curvesShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShader);
        m_curvesShader.setupShaderFromSource(GL_VERTEX_SHADER, vertShader);
        m_curvesShader.linkProgram();
    }

    m_customPointShader = true;
    m_pointShader = pointShader;

    reset(positions, opacities, curves);
}

void ofxGPUCurves::setup(vector<ofVec3f> positions,
                         vector<float> opacities,
                         vector< vector<size_t> > curves) {
    // points shader
    ofShader pointShader;
    {
        string geomShader = ("#version 150 core\n"
                             "\n"    
                             "uniform float aspectRatio;\n"
                             "uniform float radius;\n"
                             "in float pOpacity[];\n"
                             "layout(points) in;\n"
                             "layout(triangle_strip, max_vertices = 4) out;\n"
                             "\n"
                             "out vec2 texCoordVarying;\n"
                             "out float fOpacity;\n"
                             "\n"
                             "void main() {\n"
                             "    float dx = 1;\n"
                             "    float dy = aspectRatio;\n"
                             "\n"    
                             "    fOpacity = pOpacity[0];\n"
                             "\n"
                             "    texCoordVarying = vec2(0,0);\n"
                             "    gl_Position = gl_in[0].gl_Position + vec4(-dx, -dy, 0.0, 0.0) * radius;\n"
                             "    EmitVertex();\n"
                             "\n"    
                             "    texCoordVarying = vec2(0,1);\n"
                             "    gl_Position = gl_in[0].gl_Position + vec4(-dx, dy, 0.0, 0.0) * radius;\n"
                             "    EmitVertex();\n"
                             "\n"    
                             "    texCoordVarying = vec2(1,0);\n"
                             "    gl_Position = gl_in[0].gl_Position + vec4(dx, -dy, 0.0, 0.0) * radius;\n"
                             "    EmitVertex();\n"
                             "\n "   
                             "    texCoordVarying = vec2(1,1);\n"
                             "    gl_Position = gl_in[0].gl_Position + vec4(dx, dy, 0.0, 0.0) * radius;\n"
                             "    EmitVertex();\n"
                             "\n"    
                             "    EndPrimitive();\n"
                             "}\n"
            );
        
        string vertShader = ("#version 150\n"
                             "\n"         
                             "uniform mat4 modelViewProjectionMatrix;\n"
                             "in vec4 position;\n"
                             "in float opacity;\n"
                             "\n"
                             "out float pOpacity;\n"
                             "\n"
                             "void main()\n"
                             "{\n"
                             "    gl_Position = modelViewProjectionMatrix * position;\n"
                             "    pOpacity = opacity;\n"
                             "}\n"
            );

        string fragShader = ("#version 150\n"
                             "\n"
                             "uniform float radius;\n"
                             "in float fOpacity;\n"
                             "in vec2 texCoordVarying;\n"
                             "const highp vec2 center = vec2(0.5, 0.5);\n"
                             "const highp float txradius = 0.5;\n"
                             "const highp float border = 0.2;\n"
                             "uniform vec4 globalColor;\n"
                             "\n"
                             "out vec4 outputColor;\n"
                             "\n"
                             "void main()\n"
                             "{\n"
                             "    highp float distanceFromCenter = distance(center, texCoordVarying);\n"
                             "\n"
                             "    lowp float circleTest = smoothstep(txradius, txradius * border, distanceFromCenter);\n"
                             "    outputColor = globalColor * fOpacity * circleTest;\n"
                             "}\n"
            );
        
        pointShader.unload();
        pointShader.setupShaderFromSource(GL_GEOMETRY_SHADER, geomShader);
        pointShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShader);
        pointShader.setupShaderFromSource(GL_VERTEX_SHADER, vertShader);
        pointShader.linkProgram();
    }

    setup(positions, opacities, curves, pointShader);
    m_customPointShader = false;
}

void ofxGPUCurves::reset(vector<ofVec3f> positions,
                         vector<float> opacities,
                         vector< vector<size_t> > curves) {
    assert(positions.size() == opacities.size());

    m_positions = positions;
    m_opacities = opacities;

    {
        m_curvesVbo.clear();
        m_curvesVbo.setVertexData(&m_positions[0], m_positions.size(), GL_DYNAMIC_DRAW);

        // construct adjacency indices suitable for OpenGL from curve data
        vector<unsigned int> indices;
        for(size_t i=0; i<curves.size(); ++i) {
            vector<size_t> &conn = curves[i];
            if(conn.empty()) continue;

            // pack beginning and end of `conn` twice into `connIndices`
            // so that the curve goes through them
            // TODO: maybe not do this to be more like ofCurveVertices()?
            vector<size_t> connIndices;
            connIndices.push_back(conn[0]);
            connIndices.insert(connIndices.begin() + 1, conn.begin(), conn.end());
            connIndices.push_back(conn[conn.size() - 1]);
            
            if(connIndices.size() < 4) continue;
            for(size_t i=0; i+3<connIndices.size(); ++i) {
                indices.push_back(connIndices[i]);
                indices.push_back(connIndices[i+1]);
                indices.push_back(connIndices[i+2]);
                indices.push_back(connIndices[i+3]);
            }
        }
        
        m_curvesVbo.setIndexData(&indices[0], indices.size(), GL_STATIC_DRAW);
        m_indexCount = indices.size();
        
        m_curvesVbo.setAttributeData(m_curvesShader.getAttributeLocation("opacity"),
                                     &m_opacities[0], 1, m_opacities.size(), GL_DYNAMIC_DRAW);
    }

    {
        m_pointsVbo.clear();
        m_pointsVbo.setVertexData(&m_positions[0], m_positions.size(), GL_DYNAMIC_DRAW);
        m_pointsVbo.setAttributeData(m_pointShader.getAttributeLocation("opacity"),
                                     &m_opacities[0], 1, m_opacities.size(), GL_DYNAMIC_DRAW);
    }
}


void ofxGPUCurves::beginUpdates() {
    ; // we do nothing. this method is, for now, only to provide a more logical API.
}


void ofxGPUCurves::endUpdates() {
    m_curvesVbo.updateVertexData(&m_positions[0], m_positions.size());
    m_curvesVbo.updateAttributeData(m_curvesShader.getAttributeLocation("opacity"),
                                    &m_opacities[0], m_opacities.size());

    m_pointsVbo.updateVertexData(&m_positions[0], m_positions.size());
    m_pointsVbo.updateAttributeData(m_pointShader.getAttributeLocation("opacity"),
                                    &m_opacities[0], m_opacities.size());
}


void ofxGPUCurves::draw() {
    m_curvesShader.begin();
    m_curvesVbo.drawElements(GL_LINES_ADJACENCY, m_indexCount);
    m_curvesShader.end();
}


void ofxGPUCurves::drawVertices(float radius) {
    m_pointShader.begin();
    
    if(! m_customPointShader) {
        m_pointShader.setUniform1f("radius", radius);
        m_pointShader.setUniform1f("aspectRatio", ofGetWidth() / (float)ofGetHeight());
    }
    
    m_pointsVbo.draw(GL_POINTS, 0, m_positions.size());

    m_pointShader.end();
}

