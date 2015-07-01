#ifndef OFXGPUCURVES_H
#define OFXGPUCURVES_H

#include "ofMain.h"

class ofxGPUCurves
{
public:
    ofxGPUCurves() {  }
    virtual ~ofxGPUCurves() { ; }

    /// `positions` and `opacities` should be vectors of equal length containing the data
    /// for each point.
    /// each element of `curves` describes a single curve in terms of the corresponding point
    /// indices
    /// i.e we assume
    ///     positions.size == opacities.size and
    ///     for all c in curves: [  for all i in c: i <= positions.size  ]
    void setup(vector<ofVec3f> positions, vector<float> opacities,
               vector< vector<size_t> > curves);
    void setup(vector<ofVec3f> positions, vector<float> opacities,
               vector< vector<size_t> > curves,
               ofShader pointShader);
    void reset(vector<ofVec3f> positions, vector<float> opacities, vector< vector<size_t> > curves);

    const vector<ofVec3f> &positions() const { return m_positions; }
    const vector<float> &opacities() const { return m_opacities; }

    size_t numPositions() { return m_positions.size(); }

    void beginUpdates();
    void endUpdates();

    // always wrap all update calls with  `beginUpdates()` and `endUpdates()`
    void updatePosition(size_t i, ofVec3f v) { m_positions[i] = v; }
    void updateOpacity(size_t i, float o) { m_opacities[i] = o; }
    void updateVertex(size_t i, ofVec3f v, float o) {
        updatePosition(i, v);
        updateOpacity(i, o);
    }
    
    size_t numIndices() { return m_indexCount; }

    void draw();
    void drawVertices(float radius = 6); // `radius` is only used if no point shader was supplied on setup

protected:
    ofShader m_curvesShader;
    ofVbo m_curvesVbo;

    ofShader m_pointShader;
    ofVbo m_pointsVbo;

    vector<ofVec3f> m_positions;
    vector<float> m_opacities;

    vector< vector<size_t> > m_structure;
    size_t m_indexCount;

    bool m_customPointShader;
};

#endif
