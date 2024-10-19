#ifndef AREADIRECTSHADER_H
#define AREADIRECTSHADER_H

#include "shader.h"

#include "../core/hemisphericalsampler.h"

class AreaDirectShader : public Shader
{
public:
    AreaDirectShader();
    AreaDirectShader(Vector3D color_, double maxDist_, Vector3D bgColor_, Vector3D ambient_light_, int N);

    Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    double getRatioRefraction() const;


private:
    double maxDist;
    Vector3D color;
    Vector3D ambient_light;
    double ratio_ref = 0.7;
    int N_vectors;
};

#endif // AREADIRECTSHADER_H
