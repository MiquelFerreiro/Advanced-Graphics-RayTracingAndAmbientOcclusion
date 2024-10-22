#ifndef NEESHADER_H
#define NEESHADER_H

#include "shader.h"

#include "../core/hemisphericalsampler.h"

class NEEShader : public Shader
{
public:

    NEEShader(double ratio_ref_, int MaxDepth_, int N);

    Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    Ray GetLastRay(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    Vector3D NEEShader::ReflectedRadiance(Intersection its1, Vector3D wo, int depth, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const;

    Vector3D NEEShader::DirectRadiance(Intersection its1, Vector3D wo, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const;

    Vector3D NEEShader::IndirectRadiance(Intersection its1, Vector3D wo, int depth, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const;

    double getRatioRefraction() const;

    

private:
    double ratio_ref;
    int N_vectors;
    int MaxDepth;
};

#endif // NEESHADER_H
