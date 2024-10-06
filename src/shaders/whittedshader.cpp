#include "whittedshader.h"

#include "../core/utils.h"

WhittedShader::WhittedShader() :
    color(Vector3D(1, 0, 0))
{ }

WhittedShader::WhittedShader(Vector3D hitColor_, double maxDist_, Vector3D bgColor_) :
    Shader(bgColor_), maxDist(maxDist_), color(hitColor_)
{ }


Vector3D WhittedShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    
    Intersection its;

    if (Utils::getClosestIntersection(r, objList, its)) {

        Vector3D direct_illumination = Vector3D(0.0, 0.0, 0.0);

        for (int s = 0; s < lsList.size(); s++) {

            Vector3D wi = (lsList[s]->sampleLightPosition() - its.itsPoint).normalized();

            double visibility = 1.0;

            Vector3D lightDirection = (its.itsPoint - lsList[s]->sampleLightPosition()).normalized();
            Ray LightToPoint = Ray(lsList[s]->sampleLightPosition(), lightDirection, 0, Epsilon, (its.itsPoint - lsList[s]->sampleLightPosition()).length()-Epsilon);

            if (Utils::hasIntersection(LightToPoint, objList)) {
                visibility = 0.0;
            }

            Vector3D current_illumination = lsList[s]->getIntensity() * its.shape->getMaterial().getReflectance(its.normal, wi, -r.d) * dot(wi, its.normal) * visibility;

            direct_illumination += current_illumination;
        }

        return direct_illumination;

    }
    
}