#include "arealightsource.h"
#include <random>

AreaLightSource::AreaLightSource(Square* areaLightsource_) :
    myAreaLightsource(areaLightsource_)
{ }

Vector3D AreaLightSource::getIntensity() const
{
    return myAreaLightsource->getMaterial().getEmissiveRadiance();
}


Vector3D AreaLightSource::sampleLightPosition()   const
{
    Vector3D corner = myAreaLightsource->corner;
    Vector3D v1 = myAreaLightsource->v1;
    Vector3D v2 = myAreaLightsource->v2;

    double a = (rand() % 101) / 100.0;
    double b = (rand() % 101) / 100.0;

    Vector3D sampledPosition = corner + v1 * a + v2 * b;

    return sampledPosition;
}

