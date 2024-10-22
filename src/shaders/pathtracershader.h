#ifndef PATHTRACERSHADER_H
#define PATHTRACERSHADER_H

#include "shader.h"

#include "../core/hemisphericalsampler.h"

struct DirectLightResult {
    Vector3D directLight;
    Intersection last_intersection;
};

class PathTracerShader : public Shader
{
public:

    PathTracerShader(double ratio_ref_, int MaxDepth_, int N);

    Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    Intersection GetLastIntersection(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    double getRatioRefraction() const;

    

private:
    double ratio_ref;
    int N_vectors;
    int MaxDepth;
};

#endif // PATHTRACERSHADER_H
