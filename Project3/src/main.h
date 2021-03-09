#pragma once
//For Visual Studios
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#endif

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
Hit findIntersection(Point3D rayStart, Line3D rayLine); //TODO figure out what this should return
Hit raySphereIntersect_fast(Point3D rayStart, Line3D rayLine, Point3D sphereCenter, float sphereRadius);
Hit raySphereIntersect(Point3D rayStart, Line3D rayLine, Point3D sphereCenter, float sphereRadius);

//LIGHTING
Color getLighting(Hit intersection);