#include "whittedshader.h"

#include "../core/utils.h"

#include "../materials/transmissive.h"

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

    int neg_sqrt = 0;

    if (Utils::getClosestIntersection(r, objList, its)) {
        /*
        if (its.shape->getMaterial().hasTransmission()) {
            
            float raiz = 1 - pow(0.7, 2) * (1 - pow(dot(its.normal.normalized(), (r.o - its.itsPoint).normalized()), 2));

            if (raiz >= 0) {

                Vector3D wt = Transmissive::getTransmissiveRefraction(0.7, its.normal, (r.o - its.itsPoint).normalized());
                
                Ray reflected_ray = Ray(its.itsPoint, wt);

                Utils::getClosestIntersection(reflected_ray, objList, its);

                direct_illumination = WhittedShader::computeColor(reflected_ray, objList, lsList);

            }
            else {;

                neg_sqrt = 1;

            }

        }
        */
        if (its.shape->getMaterial().hasSpecular() || neg_sqrt == 1) {

        Vector3D wr = (its.normal.normalized() * 2 * dot(-r.d.normalized(), its.normal.normalized()) - (-r.d)).normalized();

        Ray reflected_ray = Ray(its.itsPoint, wr);

        Utils::getClosestIntersection(reflected_ray, objList, its);

        direct_illumination = WhittedShader::computeColor(reflected_ray, objList, lsList); 

        neg_sqrt = 1;
     
        }

        if (neg_sqrt == 0) {

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

        return direct_illumination;
    }

    else {
        return bgColor;
    }
}