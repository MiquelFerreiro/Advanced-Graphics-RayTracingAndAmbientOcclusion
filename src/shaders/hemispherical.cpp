#include "hemispherical.h"

#include "../core/utils.h"

#include "../materials/transmissive.h"
#include "../materials/emissive.h"

Hemispherical::Hemispherical() :
    color(Vector3D(1, 0, 0))
{ }

Hemispherical::Hemispherical(Vector3D hitColor_, double maxDist_, Vector3D bgColor_, Vector3D ambient_light_) :
    Shader(bgColor_), maxDist(maxDist_), color(hitColor_), ambient_light(ambient_light_)
{ }

double Hemispherical::getRatioRefraction() const {
    return ratio_ref;
}

Vector3D Hemispherical::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    Intersection its;

    Intersection its_y;

    Utils::getClosestIntersection(r, objList, its);

    Vector3D illumination = Vector3D(0.0);

    Vector3D x = its.itsPoint;

    Vector3D wo = -r.d;

    Vector3D n = its.normal;

    int N = 64;

    for (int idx = 0; idx < N; idx++) {

        Vector3D light_pos = lsList[0]->sampleLightPosition();

        Vector3D wi = (light_pos - x).normalized();

        Ray x_to_light = Ray(x, wi);

        Utils::getClosestIntersection(x_to_light, objList, its_y);

        Material* mat;

        float Xs[] = { its_y.itsPoint.x, its_y.itsPoint.y, its_y.itsPoint.z };
        float Ns[] = { its_y.normal.x, its_y.normal.y, its_y.normal.z };
        
        Vector3D radiance;

        Vector3D reflectance;

        if ((Xs[0] == 0.0 && Xs[1] == 0.0 && Xs[2] == 0.0) &&
            (Ns[0] == 0.0 && Ns[1] == 0.0 && Ns[2] == 0.0)) {

            mat = new Emissive(Vector3D(25, 25, 25), Vector3D(0.5));

            Vector3D radiance = mat->getEmissiveRadiance();

            Vector3D reflectance = mat->getReflectance(n, wo, wi);
        }
        else {
            Vector3D radiance = its_y.shape->getMaterial().getEmissiveRadiance();

            Vector3D reflectance = its_y.shape->getMaterial().getReflectance(n, wo, wi);
        }
        
        float prob_wi = 1 / (3.14 * 2);

        illumination += (radiance * reflectance * dot(wi, n) / prob_wi );

    }

    illumination = illumination / N;

    return illumination;
}