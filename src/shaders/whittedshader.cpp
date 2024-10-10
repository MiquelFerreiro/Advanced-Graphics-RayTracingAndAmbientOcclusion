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
    Vector3D direct_illumination = Vector3D(0.0);

    if (Utils::getClosestIntersection(r, objList, its)) {
        
        //TRANSMISSIVE MATERIAL

        bool isTotalInternalReflection = false;

        if (its.shape->getMaterial().hasTransmission()) {

            double ratio_ref = 0.7;

            Vector3D n = its.normal.normalized();
            Vector3D wo = (-r.d).normalized();

            if (dot(its.normal.normalized(), r.d.normalized()) > 0) {
                ratio_ref = 1 / ratio_ref;
                n *= -1;
            }

            double raiz = 1 - pow(0.7, 2) * (1 - pow(dot(n, (-r.d).normalized()), 2));

            if (raiz >= 0.0) {
                
                Vector3D wt = - wo * ratio_ref + dot(n, (ratio_ref)*dot(n, wo) - sqrt(raiz));
                
                Ray reflected_ray = Ray(its.itsPoint, wt, Epsilon, 0, INFINITY);

                direct_illumination = WhittedShader::computeColor(reflected_ray, objList, lsList);

                
                return direct_illumination;

            }
            else {
                
                return Vector3D(1.0);
                isTotalInternalReflection = true;

            }

        }
        
        //MIRROR MATERIAL

        if (its.shape->getMaterial().hasSpecular() || isTotalInternalReflection) {

        Vector3D wr = (its.normal.normalized() * 2 * dot(-r.d.normalized(), its.normal.normalized()) - (-r.d)).normalized();

        Ray reflected_ray = Ray(its.itsPoint, wr, Epsilon, 0, INFINITY);

        direct_illumination = WhittedShader::computeColor(reflected_ray, objList, lsList); 

        return direct_illumination;
     
        }


        //PHONG MATERIAL

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

        return direct_illumination;

    }

    return bgColor;
}