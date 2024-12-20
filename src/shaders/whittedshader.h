#ifndef WHITTEDSHADER_H
#define WHITTEDSHADER_H

#include "shader.h"


class WhittedShader : public Shader
{
public:
    WhittedShader();
    WhittedShader(Vector3D color_, double maxDist_, Vector3D bgColor_, Vector3D ambient_light_);

    Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<LightSource*>& lsList) const;

    double getRatioRefraction() const;


private:
    double maxDist;
    Vector3D color;
    Vector3D ambient_light;
    double ratio_ref = 0.7;
};

#endif // WHITTEDSHADER_H
