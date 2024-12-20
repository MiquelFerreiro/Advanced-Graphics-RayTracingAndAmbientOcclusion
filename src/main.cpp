#include <iostream>
#include <algorithm>

#include "core/film.h"
#include "core/matrix4x4.h"
#include "core/ray.h"
#include "core/utils.h"
#include "core/scene.h"


#include "shapes/sphere.h"
#include "shapes/infiniteplan.h"

#include "cameras/ortographic.h"
#include "cameras/perspective.h"

#include "shaders/intersectionshader.h"
#include "shaders/depthshader.h"
#include "shaders/normalshader.h"
#include "shaders/whittedshader.h"
#include "shaders/hemispherical.h"
#include "shaders/areadirectshader.h"
#include "shaders/pathtracershader.h"
#include "shaders/neeshader.h"


#include "materials/phong.h"
#include "materials/emissive.h"
#include "materials/mirror.h"
#include "materials/transmissive.h"

//COSAS NUEVAS AO

#include "shaders/basecolorshader.h"
#include "shaders/SSAOshader.h"




////////////////////////////

#include <chrono>

using namespace std::chrono;

typedef std::chrono::duration<double, std::milli> durationMs;

void buildSceneCornellBox(Camera*& cam, Film*& film,
    Scene myScene)
{
    /* **************************** */
/* Declare and place the camera */
/* **************************** */
    Matrix4x4 cameraToWorld = Matrix4x4::translate(Vector3D(0, 0, -3));
    double fovDegrees = 60;
    double fovRadians = Utils::degreesToRadians(fovDegrees);
    cam = new PerspectiveCamera(cameraToWorld, fovRadians, *film);

    /* ********* */
    /* Materials */
    /* ********* */

    Material* redDiffuse = new Phong(Vector3D(0.7, 0.2, 0.3), Vector3D(0, 0, 0), 100);
    Material* greenDiffuse = new Phong(Vector3D(0.2, 0.7, 0.3), Vector3D(0, 0, 0), 100);
    Material* greyDiffuse = new Phong(Vector3D(0.8, 0.8, 0.8), Vector3D(0, 0, 0), 100);      
    Material* blueGlossy_20 = new Phong(Vector3D(0.2, 0.3, 0.8), Vector3D(0.2, 0.2, 0.2), 20);
    Material* blueGlossy_80 = new Phong(Vector3D(0.2, 0.3, 0.8), Vector3D(0.2, 0.2, 0.2), 80);
    Material* cyandiffuse = new Phong(Vector3D(0.2, 0.8, 0.8), Vector3D(0, 0, 0), 100);
    //Material* emissive = new Emissive(Vector3D(25, 25, 25), Vector3D(0.5));

    Material* mirror = new Mirror();
    Material* transmissive = new Transmissive(0.7);

    /* ******* */
    /* Objects */
    /* ******* */
    double offset = 3.0;
    Matrix4x4 idTransform;
    // Construct the Cornell Box
    Shape* leftPlan = new InfinitePlan(Vector3D(-offset - 1, 0, 0), Vector3D(1, 0, 0), redDiffuse);
    Shape* rightPlan = new InfinitePlan(Vector3D(offset + 1, 0, 0), Vector3D(-1, 0, 0), greenDiffuse);
    Shape* topPlan = new InfinitePlan(Vector3D(0, offset, 0), Vector3D(0, -1, 0), greyDiffuse);
    Shape* bottomPlan = new InfinitePlan(Vector3D(0, -offset, 0), Vector3D(0, 1, 0), greyDiffuse);
    Shape* backPlan = new InfinitePlan(Vector3D(0, 0, 3 * offset), Vector3D(0, 0, -1), greyDiffuse);
    //Shape* square_emissive = new Square(Vector3D(-1.0, 3.0, 3.0), Vector3D(2.0, 0.0, 0.0), Vector3D(0.0, 0.0, 2.0), Vector3D(0.0, -1.0, 0.0), emissive);


    myScene.AddObject(leftPlan);
    myScene.AddObject(rightPlan);
    myScene.AddObject(topPlan);
    myScene.AddObject(bottomPlan);
    myScene.AddObject(backPlan);
    //myScene.AddObject(square_emissive);

    //Shape* square = new Square(Vector3D(offset + 0.999, -offset-0.2, 3.0), Vector3D(0.0, 4.0, 0.0), Vector3D(0.0, 0.0, 2.0), Vector3D(-1.0, 0.0, 0.0), mirror);
    //Shape* square = new Square(Vector3D(offset + 0.999, -offset - 0.2, 3.0), Vector3D(0.0, 4.0, 0.0), Vector3D(0.0, 0.0, 2.0), Vector3D(-1.0, 0.0, 0.0), cyandiffuse);

    //myScene.AddObject(square);

    // Place the Spheres inside the Cornell Box

    std::vector<double> radius = { 2, 0.5, 0.7, 1, 0.6, 0.8, 1, 0.6, 0.4 };

    //X, Y, Z --> proporcional a (derecha, arriba, + lejos)
    //std::vector<Vector3D> positions = { Vector3D(0, -offset + 1.5, 3), Vector3D(1.5, -offset + 1, 0)};

    std::vector<Vector3D> positions = { Vector3D(4, -3, 9), Vector3D(2, -offset, 3.5), Vector3D(1.4, -offset, 4), 
        Vector3D(-2, -1.5, 5), Vector3D(-1, -0.5, 4), Vector3D(1, 2, 9), Vector3D(4, 3, 4), Vector3D(-1, 3, 4), Vector3D(-2.5, 0, 5.5) };

    Material* materials[] = { blueGlossy_80, redDiffuse, cyandiffuse, greenDiffuse, blueGlossy_80, greenDiffuse, redDiffuse, blueGlossy_80, cyandiffuse };

    for (int idx = 0; idx < radius.size(); idx++) {

        Matrix4x4 sphereTransform;
        sphereTransform = Matrix4x4::translate(positions[idx]);

        Shape* sphere = new Sphere(radius[idx], sphereTransform, materials[idx]);

        myScene.AddObject(sphere);
    }


}

void buildSceneSphere(Camera*& cam, Film*& film,
    Scene myScene)
{
    /* **************************** */
      /* Declare and place the camera */
      /* **************************** */
      // By default, this gives an ID transform
      //  which means that the camera is located at (0, 0, 0)
      //  and looking at the "+z" direction
    Matrix4x4 cameraToWorld;
    double fovDegrees = 60;
    double fovRadians = Utils::degreesToRadians(fovDegrees);
    cam = new PerspectiveCamera(cameraToWorld, fovRadians, *film);



    /* ************************** */
    /* DEFINE YOUR MATERIALS HERE */
    /* ************************** */

    Material* green_100 = new Phong(Vector3D(0.2, 0.7, 0.3), Vector3D(0.2, 0.6, 0.2), 50);

    // Define and place a sphere
    Matrix4x4 sphereTransform1;
    sphereTransform1 = sphereTransform1.translate(Vector3D(-1.25, 0.5, 4.0));
    Shape* s1 = new Sphere(1.0, sphereTransform1, green_100);

    // Define and place a sphere
    Matrix4x4 sphereTransform2;
    sphereTransform2 = sphereTransform2.translate(Vector3D(1.25, 0.0, 6));
    Shape* s2 = new Sphere(1.25, sphereTransform2, green_100);

    // Define and place a sphere
    Matrix4x4 sphereTransform3;
    sphereTransform3 = sphereTransform3.translate(Vector3D(1.0, -0.75, 3.5));
    Shape* s3 = new Sphere(0.25, sphereTransform3, green_100);

    // Store the objects in the object list
    myScene.AddObject(s1);
    myScene.AddObject(s2);
    myScene.AddObject(s3);
   
}

void raytrace(Camera* &cam, Shader* &shader, Film* &film,
              std::vector<Shape*>* &objectsList, std::vector<LightSource*>* &lightSourceList)
{

    size_t resX = film->getWidth();
    size_t resY = film->getHeight();

    // Main raytracing loop
    // Out-most loop invariant: we have rendered lin lines
    for(size_t lin=0; lin<resY; lin++)
    {
        // Show progression 
        double progress = (double)lin / double(resY);
        Utils::printProgress(progress);

        // Inner loop invariant: we have rendered col columns
        for(size_t col=0; col<resX; col++)
        {
            // Compute the pixel position in NDC
            double x = (double)(col + 0.5) / resX;
            double y = (double)(lin + 0.5) / resY;
            // Generate the camera ray
            Ray cameraRay = cam->generateRay(x, y);
            Vector3D pixelColor = Vector3D(0.0);

            // Compute ray color according to the used shader
            pixelColor += shader->computeColor(cameraRay, *objectsList, *lightSourceList);

            // Store the pixel color
            film->setPixelValue(col, lin, pixelColor);
        }
    }
}


//------------TASK 1---------------------//
void PaintImage(Film* film)
{
    unsigned int sizeBar = 40;

    size_t resX = film->getWidth();
    size_t resY = film->getHeight();

    // Main Image Loop
    for (size_t lin = 0; lin < resY; lin++)
    {
        // Show progression
        if (lin % (resY / sizeBar) == 0)
            std::cout << ".";

        for (size_t col = 0; col < resX; col++)
        { 
            //CHANGE...()
            //Vector3D random_color = Vector3D((double)rand() / RAND_MAX, (double)rand() / RAND_MAX, (double)rand() / RAND_MAX);  

            Vector3D color_Task1 = Vector3D((double) col/resX, (double) lin/resY, 0);

            film->setPixelValue(col,lin, color_Task1);
           
        }
    }
}


int main()
{
    std::string separator     = "\n----------------------------------------------\n";
    std::string separatorStar = "\n**********************************************\n";
    std::cout << separator << "RT-ACG - Ray Tracer for \"Advanced Computer Graphics\"" << separator << std::endl;

    // Create an empty film
    Film *film;
    film = new Film(720, 512);

    // Declare the shader
    Vector3D bgColor(0.0, 0.0, 0.0); // Background color (for rays which do not intersect anything)
    Vector3D intersectionColor(1,0,0);
    Vector3D ambientLight(0.2, 0.2, 0.2);
    
    {


    ////First Assignment
    //Shader *shader = new IntersectionShader (intersectionColor, bgColor);
    //Shader *depthshader = new DepthShader (intersectionColor,10.0f, bgColor);
    //Shader *normalshader = new NormalShader(intersectionColor, 10.0f, bgColor);
    //Shader *whittedshader = new WhittedShader(intersectionColor, 10.0f, bgColor, ambientLight);
    //

    ////Lab 2 Part 1

    //Shader* hemispherical = new Hemispherical(intersectionColor, 10.0f, bgColor, ambientLight, 32);

    //Shader* areadirectshader = new AreaDirectShader(intersectionColor, 10.0f, bgColor, ambientLight, 32);

    ////Lab 2 Part 2

    //Shader* pathtracershader = new PathTracerShader(0.7, 2, 64);

    //Shader* neeshader = new NEEShader(0.7, 4, 64);

    //Create Scene Geometry and Illumiantion
    //buildSceneSphere(cam, film, myScene); //Task 2,3,4;

    }

    // Build the scene---------------------------------------------------------
    // Declare pointers to all the variables which describe the scene

    Camera* cam;
    Scene myScene;

    buildSceneCornellBox(cam, film, myScene); //Task 5

    // Launch some rays! TASK 2,3,...   
    auto start = high_resolution_clock::now();

    //---------------------------------------------------------------------------
    {
 //Paint Image ONLY TASK 1
 //PaintImage(film);
    }

    // Lab 1
    {
        //TASK 2
        //raytrace(cam, shader, film, myScene.objectsList, myScene.LightSourceList);
        //TASK3
        //raytrace(cam, depthshader, film, myScene.objectsList, myScene.LightSourceList);
        //TASK4
        //raytrace(cam, normalshader, film, myScene.objectsList, myScene.LightSourceList);
        //TASK5
        //raytrace(cam, whittedshader, film, myScene.objectsList, myScene.LightSourceList);
    }

    //Lab 2
    {
        // Lab 2 Part 1

        //raytrace(cam, hemispherical, film, myScene.objectsList, myScene.LightSourceList);

        //raytrace(cam, areadirectshader, film, myScene.objectsList, myScene.LightSourceList);

        // Lab 2 Part 2

        //raytrace(cam, pathtracershader, film, myScene.objectsList, myScene.LightSourceList);

        //raytrace(cam, neeshader, film, myScene.objectsList, myScene.LightSourceList);
    }

    // SHADERS AMBIENT OCCLUSION

    Shader* basecolorshader = new BaseColorShader(bgColor);

    //raytrace(cam, basecolorshader, film, myScene.objectsList, myScene.LightSourceList);

    int N = 512;

    float detection_range = 5;

    Shader* ssaoshader = new SSAOshader(N, detection_range, bgColor);

    raytrace(cam, ssaoshader, film, myScene.objectsList, myScene.LightSourceList);



    ///////////////////////////////////////

    auto stop = high_resolution_clock::now();

    // Save the final result to file
    std::cout << "\n\nSaving the result to file output.bmp\n" << std::endl;
    film->save();
    film->saveEXR();

    float durationS = (durationMs(stop - start) / 1000.0).count() ;
    std::cout <<  "FINAL_TIME(s): " << durationS << std::endl;


    std::cout << "\n\n" << std::endl;
    return 0;
}
