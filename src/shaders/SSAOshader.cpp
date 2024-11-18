#include "ssaoshader.h"
#include "../core/utils.h"

#include "../core/hemisphericalsampler.h"

SSAOshader::SSAOshader(int N_, float det_range, Vector3D bgColor_) :
    Shader(bgColor_), N(N_), detection_range(det_range)
{ }

Vector3D SSAOshader::computeColor(const Ray& r, const std::vector<Shape*>& objList, const std::vector<LightSource*>& lsList) const
{
    Intersection its;

    if (Utils::getClosestIntersection(r, objList, its)) {

        Vector3D x = its.itsPoint; // Intersection point
        Vector3D n = its.normal;   // Normal at the point of intersection

        HemisphericalSampler hem_sampler;

        // Accumulate occlusion samples
        float occlusion = 0.0f;

        for (int i = 0; i < this->N; i++) {

            Vector3D rand_dir = hem_sampler.getSample(n);

            float rand_length = (rand() / (float)RAND_MAX) * 0.9 + 0.1; // [0.1, 1]

            Ray rand_ray(x, rand_dir, 0, Epsilon, rand_length * this->detection_range);
            
            if (Utils::hasIntersection(rand_ray, objList)) {
                occlusion += 1.0f;
            }
        }

        occlusion /= (float)this->N;

        float ambient_occlusion_factor = 1.0f - occlusion;

        Vector3D baseColor = its.shape->getMaterial().getColorMaterial();

        return baseColor * ambient_occlusion_factor;
    }
    else {
        return bgColor;
    }
}



