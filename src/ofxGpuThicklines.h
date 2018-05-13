#pragma once

#include "ofMain.h"

class ofxGpuThicklines
{
public:
    ofxGpuThicklines() : m_shaderBegun(false) {  }
    virtual ~ofxGpuThicklines() { ; }

    /// `positions` and `colors` should be vectors of equal length containing the data
    /// for each point.
    /// each element of `curves` describes a single curve in terms of the corresponding point
    /// indices
    /// i.e we assume
    ///     positions.size == colors.size and
    ///     for all c in curves: [  for all i in c: i <= positions.size  ]
    /// `customFragShader` is the GLSL code for the fragment shader to use.
    ///     When this is a nonempty string, this is used instead of the default fragment shader.
    //      Use `prepareDraw()` before calling `draw()` to set uniforms or attributes on the shader.
    ///     The thickwireframe example shows how to use this.
    void setup(vector<glm::vec3> positions, vector<glm::vec4> colors,
               vector< vector<size_t> > curves, string customFragShader = "");
    void setup(vector<glm::vec3> positions, vector<glm::vec4> colors, vector<glm::vec2> texcoords,
               vector< vector<size_t> > curves, string customFragShader = "");

    // builds a thick wireframe from a mesh
    // `onlylines`: interpret the mesh as consisting only of lines instead of triangles,
    // ie two adjacent indices form a line.
    void setup(const ofMesh &mesh, string customFragShader = "", bool onlylines=false);
    
    void reset(vector<glm::vec3> positions, vector<glm::vec4> colors, vector<glm::vec2> texcoords, vector< vector<size_t> > curves);

    const vector<glm::vec3> &positions() const { return m_positions; }
    const vector<glm::vec4> &colors() const { return m_colors; }

    size_t numPositions() { return m_positions.size(); }

    void beginUpdates();
    void endUpdates();

    // always wrap all update calls with  `beginUpdates()` and `endUpdates()`
    void updatePosition(size_t i, glm::vec3 v) { m_positions[i] = v; }
    void updateColor(size_t i, glm::vec4 o) { m_colors[i] = o; }
    void updateVertex(size_t i, glm::vec3 v, glm::vec4 o) {
        updatePosition(i, v);
        updateColor(i, o);
    }
    // TODO: ability to update curves
    
    size_t numIndices() { return m_indexCount; }

    ofShader &prepareDraw(); // call this once before `draw()` if using a custom fragment shader. Do not call it multiple times.
    void draw(float lineWidth = 3, bool perspective = true, glm::vec2 viewportSize = glm::vec2(0,0)); // if viewportSize == 0, (ofGetWidth(), ofGetHeight()) is used.

protected:
    ofShader m_curvesShader;
    ofVbo m_curvesVbo;

    vector<glm::vec3> m_positions;
    vector<glm::vec4> m_colors;
    vector<glm::vec2> m_texcoords;

    vector< vector<size_t> > m_structure;
    size_t m_indexCount;

    bool m_shaderBegun; // was prepareDraw() already called?
};
