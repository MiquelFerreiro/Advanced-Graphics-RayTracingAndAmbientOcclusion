#include "hemispherical.h"

#include "../core/utils.h"

#include "../materials/transmissive.h"
#include "../materials/emissive.h"

#include "../core/hemisphericalsampler.h"

Hemispherical::Hemispherical() :
    color(Vector3D(1, 0, 0))
{ }

Hemispherical::Hemispherical(Vector3D hitColor_, double maxDist_, Vector3D bgColor_, Vector3D ambient_light_, int N) :
    Shader(bgColor_), maxDist(maxDist_), color(hitColor_), ambient_light(ambient_light_), N_vectors(N)
{ }

double Hemispherical::getRatioRefraction() const {
    return ratio_ref;
}

Vector3D Hemispherical::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    Intersection its;

    Intersection its_y;

    Vector3D direct_illumination = Vector3D((double)0.0);

    Utils::getClosestIntersection(r, objList, its);

    Vector3D x = its.itsPoint;

    Vector3D wo = -r.d.normalized();

    Vector3D n = its.normal.normalized();

    bool isTotalInternalReflection = false;
    
    //TRANSMISSIVE

    if (its.shape->getMaterial().hasTransmission()) {

        {
            double ratio_ref = Hemispherical::getRatioRefraction();

            if (dot(its.normal.normalized(), r.d.normalized()) > 0) {
                ratio_ref = 1 / ratio_ref;
                n *= -1;
            }

            double raiz = 1 - pow(ratio_ref, 2) * (1 - pow(dot(n, (-r.d).normalized()), 2));

            if (raiz >= 0.0) {

                Vector3D wt = -wo * ratio_ref + n * ((ratio_ref)*dot(n, wo) - sqrt(raiz));

                Ray reflected_ray = Ray(its.itsPoint, wt);

                return Hemispherical::computeColor(reflected_ray, objList, lsList);

            }
            else {

                isTotalInternalReflection = true;

            }
        }
    }

    //SPECULAR

    if (its.shape->getMaterial().hasSpecular() || isTotalInternalReflection) {

        Vector3D wr = (n * 2 * dot(wo, n) - wo).normalized();

        Ray reflected_ray = Ray(its.itsPoint, wr);

        return Hemispherical::computeColor(reflected_ray, objList, lsList);
    }
    

    //PHONG

    HemisphericalSampler hem_sampler;

    for (int idx = 0; idx < N_vectors; idx++) {

        Vector3D random_vector = hem_sampler.getSample(n).normalized();

        Ray x_to_point = Ray(x, random_vector);

        Vector3D radiance(0.0);

        Vector3D reflectance(0.0);

        if (Utils::getClosestIntersection(x_to_point, objList, its_y)) {

            if (its_y.shape->getMaterial().isEmissive()) {

                radiance = its_y.shape->getMaterial().getEmissiveRadiance();

                reflectance = its.shape->getMaterial().getReflectance(n, wo, random_vector);

                double prob_wi = 1 / (2 * 3.1416);

                direct_illumination += (radiance * reflectance * dot(random_vector, n)) / (double)prob_wi;
            }
 
        }

    }

    direct_illumination = direct_illumination / (double)N_vectors;

    Vector3D Le = its.shape->getMaterial().getEmissiveRadiance();

    Vector3D indirect_illumination = this->ambient_light * its.shape->getMaterial().getDiffuseReflectance();

    Vector3D total_light = direct_illumination + Le + indirect_illumination;

    return total_light;
}