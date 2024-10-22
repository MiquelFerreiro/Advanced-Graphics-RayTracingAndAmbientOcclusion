#include "neeshader.h"

#include "pathtracershader.h"

#include "../core/utils.h"

#include "../materials/transmissive.h"
#include "../materials/emissive.h"


NEEShader::NEEShader(double ratio_ref_, int MaxDepth_, int N) {
    ratio_ref = ratio_ref_;
    MaxDepth = MaxDepth_;
    N_vectors = N;
 }

double NEEShader::getRatioRefraction() const {
    return ratio_ref;
}

Ray NEEShader::GetLastRay(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {

    Ray final_ray;

    Intersection its;

    Utils::getClosestIntersection(r, objList, its);

    Vector3D x = its.itsPoint;

    Vector3D wo = -r.d.normalized();

    Vector3D n = its.normal.normalized();

    bool isTotalInternalReflection = false;

    //TRANSMISSIVE

    if (its.shape->getMaterial().hasTransmission()) {

        {
            double ratio_ref = NEEShader::getRatioRefraction();

            if (dot(its.normal.normalized(), r.d.normalized()) > 0) {
                ratio_ref = 1 / ratio_ref;
                n *= -1;
            }

            double raiz = 1 - pow(ratio_ref, 2) * (1 - pow(dot(n, (-r.d).normalized()), 2));

            if (raiz >= 0.0) {

                Vector3D wt = -wo * ratio_ref + n * ((ratio_ref)*dot(n, wo) - sqrt(raiz));

                Ray reflected_ray = Ray(its.itsPoint, wt, r.depth + 1);

                final_ray = NEEShader::GetLastRay(reflected_ray, objList, lsList);

                return final_ray;

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

        final_ray = NEEShader::GetLastRay(reflected_ray, objList, lsList);

        return final_ray;
    }

    //PHONG

    final_ray = r;

    return final_ray;

}

Vector3D NEEShader::ReflectedRadiance(Intersection its1, Vector3D wo, int depth, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {

    Vector3D Ldir = DirectRadiance(its1, wo, objList, lsList);

    Vector3D Lind = IndirectRadiance(its1, wo, depth, objList, lsList);

    //Vector3D Lind = Vector3D(0.0);

    return Ldir + Lind;


}

Vector3D NEEShader::DirectRadiance(Intersection its1, Vector3D wo, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {

    Vector3D dir_radiance = Vector3D(0.0);

    Vector3D random_light_pos = lsList[0]->sampleLightPosition();

    Vector3D x = its1.itsPoint;

    Vector3D wi = (random_light_pos - x).normalized();

    Vector3D n = its1.normal;

    Ray x_to_point = Ray(x, wi);

    Intersection its2;

    if (Utils::getClosestIntersection(x_to_point, objList, its2)) {

        Vector3D Le = its2.shape->getMaterial().getEmissiveRadiance();

        Vector3D reflectance = its1.shape->getMaterial().getReflectance(n, wo, wi);

        double prob_wi = 1 / lsList[0]->getArea();

        Vector3D n2 = its2.normal;

        Vector3D G = (dot(n, wi) * dot(-wi, n2)) / pow((random_light_pos - x).length(), 2);

        dir_radiance += (Le * reflectance * G) / (double)prob_wi;
    }

    return dir_radiance;
}


Vector3D NEEShader::IndirectRadiance(Intersection its1, Vector3D wo, int depth,  const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const {

    Vector3D ind_radiance = Vector3D(0.0);

    Vector3D x = its1.itsPoint;

    Vector3D n = its1.normal;

    HemisphericalSampler hem_sampler;

    if (depth < MaxDepth) {

        Vector3D random_light_pos = lsList[0]->sampleLightPosition();

        Vector3D wi = hem_sampler.getSample(n).normalized();

        double pdf = 1 / (2 * 3.1416);

        Ray newR = Ray(x, wi, depth + 1);

        Intersection its2;

        if (Utils::getClosestIntersection(newR, objList, its2)) {

            ind_radiance += ReflectedRadiance(its2, -wi, depth + 1, objList, lsList) * its1.shape->getMaterial().getReflectance(n, wo, wi) * dot(n, wi) / pdf;
        }
        
    }

    return ind_radiance;
}


Vector3D NEEShader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    Vector3D illumination = Vector3D(0.0);

    Ray last_ray = NEEShader::GetLastRay(r, objList, lsList);

    Intersection its;

    Utils::getClosestIntersection(last_ray, objList, its);

    Vector3D Le = its.shape->getMaterial().getEmissiveRadiance();

    Vector3D Lr = Vector3D(0.0);

    for (int idx = 0; idx < N_vectors; idx++) {

         Lr += ReflectedRadiance(its, -last_ray.d, 0, objList, lsList);

    }

    Lr /= (double)(N_vectors);

    illumination += Le + Lr;

    return illumination;
 
}