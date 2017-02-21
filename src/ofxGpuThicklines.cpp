#include "ofxGpuThicklines.h"
#include <cassert>

void ofxGpuThicklines::setup(vector<ofVec3f> positions,
                             vector<ofVec4f> colors,
                             vector< vector<size_t> > curves,
                             ofShader pointShader) {
    // curve shader
    {
        string geomShader = ("#version 150 core\n"
                             "\n"
                             "#define THICKNESS 3\n" //"uniform float	THICKNESS;\n" // the thickness of the line in pixels
                             "#define MITER_LIMIT 0.75\n" // "uniform float	MITER_LIMIT;\n" // 1.0: always miter, -1.0: never miter, 0.75: default
                             "#define WIN_SCALE vec2(1024,768)\n" // "uniform vec2	WIN_SCALE;\n"// the size of the viewport in pixels
                             "\n"
                             "in vec4 colorVarying[];\n"
                             "in vec2 texCoordVarying[];\n"
                             "\n"
                             "layout(lines_adjacency) in;\n"
                             "layout(triangle_strip, max_vertices = 7) out;\n"
                             "\n"
                             "\n" // end of next segment
                             "out vec2 fTexCoordVarying;\n"
                             "out vec4 fColorVarying;\n"
                             "\n"
                             "vec2 screen_space(vec4 vertex) {\n"
                             "    return vec2( vertex.xy / vertex.w ) * WIN_SCALE;\n"
                             "}\n"
                             "\n"
                             "void main(void)\n"
                             "{\n"
                             "    \n" // get the four vertices passed to the shader:
                             "    vec2 p0 = screen_space( gl_in[0].gl_Position );\n" // start of previous segment
                             "    vec2 p1 = screen_space( gl_in[1].gl_Position );\n" // end of previous segment, start of current segment
                             "    vec2 p2 = screen_space( gl_in[2].gl_Position );\n" // end of current segment, start of next segment
                             "    vec2 p3 = screen_space( gl_in[3].gl_Position );\n" // end of next segment
                             "\n"
                             "    \n" // determine the direction of each of the 3 segments (previous, current, next)
                             "    vec2 v0 = normalize(p1-p0);\n"
                             "    vec2 v1 = normalize(p2-p1);\n"
                             "    vec2 v2 = normalize(p3-p2);\n"
                             "\n"
                             "    \n" // determine the normal of each of the 3 segments (previous, current, next)
                             "    vec2 n0 = vec2(-v0.y, v0.x);\n"
                             "    vec2 n1 = vec2(-v1.y, v1.x);\n"
                             "    vec2 n2 = vec2(-v2.y, v2.x);\n"
                             "\n"
                             "    \n" // determine miter lines by averaging the normals of the 2 segments
                             "    vec2 miter_a = normalize(n0 + n1);\n" // miter at start of current segment
                             "    vec2 miter_b = normalize(n1 + n2);\n" // miter at end of current segment
                             "\n"
                             "    \n" // determine the length of the miter by projecting it onto normal and then inverse it
                             "    float length_a = THICKNESS / dot(miter_a, n1);\n"
                             "    float length_b = THICKNESS / dot(miter_b, n1);\n"
                             "\n"

                             "    // prevent excessively long miters at sharp corners\n"
                             "    if( dot(v0,v1) < -MITER_LIMIT ) {\n"
                             "	miter_a = n1;\n"
                             "	length_a = THICKNESS;\n"
                             "\n"
                             "        float f = sign(dot(v0,n1));\n"
                             "        float g = f > 0 ? 0 : 1;\n"
                             "\n"
                             "        fTexCoordVarying = vec2(0, g); // TODO: use tex coord from vertices\n"
                             "        fColorVarying = colorVarying[1];\n"
                             "        gl_Position = vec4( (p1 + THICKNESS * mix(n0,n1,g) * f) / WIN_SCALE, 0.0, 1.0 );\n"
                             "        EmitVertex();\n"
                             "\n"
                             "        fTexCoordVarying = vec2(0, g); \n"
                             "        fColorVarying = colorVarying[1];\n"
                             "        gl_Position = vec4( (p1 + THICKNESS * mix(n1,n0,g) * f) / WIN_SCALE, 0.0, 1.0 );\n"
                             "        EmitVertex();\n"
                             "\n"
                             "        fTexCoordVarying = vec2(0, 0.5);\n"
                             "        fColorVarying = colorVarying[1];\n"
                             "        gl_Position = vec4( p1 / WIN_SCALE, 0.0, 1.0 );\n"
                             "        EmitVertex();\n"
                             "        EndPrimitive();\n"
                             "    }\n"
                             "\n"
                             "    if( dot(v1,v2) < -MITER_LIMIT ) {\n"
                             "	      miter_b = n1;\n"
                             "	      length_b = THICKNESS;\n"
                             "    }\n"
                             "  \n"
                             "    \n" // generate the triangle strip
                             "    fTexCoordVarying = vec2(0, 0);\n"
                             "    fColorVarying = colorVarying[1];\n"
                             "    gl_Position = vec4( (p1 + length_a * miter_a) / WIN_SCALE, 0.0, 1.0 );\n"
                             "    EmitVertex();\n"
                             "  \n"
                             "    fTexCoordVarying = vec2(0, 1);\n"
                             "    fColorVarying = colorVarying[1];\n"
                             "    gl_Position = vec4( (p1 - length_a * miter_a) / WIN_SCALE, 0.0, 1.0 );\n"
                             "    EmitVertex();\n"
                             "  \n"
                             "    fTexCoordVarying = vec2(0, 0);\n"
                             "    fColorVarying = colorVarying[2];\n"
                             "    gl_Position = vec4( (p2 + length_b * miter_b) / WIN_SCALE, 0.0, 1.0 );\n"
                             "    EmitVertex();\n"
                             "  \n"
                             "    fTexCoordVarying = vec2(0, 1);\n"
                             "    fColorVarying = colorVarying[2];\n"
                             "    gl_Position = vec4( (p2 - length_b * miter_b) / WIN_SCALE, 0.0, 1.0 );\n"
                             "    EmitVertex();\n"
                             "\n"
                             "    EndPrimitive();\n"
                             "}\n");
        
        string vertShader = ("#version 150\n"
                             "\n"
                             "uniform mat4 modelViewProjectionMatrix;\n"
                             "uniform mat4 textureMatrix;\n"
                             "in vec4 position;\n"
                             "in vec4 color;\n"
                             "in vec2 texcoord;\n"
                             "\n"
                             "out vec4 colorVarying;\n"
                             "out vec2 texCoordVarying;\n"
                             "\n"
                             "void main()\n"
                             "{\n"
                             "    gl_Position = modelViewProjectionMatrix * position;\n"
                             "    colorVarying = color;\n"
                             "    vec2 drawTexCoord = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;\n"
                             "    texCoordVarying = drawTexCoord;\n"
                             "}\n"
            );
        
        string fragShader = ("#version 150\n"
                             "\n"
                             "in vec4 fColorVarying;\n"
                             "in vec2 fTexCoordVarying;\n"
                             "uniform vec4 globalColor;\n"
                             "\n"
                             "out vec4 outputColor;\n"
                             "\n"
                             "void main()\n"
                             "{\n"
                             "    outputColor = globalColor * fColorVarying;\n"
                             "}\n"
            );

        if(m_curvesShader.isLoaded())
            m_curvesShader.unload();
        m_curvesShader.setupShaderFromSource(GL_GEOMETRY_SHADER, geomShader);
        m_curvesShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShader);
        m_curvesShader.setupShaderFromSource(GL_VERTEX_SHADER, vertShader);
        m_curvesShader.linkProgram();
    }

    m_customPointShader = true;
    m_pointShader = pointShader;

    reset(positions, colors, curves);
}

void ofxGpuThicklines::setup(vector<ofVec3f> positions,
                         vector<ofVec4f> colors,
                         vector< vector<size_t> > curves) {
    // points shader
    ofShader pointShader;
    {
        string geomShader = ("#version 150 core\n"
                             "\n"    
                             "uniform float aspectRatio;\n"
                             "uniform float radius;\n"
                             "in vec4 colorVarying[];\n"
                             "layout(points) in;\n"
                             "layout(triangle_strip, max_vertices = 4) out;\n"
                             "\n"
                             "out vec2 texCoordVarying;\n"
                             "out vec4 fColorVarying;\n"
                             "\n"
                             "void main() {\n"
                             "    float dx = 1;\n"
                             "    float dy = aspectRatio;\n"
                             "\n"    
                             "    fColorVarying = colorVarying[0];\n"
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
                             "in vec4 color;\n"
                             "\n"
                             "out vec4 colorVarying;\n"
                             "\n"
                             "void main()\n"
                             "{\n"
                             "    gl_Position = modelViewProjectionMatrix * position;\n"
                             "    colorVarying = color;\n"
                             "}\n"
            );

        string fragShader = ("#version 150\n"
                             "\n"
                             "uniform float radius;\n"
                             "in vec4 fColorVarying;\n"
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
                             "    outputColor = globalColor * fColorVarying * circleTest;\n"
                             "}\n"
            );

        if(pointShader.isLoaded())
            pointShader.unload();
        pointShader.setupShaderFromSource(GL_GEOMETRY_SHADER, geomShader);
        pointShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShader);
        pointShader.setupShaderFromSource(GL_VERTEX_SHADER, vertShader);
        pointShader.linkProgram();
    }

    setup(positions, colors, curves, pointShader);
    m_customPointShader = false;
}

void ofxGpuThicklines::reset(vector<ofVec3f> positions,
                         vector<ofVec4f> colors,
                         vector< vector<size_t> > curves) {
    assert(positions.size() == colors.size());

    m_positions = positions;
    m_colors = colors;

    {
        m_curvesVbo.clear();
        m_curvesVbo.setVertexData(&m_positions[0], m_positions.size(), GL_DYNAMIC_DRAW);

        // construct adjacency indices suitable for OpenGL from curve data
        vector<unsigned int> indices;
        for(size_t i=0; i<curves.size(); ++i) {
            vector<size_t> &conn = curves[i];
            // printf("connection %d\n", conn.size());
            if(conn.empty()) continue;

            // pack beginning and end of `conn` twice into `connIndices`
            // so that the curve goes through them
            // TODO: maybe not do this to be more like ofCurveVertices()?
            vector<size_t> connIndices;
            connIndices.push_back(conn[0]);
            connIndices.insert(connIndices.begin() + 1, conn.begin(), conn.end());
            connIndices.push_back(conn[conn.size() - 1]);
            /*
            printf("connx: ");
            for(size_t i = 0; i<connIndices.size(); ++i) 
                printf("%lu ", connIndices[i]);
            printf("\n");
            */
            
            if(connIndices.size() < 4) continue;

            // transform connIndices to be like GL lines adjacency
            for(size_t i=0; i+3<connIndices.size(); ++i) {
                // if(i % 2 == 0) {
                indices.push_back(connIndices[i]);
                indices.push_back(connIndices[i+1]);
                indices.push_back(connIndices[i+2]);
                indices.push_back(connIndices[i+3]);
                // }
                // else {
                /*
                                    indices.push_back(connIndices[i+3]);
                indices.push_back(connIndices[i+2]);
                indices.push_back(connIndices[i+1]);
                indices.push_back(connIndices[i+0]);
                */
                //  }
                
            }
            /*
            printf("conn: ");
            for(size_t i = 0; i<indices.size(); ++i)
                printf("%lu ", indices[i]);
            printf("\n");
            */
        }
        
        m_curvesVbo.setIndexData(&indices[0], indices.size(), GL_STATIC_DRAW);
        m_indexCount = indices.size();
        
        m_curvesVbo.setAttributeData(m_curvesShader.getAttributeLocation("color"),
                                     &m_colors[0].x, 4, m_colors.size(), GL_DYNAMIC_DRAW);
    }

    {
        m_pointsVbo.clear();
        m_pointsVbo.setVertexData(&m_positions[0], m_positions.size(), GL_DYNAMIC_DRAW);
        m_pointsVbo.setAttributeData(m_pointShader.getAttributeLocation("color"),
                                     &m_colors[0].x, 4, m_colors.size(), GL_DYNAMIC_DRAW);
    }
}

void ofxGpuThicklines::exit() {
    m_curvesShader.unload();
    m_pointShader.unload();
}


void ofxGpuThicklines::beginUpdates() {
    ; // we do nothing. this method is, for now, only to provide a more logical API.
}


void ofxGpuThicklines::endUpdates() {
    m_curvesVbo.updateVertexData(&m_positions[0], m_positions.size());
    m_curvesVbo.updateAttributeData(m_curvesShader.getAttributeLocation("color"),
                                    &m_colors[0].x, m_colors.size());

    m_pointsVbo.updateVertexData(&m_positions[0], m_positions.size());
    m_pointsVbo.updateAttributeData(m_pointShader.getAttributeLocation("color"),
                                    &m_colors[0].x, m_colors.size());
}


void ofxGpuThicklines::draw() {
    ofFill();
    m_curvesShader.begin();
    m_curvesVbo.drawElements(GL_LINES_ADJACENCY, m_indexCount);
    m_curvesShader.end();
}


void ofxGpuThicklines::drawVertices(float radius) {
    ofFill();
    m_pointShader.begin();
    
    if(! m_customPointShader) {
        m_pointShader.setUniform1f("radius", radius);
        m_pointShader.setUniform1f("aspectRatio", ofGetViewportWidth() / (float)ofGetViewportHeight());
    }

    m_pointsVbo.draw(GL_POINTS, 0, m_positions.size());

    m_pointShader.end();
}

