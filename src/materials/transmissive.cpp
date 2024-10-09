#include "transmissive.h"

#include <iostream>

#include <cmath>


Transmissive::Transmissive(double idx_refraction_) { 

    idx_refraction = idx_refraction_;

}

Vector3D Transmissive::getReflectance(const Vector3D& n, const Vector3D& wo, const Vector3D& wi) const {


    return Vector3D(0.0);

};

double Transmissive::getIndexOfRefraction() const
{

    return idx_refraction;
}


Vector3D Transmissive::getEmissiveRadiance() const
{
    return Vector3D(0.0);
}

Vector3D Transmissive::getDiffuseReflectance() const
{
    return Vector3D(0.0);
}

Vector3D Transmissive::getTransmissiveRefraction(float idx_ref, Vector3D n, Vector3D wo) {

    float raiz = 1 - pow(idx_ref, 2) * (1 - pow(dot(n, wo), 2));

    Vector3D wt = dot(-idx_ref, wo) + dot(n, (idx_ref)*dot(n, wo) - sqrt(raiz));

    return wt;

}


