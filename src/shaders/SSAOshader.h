#ifndef SSAOSHADER_H
#define SSAOSHADER_H

#include "shader.h"

class SSAOshader : public Shader
{
public:

    SSAOshader(int N_, float det_range, Vector3D bgColor_);

    virtual Vector3D computeColor(const Ray &r, const std::vector<Shape*> &objList, const std::vector<LightSource*> &lsList) const;
   
private:

    int N;
    float detection_range;

};

#endif // SSAOSHADER_H
