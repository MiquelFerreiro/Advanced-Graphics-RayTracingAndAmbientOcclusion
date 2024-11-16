#ifndef TRANSMISSIVEMATERIAL
#define TRANSMISSIVEMATERIAL

#include "material.h"

class Transmissive : public Material
{
public:

    Transmissive(double idx_refraction_);

    Vector3D getReflectance(const Vector3D& n, const Vector3D& wo,
        const Vector3D& wi) const;

    bool hasSpecular() const { return false; }
    bool hasTransmission() const { return true; }
    bool hasDiffuseOrGlossy() const { return false; }
    bool isEmissive() const { return false; }

    double getIndexOfRefraction() const;
    Vector3D getEmissiveRadiance() const;
    Vector3D getDiffuseReflectance() const;

    //static Vector3D getTransmissiveRefraction(double idx_ref, Vector3D n, Vector3D wo);

    virtual Vector3D getColorMaterial() const {
        return Vector3D(0.0);
    }

private:
    float idx_refraction;
    

};
#endif // MATERIAL