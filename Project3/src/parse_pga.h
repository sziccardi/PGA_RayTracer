
//Set the global scene parameter variables
//TODO: Set the scene parameters based on the values in the scene file

#ifndef PARSE_PGA_H
#define PARSE_PGA_H

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <utility> 
#include <cmath>
#include <string>
#include <vector>

using std::cout;
using std::endl;

//Camera & Scene Parmaters (Global Variables)
//Here we set default values, override them in parseSceneFile()

//Image Parmaters
int img_width = 640, img_height = 480;
std::string imgName = "raytraced.bmp";

//Camera Parmaters
Point3D eye = Point3D(0,0,0); 
Dir3D forward = Dir3D(0,0,-1).normalized();
Dir3D up = Dir3D(0,1,0).normalized();
Dir3D right = Dir3D(1,0,0).normalized();
float halfAngleVFOV = 45; 

//Scene (Sphere) Parmaters
std::vector<Sphere> spheres;
std::vector<Light*> lights;
Material currentMaterial = Material(0, 0, 0, 1, 1, 1, 0, 0, 0, 5, 0, 0, 0, 1);
Color background = Color(0, 0, 0);
float maxDepth = 5.f;

// Sampling
Sampling sampling = Sampling(NONE);
std::vector<Point3D> vertices;
std::vector<Triangle> triangles;
std::vector<Dir3D> normals;
int maxVertices = 10000;
int maxNormals = 10000;


void resetScene() {
    spheres.clear();
    lights.clear();
    currentMaterial = Material(0, 0, 0, 1, 1, 1, 0, 0, 0, 5, 0, 0, 0, 1);
    background = Color(0, 0, 0);
    maxDepth = 5.f;
}

void parseSceneFile(std::string fileName){
    resetScene();
    std::string line;
    std::ifstream myFile;
	myFile.open(fileName);
    if (myFile.is_open()) {
        while (std::getline(myFile, line)) {
            std::size_t commentFound = line.find("#");

            if (commentFound != std::string::npos) {
               cout << "found a comment!" << endl;
           }
           else {
                int iter = 0;
               std::string command = "output_image:";
               std::size_t found = line.find(command);
               if (found != std::string::npos) {
                   imgName = "";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   imgName = subString.substr(start, end);
                   cout << "output image = " << imgName << endl;
               }

               command = "sphere:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new sphere = (";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringX = subString.substr(start);
                   if (end >= 0) stringX = subString.substr(start, start + end);
                   float x = std::stof(stringX);
                   cout << std::to_string(x) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringY = subString.substr(start);
                   if (end >= 0) stringY = subString.substr(start, start + end);
                   float y = std::stof(stringY);
                   cout << std::to_string(y) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringZ = subString.substr(start);
                   if (end >= 0) stringZ = subString.substr(start, start + end);
                   float z = std::stof(stringZ);
                   cout << std::to_string(z) << ") ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringR = subString.substr(start);
                   if (end >= 0) stringR = subString.substr(start, start + end);
                   float r = std::stof(stringR);

                   cout << "r = " << std::to_string(r) << endl;

                   spheres.push_back(Sphere(currentMaterial, Point3D(x, y, z), r));
               }

               command = "vertex:";
               found = line.find(command);
               if (found != std::string::npos) {
                    if (vertices.size() < maxVertices) {
                        cout << "new vertex = (";
                        iter = found + command.length();
                        std::string subString = line.substr(iter);
                        int start = subString.find_first_not_of(" \t\r\n");
                        int end = subString.substr(start).find_first_of(" \t\r\n");
                        std::string stringX = subString.substr(start);
                        if (end >= 0) stringX = subString.substr(start, start + end);
                        float x = std::stof(stringX);
                        cout << std::to_string(x) << ", ";

                        subString = subString.substr(end + start);
                        start = subString.find_first_not_of(" \t\r\n");
                        end = subString.substr(start).find_first_of(" \t\r\n");
                        std::string stringY = subString.substr(start);
                        if (end >= 0) stringY = subString.substr(start, start + end);
                        float y = std::stof(stringY);
                        cout << std::to_string(y) << ", ";

                        subString = subString.substr(end + start);
                        start = subString.find_first_not_of(" \t\r\n");
                        end = subString.substr(start).find_first_of(" \t\r\n");
                        std::string stringZ = subString.substr(start);
                        if (end >= 0) stringZ = subString.substr(start, start + end);
                        float z = std::stof(stringZ);
                        cout << std::to_string(z) << ") " << std::endl;

                        vertices.push_back(Point3D(x, y, z));
                   }
               }

               command = "normal:";
               found = line.find(command);
               if (found != std::string::npos) {
                    if (normals.size() < maxNormals) {
                        cout << "new normal = (";
                        iter = found + command.length();
                        std::string subString = line.substr(iter);
                        int start = subString.find_first_not_of(" \t\r\n");
                        int end = subString.substr(start).find_first_of(" \t\r\n");
                        std::string stringX = subString.substr(start);
                        if (end >= 0) stringX = subString.substr(start, start + end);
                        float x = std::stof(stringX);
                        cout << std::to_string(x) << ", ";

                        subString = subString.substr(end + start);
                        start = subString.find_first_not_of(" \t\r\n");
                        end = subString.substr(start).find_first_of(" \t\r\n");
                        std::string stringY = subString.substr(start);
                        if (end >= 0) stringY = subString.substr(start, start + end);
                        float y = std::stof(stringY);
                        cout << std::to_string(y) << ", ";

                        subString = subString.substr(end + start);
                        start = subString.find_first_not_of(" \t\r\n");
                        end = subString.substr(start).find_first_of(" \t\r\n");
                        std::string stringZ = subString.substr(start);
                        if (end >= 0) stringZ = subString.substr(start, start + end);
                        float z = std::stof(stringZ);
                        cout << std::to_string(z) << ") " << std::endl;

                        normals.push_back(Dir3D(x, y, z));
                   }
               }

               command = "normal_triangle:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new directional light " << endl;
                   cout << "color = (";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringR = subString.substr(start);
                   if (end >= 0) stringR = subString.substr(start, start + end);
                   float r = std::stof(stringR);
                   cout << std::to_string(r) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringG = subString.substr(start);
                   if (end >= 0) stringG = subString.substr(start, start + end);
                   float g = std::stof(stringG);
                   cout << std::to_string(g) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringB = subString.substr(start);
                   if (end >= 0) stringB = subString.substr(start, start + end);
                   float b = std::stof(stringB);
                   cout << std::to_string(b) << ") " << endl;

                   cout << "direction = ( ";
                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringX = subString.substr(start);
                   if (end >= 0) stringX = subString.substr(start, start + end);
                   float x = std::stof(stringX);
                   cout << std::to_string(x) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringY = subString.substr(start);
                   if (end >= 0) stringY = subString.substr(start, start + end);
                   float y = std::stof(stringY);
                   cout << std::to_string(y) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringZ = subString.substr(start);
                   if (end >= 0) stringZ = subString.substr(start, start + end);
                   float z = std::stof(stringZ);
                   cout << std::to_string(z) << ") " << endl;

                   triangles.push_back(NormalTriangle(currentMaterial, vertices[r], vertices[g], vertices[b], normals[x], normals[y], normals[z]));
               } else {
                //    If we find a normal triangle on a line, we don't want to also find a triangle
                    command = "triangle:";
                    found = line.find(command);
                    if (found != std::string::npos) {
                            if (triangles.size() < maxNormals) {
                                cout << "new vertex = (";
                                iter = found + command.length();
                                std::string subString = line.substr(iter);
                                int start = subString.find_first_not_of(" \t\r\n");
                                int end = subString.substr(start).find_first_of(" \t\r\n");
                                std::string stringX = subString.substr(start);
                                if (end >= 0) stringX = subString.substr(start, start + end);
                                int x = std::atoi(stringX.c_str());
                                cout << std::to_string(x) << ", ";

                                subString = subString.substr(end + start);
                                start = subString.find_first_not_of(" \t\r\n");
                                end = subString.substr(start).find_first_of(" \t\r\n");
                                std::string stringY = subString.substr(start);
                                if (end >= 0) stringY = subString.substr(start, start + end);
                                int y = std::atoi(stringY.c_str());
                                cout << std::to_string(y) << ", ";

                                subString = subString.substr(end + start);
                                start = subString.find_first_not_of(" \t\r\n");
                                end = subString.substr(start).find_first_of(" \t\r\n");
                                std::string stringZ = subString.substr(start);
                                if (end >= 0) stringZ = subString.substr(start, start + end);
                                int z = std::atoi(stringZ.c_str());
                                cout << std::to_string(z) << ") " << std::endl;

                                triangles.push_back(Triangle(currentMaterial, vertices[x], vertices[y], vertices[z]));
                        }
                    }

               }

               command = "film_resolution:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new image resolution = (";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringW = subString.substr(start);
                   if (end >= 0) stringW = subString.substr(start, start + end);
                   float w = std::stof(stringW);
                   cout << std::to_string(w) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringH = subString.substr(start);
                   if (end >= 0) stringH = subString.substr(start, start + end);
                   float h = std::stof(stringH);
                   cout << std::to_string(h) << ")" << endl;

                   img_width = w;
                   img_height = h;
               }

               command = "camera_pos:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new camera pos = (";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringX = subString.substr(start);
                   if (end >= 0) stringX = subString.substr(start, start + end);
                   float x = std::stof(stringX);
                   cout << std::to_string(x) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringY = subString.substr(start);
                   if (end >= 0) stringY = subString.substr(start, start + end);
                   float y = std::stof(stringY);
                   cout << std::to_string(y) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringZ = subString.substr(start);
                   if (end >= 0) stringZ = subString.substr(start, start + end);
                   float z = std::stof(stringZ);
                   cout << std::to_string(z) << ") " << endl;

                   eye = Point3D(x, y, z);
               }

               command = "camera_fwd:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new camera fwd = (";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringX = subString.substr(start);
                   if (end >= 0) stringX = subString.substr(start, start + end);
                   float x = std::stof(stringX);
                   cout << std::to_string(x) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringY = subString.substr(start);
                   if (end >= 0) stringY = subString.substr(start, start + end);
                   float y = std::stof(stringY);
                   cout << std::to_string(y) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringZ = subString.substr(start);
                   if (end >= 0) stringZ = subString.substr(start, start + end);
                   float z = std::stof(stringZ);
                   cout << std::to_string(z) << ") " << endl;

                   forward = Dir3D(x, y, z);
               }

               command = "camera_up:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new camera up = (";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringX = subString.substr(start);
                   if (end >= 0) stringX = subString.substr(start, start + end);
                   float x = std::stof(stringX);
                   cout << std::to_string(x) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringY = subString.substr(start);
                   if (end >= 0) stringY = subString.substr(start, start + end);
                   float y = std::stof(stringY);
                   cout << std::to_string(y) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringZ = subString.substr(start);
                   if (end >= 0) stringZ = subString.substr(start, start + end);
                   float z = std::stof(stringZ);
                   cout << std::to_string(z) << ") " << endl;

                   up = Dir3D(x, y, z);
               }

               command = "camera_fov_ha:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new camera fov ha = ";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringFOV = subString.substr(start);
                   if (end >= 0) stringFOV = subString.substr(start, start + end);
                   float ha = std::stof(stringFOV);
                   cout << std::to_string(ha) << endl;

                   halfAngleVFOV = ha;
               }

               command = "max_vertices:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "max vertices = ";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringFOV = subString.substr(start);
                   if (end >= 0) stringFOV = subString.substr(start, start + end);
                   int maxVerts = std::atoi(stringFOV.c_str());
                   cout << std::to_string(maxVerts) << endl;

                   maxVertices = maxVerts;
               }

               command = "max_normals:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "max normals = ";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringFOV = subString.substr(start);
                   if (end >= 0) stringFOV = subString.substr(start, start + end);
                   int maxNorms = std::atoi(stringFOV.c_str());
                   cout << std::to_string(maxNorms) << endl;

                   maxNormals = maxNorms;
               }

               command = "background:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new background color = (";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringR = subString.substr(start);
                   if (end >= 0) stringR = subString.substr(start, start + end);
                   float r = std::stof(stringR);
                   cout << std::to_string(r) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringG = subString.substr(start);
                   if (end >= 0) stringG = subString.substr(start, start + end);
                   float g = std::stof(stringG);
                   cout << std::to_string(g) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringB = subString.substr(start);
                   if (end >= 0) stringB = subString.substr(start, start + end);
                   float b = std::stof(stringB);
                   cout << std::to_string(b) << ") " << endl;

                   background = Color(r, g, b);
               }

               command = "material:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new material" << endl;
                   cout << "ambient color = (";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringR = subString.substr(start);
                   if (end >= 0) stringR = subString.substr(start, start + end);
                   float r = std::stof(stringR);
                   cout << std::to_string(r) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringG = subString.substr(start);
                   if (end >= 0) stringG = subString.substr(start, start + end);
                   float g = std::stof(stringG);
                   cout << std::to_string(g) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringB = subString.substr(start);
                   if (end >= 0) stringB = subString.substr(start, start + end);
                   float b = std::stof(stringB);
                   cout << std::to_string(b) << ") " << endl;

                   Color ambient = Color(r, g, b);

                   cout << "diffuse color = (";
                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   stringR = subString.substr(start);
                   if (end >= 0) stringR = subString.substr(start, start + end);
                   r = std::stof(stringR);
                   cout << std::to_string(r) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   stringG = subString.substr(start);
                   if (end >= 0) stringG = subString.substr(start, start + end);
                   g = std::stof(stringG);
                   cout << std::to_string(g) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   stringB = subString.substr(start);
                   if (end >= 0) stringB = subString.substr(start, start + end);
                   b = std::stof(stringB);
                   cout << std::to_string(b) << ") " << endl;

                   Color diffuse = Color(r, g, b);

                   cout << "specular color = (";
                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   stringR = subString.substr(start);
                   if (end >= 0) stringR = subString.substr(start, start + end);
                   r = std::stof(stringR);
                   cout << std::to_string(r) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   stringG = subString.substr(start);
                   if (end >= 0) stringG = subString.substr(start, start + end);
                   g = std::stof(stringG);
                   cout << std::to_string(g) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   stringB = subString.substr(start);
                   if (end >= 0) stringB = subString.substr(start, start + end);
                   b = std::stof(stringB);
                   cout << std::to_string(b) << ") " << endl;
                   Color specular = Color(r, g, b);

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringNS = subString.substr(start);
                   if (end >= 0) stringNS = subString.substr(start, start + end);
                   float ns = std::stof(stringNS);
                   cout << "new ns = " << std::to_string(ns) << endl;

                   cout << "transmissive color = (";
                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   stringR = subString.substr(start);
                   if (end >= 0) stringR = subString.substr(start, start + end);
                   r = std::stof(stringR);
                   cout << std::to_string(r) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   stringG = subString.substr(start);
                   if (end >= 0) stringG = subString.substr(start, start + end);
                   g = std::stof(stringG);
                   cout << std::to_string(g) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   stringB = subString.substr(start);
                   if (end >= 0) stringB = subString.substr(start, start + end);
                   b = std::stof(stringB);
                   cout << std::to_string(b) << ") " << endl;
                   Color transmissive = Color(r, g, b);

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringIOR = subString.substr(start);
                   if (end >= 0) stringIOR = subString.substr(start, start + end);
                   float ior = std::stof(stringIOR);
                   cout << "new ior = " << std::to_string(ior) << endl;

                   currentMaterial = Material(ambient, diffuse, specular, ns, transmissive, ior);
               }

               command = "point_light:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new point light " << endl;
                   cout << "color = (";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringR = subString.substr(start);
                   if (end >= 0) stringR = subString.substr(start, start + end);
                   float r = std::stof(stringR);
                   cout << std::to_string(r) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringG = subString.substr(start);
                   if (end >= 0) stringG = subString.substr(start, start + end);
                   float g = std::stof(stringG);
                   cout << std::to_string(g) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringB = subString.substr(start);
                   if (end >= 0) stringB = subString.substr(start, start + end);
                   float b = std::stof(stringB);
                   cout << std::to_string(b) << ") " << endl;

                   cout << "position = ( ";
                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringX = subString.substr(start);
                   if (end >= 0) stringX = subString.substr(start, start + end);
                   float x = std::stof(stringX);
                   cout << std::to_string(x) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringY = subString.substr(start);
                   if (end >= 0) stringY = subString.substr(start, start + end);
                   float y = std::stof(stringY);
                   cout << std::to_string(y) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringZ = subString.substr(start);
                   if (end >= 0) stringZ = subString.substr(start, start + end);
                   float z = std::stof(stringZ);
                   cout << std::to_string(z) << ") " << endl;

                   lights.push_back(new PointLight(Color(r, g, b), Point3D(x, y, z)));
               }

               command = "directional_light:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new directional light " << endl;
                   cout << "color = (";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringR = subString.substr(start);
                   if (end >= 0) stringR = subString.substr(start, start + end);
                   float r = std::stof(stringR);
                   cout << std::to_string(r) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringG = subString.substr(start);
                   if (end >= 0) stringG = subString.substr(start, start + end);
                   float g = std::stof(stringG);
                   cout << std::to_string(g) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringB = subString.substr(start);
                   if (end >= 0) stringB = subString.substr(start, start + end);
                   float b = std::stof(stringB);
                   cout << std::to_string(b) << ") " << endl;

                   cout << "direction = ( ";
                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringX = subString.substr(start);
                   if (end >= 0) stringX = subString.substr(start, start + end);
                   float x = std::stof(stringX);
                   cout << std::to_string(x) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringY = subString.substr(start);
                   if (end >= 0) stringY = subString.substr(start, start + end);
                   float y = std::stof(stringY);
                   cout << std::to_string(y) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringZ = subString.substr(start);
                   if (end >= 0) stringZ = subString.substr(start, start + end);
                   float z = std::stof(stringZ);
                   cout << std::to_string(z) << ") " << endl;

                   lights.push_back(new DirectionalLight(Color(r, g, b), Dir3D(x, y, z)));
               }

               command = "spot_light:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new spot light " << endl;
                   cout << "color = (";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringR = subString.substr(start);
                   if (end >= 0) stringR = subString.substr(start, start + end);
                   float r = std::stof(stringR);
                   cout << std::to_string(r) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringG = subString.substr(start);
                   if (end >= 0) stringG = subString.substr(start, start + end);
                   float g = std::stof(stringG);
                   cout << std::to_string(g) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringB = subString.substr(start);
                   if (end >= 0) stringB = subString.substr(start, start + end);
                   float b = std::stof(stringB);
                   cout << std::to_string(b) << ") " << endl;

                   cout << "position = ( ";
                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringX = subString.substr(start);
                   if (end >= 0) stringX = subString.substr(start, start + end);
                   float x = std::stof(stringX);
                   cout << std::to_string(x) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringY = subString.substr(start);
                   if (end >= 0) stringY = subString.substr(start, start + end);
                   float y = std::stof(stringY);
                   cout << std::to_string(y) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringZ = subString.substr(start);
                   if (end >= 0) stringZ = subString.substr(start, start + end);
                   float z = std::stof(stringZ);
                   cout << std::to_string(z) << ") " << endl;

                   cout << "direction = ( ";
                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringDX = subString.substr(start);
                   if (end >= 0) stringDX = subString.substr(start, start + end);
                   float dx = std::stof(stringDX);
                   cout << std::to_string(dx) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringDY = subString.substr(start);
                   if (end >= 0) stringDY = subString.substr(start, start + end);
                   float dy = std::stof(stringDY);
                   cout << std::to_string(dy) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringDZ = subString.substr(start);
                   if (end >= 0) stringDZ = subString.substr(start, start + end);
                   float dz = std::stof(stringDZ);
                   cout << std::to_string(dz) << ") " << endl;


                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringA1 = subString.substr(start);
                   if (end >= 0) stringA1 = subString.substr(start, start + end);
                   float a1 = std::stof(stringA1);
                   cout << "minAngle = " << std::to_string(a1) << endl;

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringA2 = subString.substr(start);
                   if (end >= 0) stringA2 = subString.substr(start, start + end);
                   float a2 = std::stof(stringA2);
                   cout << "minAngle = " << std::to_string(a2) << endl;

                   lights.push_back(new SpotLight(Color(r, g, b), Point3D(x, y, z), Dir3D(dx, dy, dz), a1, a2));
               }

               command = "ambient_light:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new ambient light with color = (";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringR = subString.substr(start);
                   if (end >= 0) stringR = subString.substr(start, start + end);
                   float r = std::stof(stringR);
                   cout << std::to_string(r) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringG = subString.substr(start);
                   if (end >= 0) stringG = subString.substr(start, start + end);
                   float g = std::stof(stringG);
                   cout << std::to_string(g) << ", ";

                   subString = subString.substr(end + start);
                   start = subString.find_first_not_of(" \t\r\n");
                   end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringB = subString.substr(start);
                   if (end >= 0) stringB = subString.substr(start, start + end);
                   float b = std::stof(stringB);
                   cout << std::to_string(b) << ") " << endl;

                   lights.push_back(new Light(Color(r, g, b)));
               }

               command = "max_depth:";
               found = line.find(command);
               if (found != std::string::npos) {
                   cout << "new max depth = ";
                   iter = found + command.length();
                   std::string subString = line.substr(iter);
                   int start = subString.find_first_not_of(" \t\r\n");
                   int end = subString.substr(start).find_first_of(" \t\r\n");
                   std::string stringN = subString.substr(start);
                   if (end >= 0) stringN = subString.substr(start, start + end);
                   float n = std::stof(stringN);
                   cout << std::to_string(n) << endl;

                   maxDepth = n;
               }

               command = "sampling_method:";
               found = line.find(command);
               if (found != std::string::npos) {
                    iter = found + command.length();
                    std::string subString = line.substr(iter);
                    int start = subString.find_first_not_of(" \t\r\n");
                    int end = subString.substr(start).find_first_of(" \t\r\n");
                    std::string samplingName = subString.substr(start, end);

                    if (strcmp("jittered", samplingName.c_str()) == 0) {
                        std::string rest = subString.substr(end + 1);

                        int start = rest.find_first_not_of(" \t\r\n");
                        int end = rest.substr(start).find_first_of(" \t\r\n");
                        std::string stringS = rest.substr(start);
                        if (end >= 0) stringS = rest.substr(start, start + end);
                        int sampleSize = (int) std::stof(stringS);

                        sampling = Sampling(JITTERED, sampleSize);
                        cout << "Jittered Sampling turned on with : " << std::to_string(sampleSize * sampleSize) << " Samples" << endl;
                    }
               }
           }
        }
        myFile.close();
    } else cout << "Unable to open file";

    printf("found %d lights\n", lights.size());
    printf("found %d spheres\n", spheres.size());
  printf("Orthagonal Camera Basis:\n");
  right = cross(up, forward);
  
  //v1 = up, v2 = forward, v3 = right
  Dir3D u1 = forward;
  Dir3D u2 = up - dot(u1, up) / u1.magnitudeSqr() * u1;
  Dir3D u3 = right - dot(u1, right) / u1.magnitudeSqr() * u1 - dot(u2, right) / u2.magnitudeSqr() * u2;

  up = u2.normalized();
  forward = -1.f * u1.normalized();
  right = u3.normalized();

  forward.print("forward");
  right.print("right");
  up.print("up");
}

#endif