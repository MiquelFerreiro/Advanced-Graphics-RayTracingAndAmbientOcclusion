#include "basecolorshader.h"
#include "../core/utils.h"


BaseColorShader::BaseColorShader(Vector3D bgColor_) :
    Shader(bgColor_)
{ }

Vector3D BaseColorShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{

    Intersection its;

    if (Utils::getClosestIntersection(r, objList, its)) {


        return its.shape->getMaterial().getColorMaterial();


    }
    else {
        return bgColor;
    }
}

