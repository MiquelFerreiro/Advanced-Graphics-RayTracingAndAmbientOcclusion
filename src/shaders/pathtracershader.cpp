#include "pathtracershader.h"

#include "../core/utils.h"

#include "../materials/transmissive.h"
#include "../materials/emissive.h"


PathTracerShader::PathTracerShader(double ratio_ref_, int MaxDepth_, int N) {
    ratio_ref = ratio_ref_;
    MaxDepth = MaxDepth_;
    N_vectors = N;
 }

double PathTracerShader::getRatioRefraction() const {
    return ratio_ref;
}

Vector3D PathTracerShader::DirectLight(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {

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
            double ratio_ref = PathTracerShader::getRatioRefraction();

            if (dot(its.normal.normalized(), r.d.normalized()) > 0) {
                ratio_ref = 1 / ratio_ref;
                n *= -1;
            }

            double raiz = 1 - pow(ratio_ref, 2) * (1 - pow(dot(n, (-r.d).normalized()), 2));

            if (raiz >= 0.0) {

                Vector3D wt = -wo * ratio_ref + n * ((ratio_ref)*dot(n, wo) - sqrt(raiz));

                Ray reflected_ray = Ray(its.itsPoint, wt);

                return PathTracerShader::DirectLight(reflected_ray, objList, lsList);

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

        return PathTracerShader::DirectLight(reflected_ray, objList, lsList);
    }

    //PHONG

    for (int idx = 0; idx < N_vectors; idx++) {

        Vector3D random_light_pos = lsList[0]->sampleLightPosition();

        Vector3D random_vector = (random_light_pos - x).normalized();

        Ray x_to_point = Ray(x, random_vector.normalized(), (random_light_pos - x).length() - Epsilon);

        Vector3D radiance(0.0);

        Vector3D reflectance(0.0);

        if (Utils::getClosestIntersection(x_to_point, objList, its_y)) {

            radiance = its_y.shape->getMaterial().getEmissiveRadiance();

            reflectance = its.shape->getMaterial().getReflectance(n, wo, random_vector);

            double prob_wi = 1 / lsList[0]->getArea();

            Vector3D ny = its_y.normal;

            Vector3D G = (dot(n, random_vector) * dot(-random_vector, ny)) / pow((random_light_pos - x).length(), 2);

            direct_illumination += (radiance * reflectance * G) / (double)prob_wi;

        }

    }

    direct_illumination = direct_illumination / (double)N_vectors;

    return direct_illumination;

}

Vector3D PathTracerShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{

    Vector3D illumination = PathTracerShader::DirectLight(r, objList, lsList);

    Intersection its;

    Intersection its2;

    HemisphericalSampler hem_sampler;

    if (Utils::getClosestIntersection(r, objList, its)) {

        Vector3D x = its.itsPoint;

        Vector3D n = its.normal;

        Vector3D random_vector = hem_sampler.getSample(n).normalized();

        if (r.depth < MaxDepth) {

            Ray newR = Ray(x, random_vector, r.depth + 1);

            if (Utils::getClosestIntersection(newR, objList, its2)) {

                illumination += PathTracerShader::computeColor(newR, objList, lsList) * its.shape->getMaterial().getReflectance(n, -r.d, random_vector) * dot(n, random_vector) * (2 * 3.1416);

            }
        
        }
    }

    Vector3D Le = Vector3D(0.0);

    if (r.depth == 0) {
        illumination += its.shape->getMaterial().getEmissiveRadiance();
    }

    return illumination;
 
}