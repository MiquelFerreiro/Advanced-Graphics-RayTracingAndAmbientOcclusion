#include "whittedshader.h"

#include "../core/utils.h"

WhittedShader::WhittedShader() :
    color(Vector3D(1, 0, 0))
{ }

WhittedShader::WhittedShader(Vector3D hitColor_, double maxDist_, Vector3D bgColor_, Vector3D ambient_light_) :
    Shader(bgColor_), maxDist(maxDist_), color(hitColor_), ambient_light(ambient_light_)
{ }


Vector3D WhittedShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    
    Intersection its;
    Vector3D direct_illumination = Vector3D(0.0, 0.0, 0.0);

    if (Utils::getClosestIntersection(r, objList, its)) {

        if (!its.shape->getMaterial().hasSpecular()) {

            for (int s = 0; s < lsList.size(); s++) {

                Vector3D wi = (lsList[s]->sampleLightPosition() - its.itsPoint).normalized();

                double visibility = 1.0;

                Vector3D lightDirection = (its.itsPoint - lsList[s]->sampleLightPosition()).normalized();
                Ray LightToPoint = Ray(lsList[s]->sampleLightPosition(), lightDirection, 0, Epsilon, (its.itsPoint - lsList[s]->sampleLightPosition()).length() - Epsilon);

                if (Utils::hasIntersection(LightToPoint, objList)) {
                    visibility = 0.0;
                }

                Vector3D current_illumination = lsList[s]->getIntensity() * its.shape->getMaterial().getReflectance(its.normal, wi, -r.d) * dot(wi, its.normal) * visibility;

                direct_illumination += current_illumination;
            }

            direct_illumination = direct_illumination + ambient_light * its.shape->getMaterial().getDiffuseReflectance();


        }
        else {

            Vector3D wr = (its.normal.normalized() * 2 * dot(-r.d.normalized(), its.normal.normalized()) - (-r.d)).normalized();

            Ray reflected_ray = Ray(its.itsPoint, wr);

            Utils::getClosestIntersection(reflected_ray, objList, its);
                
            direct_illumination = WhittedShader::computeColor(reflected_ray, objList, lsList);
        }

        return direct_illumination;
    }

    else {
        return bgColor;
    }
}