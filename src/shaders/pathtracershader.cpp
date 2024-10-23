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

Intersection PathTracerShader::GetLastIntersection(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {

    if (r.depth > 5) {
        return Intersection();
    }

    Intersection final_its;

    Intersection its;

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

                Ray reflected_ray = Ray(its.itsPoint, wt, r.depth + 1);

                final_its = PathTracerShader::GetLastIntersection(reflected_ray, objList, lsList);

                return final_its;

            }
            else {

                isTotalInternalReflection = true;

            }
        }
    }

    //SPECULAR

    if (its.shape->getMaterial().hasSpecular() || isTotalInternalReflection) {

        Vector3D wr = (n * 2 * dot(wo, n) - wo).normalized();

        Ray reflected_ray = Ray(its.itsPoint, wr, r.depth + 1);

        final_its = PathTracerShader::GetLastIntersection(reflected_ray, objList, lsList);

        return final_its;
    }

    //PHONG

    final_its = its;

    return final_its;

}

Vector3D PathTracerShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{

    Vector3D illumination = Vector3D(0.0);

    Intersection last_its = PathTracerShader::GetLastIntersection(r, objList, lsList);

    if (last_its.itsPoint != Vector3D(0.0) && last_its.normal != Vector3D(0.0)) {

        illumination += last_its.shape->getMaterial().getEmissiveRadiance();

        Vector3D sum_radiance = Vector3D(0.0);

        Intersection its;

        HemisphericalSampler hem_sampler;

        if (r.depth < MaxDepth) {

            for (int idx = 0; idx < N_vectors; idx++) {

                Vector3D x = last_its.itsPoint;

                Vector3D n = last_its.normal;

                Vector3D wo = -r.d;

                Vector3D wi = hem_sampler.getSample(n).normalized();

                double pdf = 1 / (2 * 3.1416);

                Ray newR = Ray(x, wi, r.depth + 1);

                if (Utils::getClosestIntersection(newR, objList, its)) {

                    sum_radiance += PathTracerShader::computeColor(newR, objList, lsList) * last_its.shape->getMaterial().getReflectance(n, wo, wi) * dot(n, wi) / pdf;

                }
            }

            sum_radiance /= (double)(N_vectors);  //Average de vectores random

            illumination += sum_radiance;
        }

    }

    return illumination;
}