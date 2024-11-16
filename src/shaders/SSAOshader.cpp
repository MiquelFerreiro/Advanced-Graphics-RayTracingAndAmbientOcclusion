#include "ssaoshader.h"
#include "../core/utils.h"

#include "../core/hemisphericalsampler.h"

SSAOshader::SSAOshader(int N_, Vector3D bgColor_) :
    Shader(bgColor_), N(N_)
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

            Ray rand_ray(x, rand_dir, 0, Epsilon, rand_length);
            
            if (Utils::hasIntersection(rand_ray, objList)) {
                // If the ray hits an object, increment occlusion
                occlusion += 1.0f;
            }
        }

        // Normalize the occlusion factor (0 to 1)
        occlusion /= (float)this->N;

        // Ambient occlusion factor: less occlusion means more exposure to ambient light
        // Inverse occlusion is used for better visual contrast
        float ambient_occlusion_factor = 1.0f - occlusion;

        // Get the base color of the material
        Vector3D baseColor = its.shape->getMaterial().getColorMaterial();

        // Multiply the base color by the ambient occlusion factor to darken the color based on occlusion
        return baseColor * ambient_occlusion_factor;
    }
    else {
        return bgColor; // Background color if no intersection
    }
}



