#include "ofxGpuThicklines.h"
#include <cassert>

void ofxGpuThicklines::setup(vector<ofVec3f> positions,
                             vector<ofVec4f> colors,
                             vector<ofVec2f> texcoords,
                             vector< vector<size_t> > curves,
                             string customFragShader) {
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
                             "in int vertexID[];\n"
                             "\n"
                             "layout(lines_adjacency) in;\n"
                             "layout(triangle_strip, max_vertices = 7) out;\n"
                             "\n"
                             "\n" // end of next segment
                             "out vec2 fTexCoordVarying;\n"
                             "out vec2 flocalTexCoord;\n"
                             "out vec4 fColorVarying;\n"
                             "flat out int edgeID;\n"
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
                             "    vec2 texCoord1 = texCoordVarying[1];\n"
                             "    vec2 texCoord2 = texCoordVarying[2];\n"
                             "\n"
                             // Cantor's pairing function.
                             // might be possible to find a tighter mapping when assuming that the two vertex
                             // ids are never equal.
                             "    edgeID = ((vertexID[1] + vertexID[2]) * (vertexID[1] + vertexID[2] + 1) / 2 + vertexID[2]);\n" 
                             "\n"
                             "    float thicknessA = THICKNESS;"// * (500.0 / gl_in[1].gl_Position.w);\n" // for uniformly thick lines, set these to be just THICKNESS. here, we estimate the scaling of the width by perspective
                             "    float thicknessB = THICKNESS;"// * (500.0 / gl_in[2].gl_Position.w);\n"
                             "\n"
                             "    \n" // determine the direction of each of the 3 segments (previous, current, next)
                             "    vec2 v0 = normalize(p1-p0);\n"
                             "    vec2 v1 = normalize(p2-p1);\n"
                             "    vec2 v2 = normalize(p3-p2);\n"
                             "\n"
                             "    \n" // determine the normal of each of the 3 segments (previous, current, next)
                                      // the mixing and factors are to catch case where p0==p1 or p2==p1 and thus the corresponding normals are not defined.
                             "    vec2 n1 = vec2(-v1.y, v1.x);\n"
                             "    float p_n0 = dot(v0,v0) < 0.1 ? 0.0 : 1.0;\n"
                             "    vec2 n0 = mix(n1, vec2(-v0.y, v0.x), p_n0);\n"
                             "    float p_n2 = dot(v2,v2) < 0.1 ? 0.0 : 1.0;\n"
                             "    vec2 n2 = mix(n1, vec2(-v2.y, v2.x), 1.0 - p_n2);\n"
                             // FIXME: this somehow doesn't work. we still get flickering unless we set all normals to be the same
                             "    n0 = n1;"
                             "    n2 = n1;"
                             "\n"
                             "    \n" // determine miter lines by averaging the normals of the 2 segments
                             "    vec2 miter_a = normalize(n0 + n1);\n" // miter at start of current segment
                             "    vec2 miter_b = normalize(n1 + n2);\n" // miter at end of current segment
                             "\n"
                             "    \n" // determine the length of the miter by projecting it onto normal and then inverse it
                             "    float length_a = thicknessA / dot(miter_a, n1);\n"
                             "    float length_b = thicknessB / dot(miter_b, n1);\n"
                             "\n"
                             "    // prevent excessively long miters at sharp corners\n"
                             "    if( dot(v0,v1) < -MITER_LIMIT ) {\n"
                             "        miter_a = n1;\n"
                             "	      length_a = thicknessA;\n"
                             "\n"
                             "        float f = sign(dot(v0,n1));\n"
                             "        float g = 1.0 - abs(f);\n"
                             "\n"
                             "        fTexCoordVarying = texCoord1;\n"
                             "        flocalTexCoord = vec2(0, g);\n"
                             "        fColorVarying = colorVarying[1];\n"
                             "        gl_Position = vec4( (p1 + thicknessA * mix(n0,n1,g) * f) / WIN_SCALE, 0.0, 1.0 );\n"
                             "        EmitVertex();\n"
                             "\n"
                             "        fTexCoordVarying = texCoord1;\n"
                             "        flocalTexCoord = vec2(0, g); \n"
                             "        fColorVarying = colorVarying[1];\n"
                             "        gl_Position = vec4( (p1 + thicknessA * mix(n1,n0,g) * f) / WIN_SCALE, 0.0, 1.0 );\n"
                             "        EmitVertex();\n"
                             "\n"
                             "        fTexCoordVarying = texCoord1;\n"
                             "        flocalTexCoord = vec2(0, 0.5);\n"
                             "        fColorVarying = colorVarying[1];\n"
                             "        gl_Position = vec4( p1 / WIN_SCALE, 0.0, 1.0 );\n"
                             "        EmitVertex();\n"
                             "        EndPrimitive();\n"
                             "    }\n"
                             "\n"
                             "    if( dot(v1,v2) < -MITER_LIMIT ) {\n"
                             "	      miter_b = n1;\n"
                             "	      length_b = thicknessB;\n"
                             "    }\n"
                             "  \n"
                             "    \n" // generate the triangle strip
                             "    fTexCoordVarying = texCoord1;\n"
                             "    flocalTexCoord = vec2(0,0);\n"
                             "    fColorVarying = colorVarying[1];\n"
                             "    gl_Position = vec4( (p1 + length_a * miter_a) / WIN_SCALE, 0.0, 1.0 );\n"
                             "    EmitVertex();\n"
                             "  \n"
                             "    fTexCoordVarying = texCoord1;\n"
                             "    flocalTexCoord = vec2(0,1);\n"
                             "    fColorVarying = colorVarying[1];\n"
                             "    gl_Position = vec4( (p1 - length_a * miter_a) / WIN_SCALE, 0.0, 1.0 );\n"
                             "    EmitVertex();\n"
                             "  \n"
                             "    fTexCoordVarying = texCoord2;\n"
                             "    flocalTexCoord = vec2(1,0);\n"
                             "    fColorVarying = colorVarying[2];\n"
                             "    gl_Position = vec4( (p2 + length_b * miter_b) / WIN_SCALE, 0.0, 1.0 );\n"
                             "    EmitVertex();\n"
                             "  \n"
                             "    fTexCoordVarying = texCoord2;\n"
                             "    flocalTexCoord = vec2(1,1);\n"
                             "    fColorVarying = colorVarying[2];\n"
                             "    gl_Position = vec4( (p2 - length_b * miter_b) / WIN_SCALE, 0.0, 1.0 );\n"
                             "    EmitVertex();\n"
                             "\n"
                             "    EndPrimitive();\n"
                             "}\n");
        
        string vertShader = ("#version 150\n"
                             "\n"
                             "uniform mat4 textureMatrix;\n"
                             "uniform mat4 modelViewProjectionMatrix;\n"
                             "\n"
                             "in vec4 position;\n"
                             "in vec4 color;\n"
                             "in vec2 texcoord;\n"
                             "\n"
                             "out vec4 colorVarying;\n"
                             "out vec2 texCoordVarying;\n"
                             "out int vertexID;\n"
                             "\n"
                             "void main()\n"
                             "{\n"
                             "    gl_Position = modelViewProjectionMatrix * position;\n"
                             "    colorVarying = color;\n"
                             "    vec2 drawTexCoord = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;\n"
                             "    texCoordVarying = drawTexCoord;\n"
                             "    vertexID = gl_VertexID;\n"
                             "}\n"
            );
        
        string fragShader = ("#version 150\n"
                             "\n"
                             "in vec4 fColorVarying;\n"
                             "in vec2 fTexCoordVarying;\n"
                             "in vec2 flocalTexCoord;\n"
                             "uniform vec4 globalColor;\n"
                             "\n"
                             "out vec4 outputColor;\n"
                             "\n"
                             "void main()\n"
                             "{\n"
                             "    outputColor = globalColor * fColorVarying;\n"
                             "}\n"
            );
        if(customFragShader.length() != 0)
            fragShader = customFragShader;

        if(m_curvesShader.isLoaded())
            m_curvesShader.unload();
        m_curvesShader.setupShaderFromSource(GL_GEOMETRY_SHADER, geomShader);
        m_curvesShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShader);
        m_curvesShader.setupShaderFromSource(GL_VERTEX_SHADER, vertShader);
        m_curvesShader.bindDefaults();
        m_curvesShader.linkProgram();
    }
    
    reset(positions, colors, texcoords, curves);
}

void ofxGpuThicklines::setup(vector<ofVec3f> positions,
                             vector<ofVec4f> colors,
                             vector< vector<size_t> > curves,
                             string customFragShader) {
    vector<ofVec2f> texcoords; // intentionally empty
    setup(positions, colors, texcoords, curves, customFragShader);
}

void ofxGpuThicklines::setup(ofMesh &mesh, string customFragShader, bool onlylines) {
    mesh.mergeDuplicateVertices();
    vector<ofVec4f> colors; colors.reserve(mesh.getNumVertices());
    if(mesh.getNumColors() == mesh.getNumVertices()) {
        for(const ofFloatColor &c : mesh.getColors()) {
            colors.push_back(ofVec4f(c.r,c.g,c.b,c.a));
        }
    }
    else {
        for(size_t i=0; i<mesh.getNumVertices(); ++i)
            colors.push_back(ofVec4f(1,1,1,0.3));
    }
    // ofLogNotice("ofxGpuThicklines", "setup mesh with %lu vertices, %lu colors (resolved to %lu)",
    // mesh.getNumVertices(), mesh.getNumColors(), colors.size());

    vector< vector<size_t> > curves;
    {
        // each edge needs to be part of exactly one curve
        // we'd like to add multiple edges in each curve.

        // build adjacency list. For edge (i,j), adjacency[i] contains j and adjacency[j] contains i
        map< size_t, set<size_t> > adjacency;
        set< size_t > liveVertices;
        {
            if(! onlylines) { // triangles
                for(int i0=0; i0<(int)mesh.getIndices().size() - 2; i0+=3) {
                    size_t i = mesh.getIndex(i0);
                    size_t j = mesh.getIndex(i0 + 1);
                    size_t k = mesh.getIndex(i0 + 2);
            
                    adjacency[i].insert(j);
                    adjacency[j].insert(i);
                    adjacency[i].insert(k);
                    adjacency[k].insert(i);
                    adjacency[j].insert(k);
                    adjacency[k].insert(j);

                    liveVertices.insert(i);
                    liveVertices.insert(j);
                    liveVertices.insert(k);
                }
            }
            else {
                ofLogNotice("only");
                for(int i0=0; i0<(int)mesh.getIndices().size() - 1; i0+=2) {
                    size_t i = mesh.getIndex(i0);
                    size_t j = mesh.getIndex(i0 + 1);
            
                    adjacency[i].insert(j);
                    adjacency[j].insert(i);

                    liveVertices.insert(i);
                    liveVertices.insert(j);

                }
            }
        }
        // ofLogNotice("ofxGpuThicklines", "adjacency has %lu elements, live vertices are %lu", adjacency.size(), liveVertices.size());

        vector<size_t> currentCurve;
        map<size_t, set<size_t>> addedEdges;
        while(! liveVertices.empty()) {
            size_t curVertex = *liveVertices.begin(); // take any vertex
                
            bool firstVertex = true;
            
            // ofLogNotice("ofxGpuThicklines", "curve add iter %lu", curVertex);
            while(true) {
                bool foundNone = true;
                for(const size_t &neighbor : adjacency[curVertex]) { // find any neighbor whose edge is not yet added
                    size_t a = curVertex;
                    size_t b = neighbor;
                    if(a > b) std::swap(a, b);
                            
                    if(addedEdges[a].find(b) == addedEdges[a].end()) { // if the edge to neighbor was not yet added...
                        // add that edge to the current curve and the added edges
                        
                        // current edge not yet added
                        if(firstVertex) { // add first vertex as well => avoid 1-vertex curves
                            firstVertex = false;
                            currentCurve.push_back(curVertex);
                        }
                        foundNone = false;
                        currentCurve.push_back(neighbor);
                        addedEdges[a].insert(b);

                        // keep going with the edge we just added
                        curVertex = neighbor;
                        break;
                    }
                }
                // if no neighbor vertex with a non-added edge was found,
                // remove the current vertex from the live vertices so that another one is picked next time (and a new curve started)
                if(foundNone) {
                    if(currentCurve.size() > 0) {
                        curves.push_back(currentCurve);
                        currentCurve = vector<size_t>();
                        firstVertex = true;
                    }
                    liveVertices.erase(liveVertices.find(curVertex));
                    break;
                }
            }
        }
        // if there is an unfinished curve, add it as well
        if(currentCurve.size() > 0) {
            curves.push_back(currentCurve);
        }
    } // end add curves
    
    // ofLogNotice("ofxGpuThicklines", "setup mesh with %lu curves", curves.size());
    // int x = 0;
    // for(vector<size_t> &c : curves) {
    // ofLogNotice("ofxGpuThicklines", "\tcurve %d: %s", x, ofToString(c).c_str());
    // x++;
    // }        
        
    setup(mesh.getVertices(), colors, mesh.getTexCoords(), curves, customFragShader);
}

void ofxGpuThicklines::reset(vector<ofVec3f> positions,
                             vector<ofVec4f> colors,
                             vector<ofVec2f> texcoords,
                             vector< vector<size_t> > curves) {
    m_shaderBegun = false;
    
    assert(positions.size() == colors.size());

    m_positions = positions;
    m_colors = colors;
    m_texcoords = texcoords;

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
                indices.push_back(connIndices[i]);
                indices.push_back(connIndices[i+1]);
                indices.push_back(connIndices[i+2]);
                indices.push_back(connIndices[i+3]);
            }
        }
        
        m_curvesVbo.setIndexData(&indices[0], indices.size(), GL_STATIC_DRAW);
        m_indexCount = indices.size();
        
        m_curvesVbo.setAttributeData(m_curvesShader.getAttributeLocation("color"),
                                     &m_colors[0].x, 4, m_colors.size(), GL_DYNAMIC_DRAW);
        
        m_curvesVbo.setTexCoordData(&m_texcoords[0], m_texcoords.size(), GL_DYNAMIC_DRAW);

        // using `setTexCoordData` here doesn't work, probably because our attributes are in a different order from the default shader (from which that function takes the attribute location to set)
        m_curvesVbo.setAttributeData(m_curvesShader.getAttributeLocation("texcoord"),
                                     &m_texcoords[0].x, 2, m_texcoords.size(), GL_DYNAMIC_DRAW);
    }
}

void ofxGpuThicklines::exit() {
    ofLogNotice("exit");
    m_curvesVbo.clear();
    m_curvesShader.unload();
}


void ofxGpuThicklines::beginUpdates() {
    ; // we do nothing. this method is, for now, only to provide a more logical API.
}


void ofxGpuThicklines::endUpdates() {
    m_curvesVbo.updateVertexData(&m_positions[0], m_positions.size());
    m_curvesVbo.updateAttributeData(m_curvesShader.getAttributeLocation("color"),
                                    &m_colors[0].x, m_colors.size());
}

ofShader &ofxGpuThicklines::prepareDraw() {
    m_curvesShader.begin();
    m_shaderBegun = true;
    return m_curvesShader;
}

void ofxGpuThicklines::draw() {
    ofFill();
    if(! m_shaderBegun)
        m_curvesShader.begin();
    m_curvesVbo.drawElements(GL_LINES_ADJACENCY, m_indexCount);
    m_curvesShader.end();

    m_shaderBegun = false;
}

