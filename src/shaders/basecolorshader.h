#ifndef BASECOLORSHADER_H
#define BASECOLORSHADER_H

#include "shader.h"

class BaseColorShader : public Shader
{
public:

    BaseColorShader(Vector3D bgColor_);

    virtual Vector3D computeColor(const Ray &r,
                             const std::vector<Shape*> &objList,
                             const std::vector<LightSource*> &lsList) const;
   
};

#endif // BASECOLORSHADER_H
