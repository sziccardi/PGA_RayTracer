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
#include "PGA_3D.h"
#include "reference_classes.h"

//High resolution timer
#include <chrono>

//Scene file parser
#include "parse_pga.h"


//INTERSECTIONS
Hit findIntersection(Point3D rayStart, Line3D rayLine);
Hit findIntersection(Point3D p1, Point3D p2);
Hit lineSegmentSphereIntersect_fast(Point3D p1, Point3D p2, Point3D sphereCenter, float sphereRadius);
Hit raySphereIntersect_fast(Point3D rayStart, Line3D rayLine, Point3D sphereCenter, float sphereRadius);
Hit raySphereIntersect(Point3D rayStart, Line3D rayLine, Point3D sphereCenter, float sphereRadius);

//LIGHTING
int mCurrentDepth = 0;
Color getLighting(Hit hit, Dir3D ray);
Color evaluateRayTree(Point3D rayStart, Dir3D ray);

// Sampling
float randomPixelLocationNoise(float noiseSize);
