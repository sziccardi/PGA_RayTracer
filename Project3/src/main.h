#pragma once
//For Visual Studios
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <algorithm>

//Images Lib includes:
#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "image_lib.h" //Defines an image class and a color class

//#3D PGA
#include "print_PGA.h"
#include "reference_classes.h"

//High resolution timer
#include <chrono>

//Scene file parser
#include "parse_pga.h"

float pi = 3.1415926;

//INTERSECTIONS
Hit findIntersection(Point3D rayStart, Line3D rayLine);
Hit findIntersection(Point3D p1, Point3D p2);
Hit lineSegmentSphereIntersect_fast(Point3D p1, Point3D p2, Point3D sphereCenter, float sphereRadius);
Hit raySphereIntersect_fast(Point3D rayStart, Line3D rayLine, Point3D sphereCenter, float sphereRadius);
Hit raySphereIntersect(Point3D rayStart, Line3D rayLine, Point3D sphereCenter, float sphereRadius);

Hit rayTriIntersect(Point3D rayStart, Line3D rayLine, Triangle tri, Material m);
Hit rayTriIntersect(Point3D rayStart, Point3D rayEnd, Triangle tri, Material m);
Coord3D getBarycentricCoord(Point3D p, Triangle tri);


//LIGHTING
Dir3D getReflectRay(Hit hit, Dir3D ray);
Dir3D getRefrectRay(Dir3D normal, Dir3D ray, float n);
Color getLighting(Hit hit, Point3D rayStart, Dir3D ray, int currentDepth);
Color evaluateRayTree(Point3D rayStart, Dir3D ray, int currentDepth);

// Sampling
float randomPixelLocationNoise(float noiseSize);
