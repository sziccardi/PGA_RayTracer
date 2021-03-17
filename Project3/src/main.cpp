#include "main.h"

using std::cout;
using std::endl;

int main(int argc, char** argv) {

    //Read command line paramaters to get scene file
    if (argc != 2) {
        std::cout << "Usage: ./a.out scenefile\n";
        return(0);
    }
    std::string secenFileName = argv[1];

    //Parse Scene File
    parseSceneFile(secenFileName);

    //Render scene
    float imgW = img_width, imgH = img_height;
    float halfW = imgW / 2, halfH = imgH / 2;
    float d = halfH / tanf(halfAngleVFOV * (M_PI / 180.0f));

    Image outputImg = Image(img_width, img_height);
    auto t_start = std::chrono::high_resolution_clock::now();

    for (int i = 0 / 2; i < img_width; i++) {
        for (int j = 0 / 2; j < img_height; j++) {

            if (sampling.mType == JITTERED) {

                float red = 0, green = 0, blue = 0, alpha = 1;

                for (float m = (1.f / sampling.sampleSize) / 2; m < 1; m += 1.f / sampling.sampleSize) {
                    for (float n = (1.f / sampling.sampleSize) / 2; n < 1; n += 1.f / sampling.sampleSize) {
                        // m and n now refer to the center of each ray location we want
                        // Add some noise, then send the ray.
                        // cout << "m, n: " << m << ", " << n << endl;
                        float u = (halfW - (imgW) * ((i + m + randomPixelLocationNoise((1.f / sampling.sampleSize) / 2)) / imgW));
                        float v = (halfH - (imgH) * ((j + n + randomPixelLocationNoise((1.f / sampling.sampleSize) / 2)) / imgH));

                        // p : the point u,v in 3D camera coordinates
                        Point3D p = d * forward + u * right + v * up + eye;
                        //cout << p << endl;
                        // rayDir : ray starting at eye and going through the image plane at p
                        Dir3D rayDir = (p - eye);
                        // rayLine : line version of the ray
                        Line3D rayLine = vee(eye, rayDir).normalized();  //Normalizing here is optional
                        //TODO starting here is the loop over all objects?

                        Color color = evaluateRayTree(eye, rayLine, 0);
                        
                        red += color.r;
                        green += color.g;
                        blue += color.b;
                    }
                }
                 // We know where we want the ray to go, so now we just need to know how large each pixel is and split it up
    //          _________________
    //          |       |   *   |
    //          |   *   |       |
    //          _________________
    //          |       |    *  |
    //          |   *   |       |
    //          _________________
                int aveSum = sampling.sampleSize * sampling.sampleSize;
                outputImg.setPixel(i, j, Color(red / aveSum, green / aveSum, blue / aveSum));


            } else {
                //cast 1 ray per pixel in the image plane
                // (u,v) : coordinate on the image plane for the center of the pixel
                float u = (halfW - (imgW) * ((i + 0.5) / imgW));
                float v = (halfH - (imgH) * ((j + 0.5) / imgH));
                // p : the point u,v in 3D camera coordinates
                Point3D p = d * forward + u * right + v * up + eye;
                //cout << p << endl;
                // rayDir : ray starting at eye and going through the image plane at p
                Dir3D rayDir = (p - eye);
                // rayLine : line version of the ray
                Line3D rayLine = vee(eye, rayDir).normalized();  //Normalizing here is optional
                //TODO starting here is the loop over all objects?
                Color color = evaluateRayTree(eye, rayLine, 0);
                
                outputImg.setPixel(i, j, color);
            }
            

           
            
        }
    }
    auto t_end = std::chrono::high_resolution_clock::now();
    printf("Rendering took %.2f ms\n", std::chrono::duration<double, std::milli>(t_end - t_start).count());

    outputImg.write(imgName.c_str());
    return 0;
}

Color evaluateRayTree(Point3D start, Line3D rayLine, int depth) {
    if (depth > maxDepth) return Color();
    Hit myHit = findIntersection(start, rayLine);
    Color color;
    if (myHit.mIntersected) {
        color = getLighting(myHit, depth); //Color(std::abs(myHit.mNormal.x), std::abs(myHit.mNormal.y), std::abs(myHit.mNormal.z)); //TODO do color calculations
        //cout << color << endl;
    }
    else color = background;

    return color;
}

Hit lineSegmentSphereIntersect_fast(Point3D p1, Point3D p2, Point3D sphereCenter, float sphereRadius) {
    Line3D rayLine = vee(p1, p2);
    Dir3D dir = rayLine.dir();
    float a = dot(dir, dir);
    Dir3D toStart = (p1 - sphereCenter);
    float b = 2 * dot(dir, toStart);
    float c = dot(toStart, toStart) - sphereRadius * sphereRadius;
    float discr = b * b - 4 * a * c;
    if (discr < 0) {
        return Hit();
    }
    else {
        float t0 = (-b + sqrt(discr)) / (2 * a);
        float t1 = (-b - sqrt(discr)) / (2 * a);
        float tEnd = (p2 - p1).x / dir.x;
        // p = p0 + t * v
        if (t0 > 0 && t1 > 0 && t0 < tEnd && t1 < tEnd) {
            float val = std::min(t1, t0);
            Point3D p = p1 + val * dir;
            Dir3D norm = (p - sphereCenter).normalized();
            return Hit(p, p1, rayLine, norm);
        }
        else if (t0 > 0 && t1 < 0 && t0 < tEnd) {
            Point3D p = p1 + t0 * dir;
            Dir3D norm = (p - sphereCenter).normalized();
            return Hit(p, p1, rayLine, norm);
        }
        else if (t1 > 0 && t0 < 0 && t1 < tEnd) {
            Point3D p = p1 + t1 * dir;
            Dir3D norm = (p - sphereCenter).normalized();
            return Hit(p, p1, rayLine, norm);
        }

    }
    return Hit();
}

Hit raySphereIntersect_fast(Point3D rayStart, Line3D rayLine, Point3D sphereCenter, float sphereRadius) {
    Dir3D dir = rayLine.dir();
    float a = dot(dir, dir);
    Dir3D toStart = (rayStart - sphereCenter);
    float b = 2 * dot(dir, toStart);
    float c = dot(toStart, toStart) - sphereRadius * sphereRadius;
    float discr = b * b - 4 * a * c;
    if (discr < 0) {
        return Hit();
    }
    else {
        float t0 = (-b + sqrt(discr)) / (2 * a);
        float t1 = (-b - sqrt(discr)) / (2 * a);
        // p = p0 + t * v
        if (t0 > 0 && t1 > 0) {
            float val = std::min(t1, t0);
            Point3D p = rayStart + val * dir;
            Dir3D norm = (p - sphereCenter).normalized();
            return Hit(p, rayStart, rayLine, norm);
        }
        else if (t0 > 0 && t1 < 0) {
            Point3D p = rayStart + t0 * dir;
            Dir3D norm = (p - sphereCenter).normalized();
            return Hit(p, rayStart, rayLine, norm);
        }
        else if (t1 > 0 && t0 < 0) {
            Point3D p = rayStart + t1 * dir;
            Dir3D norm = (p - sphereCenter).normalized();
            return Hit(p, rayStart, rayLine, norm);
        }
        
    }
    return Hit();
}

Hit raySphereIntersect(Point3D rayStart, Line3D rayLine, Point3D sphereCenter, float sphereRadius) {
    Point3D projPoint = dot(rayLine, sphereCenter) * rayLine;      //Project to find closest point between circle center and line [proj(sphereCenter,rayLine);]
    float distSqr = projPoint.distToSqr(sphereCenter);          //Point-line distance (squared)
    float d2 = distSqr / (sphereRadius * sphereRadius);             //If distance is larger than radius, then...
    if (d2 > 1) return Hit();                                   //... the ray missed the sphere
    float w = sphereRadius * sqrt(1 - d2);                          //Pythagorean theorem to determine dist between proj point and intersection points
    Point3D p1 = projPoint - rayLine.dir() * w;                   //Add/subtract above distance to find hit points
    Point3D p2 = projPoint + rayLine.dir() * w;
    Dir3D norm1 = (p1 - sphereCenter);
    Dir3D norm2 = (p2 - sphereCenter);

    if (dot((p1 - rayStart), rayLine.dir()) >= 0 || dot((p2 - rayStart), rayLine.dir()) >= 0) { //are the points in same direction as the ray line?
        if ((rayStart - p1).magnitude() > (rayStart - p2).magnitude()) return Hit(p2, rayStart, rayLine, norm2);
        return Hit(p1, rayStart, rayLine, norm1);    
    }
    return Hit();
}

Hit findIntersection(Point3D rayStart, Line3D rayLine) {
    Hit closestHit = Hit();
    float currMinDist = 10000000000;
    int i = 0;
    for (Sphere s : spheres) {
        //TODO: add check for max depth
        Hit tempHit = raySphereIntersect_fast(rayStart, rayLine, s.mCenter, s.mRadius);
        if (tempHit.mIntersected) {
            float tempDist = (tempHit.mPosition - rayStart).magnitude();
            if (tempDist < currMinDist) {
                currMinDist = tempDist;
                tempHit.mObjectIter = i;
                tempHit.mMaterial = s.mMaterial;
                closestHit = tempHit;
            }
        }
        i++;
    }
    
    return closestHit;
}

Hit findIntersection(Point3D p1, Point3D p2) {
    Hit closestHit = Hit();
    float currMinDist = 10000000000;
    int i = 0;
    for (Sphere s : spheres) {
        //TODO: add check for max depth
        Hit tempHit = lineSegmentSphereIntersect_fast(p1, p2, s.mCenter, s.mRadius);
        if (tempHit.mIntersected) {
            float tempDist = (tempHit.mPosition - p1).magnitude();
            if (tempDist < currMinDist) {
                currMinDist = tempDist;
                tempHit.mObjectIter = i;
                tempHit.mMaterial = s.mMaterial;
                closestHit = tempHit;
            }
        }
        i++;
    }

    return closestHit;
}

Color getLighting(Hit intersection, int depth) {

    Color totalColor = Color(0, 0, 0);
    for (Light* l : lights) {
        //cout << "light type = " << std::to_string(l->mType) << endl;
        if (l->mType == AMBIENT) {
            Color myMult = l->mColor * intersection.mMaterial.mAmbientColor;
            totalColor = totalColor + myMult; 
        } else if (l->mType == SPOT) {
            SpotLight* sl = (SpotLight*)(l); 
            Hit lightIntersect = findIntersection(sl->mPosition, (intersection.mPosition + intersection.mNormal * 0.05f));
            if (!lightIntersect.mIntersected) {
                Dir3D lightToSphere = (intersection.mPosition) - sl->mPosition;
                float angleToLight = acos(dot(lightToSphere.normalized(), sl->mDirection.normalized())) * 180.0 / M_PI;
                float deflectionScaling = dot(intersection.mNormal, -1.f * sl->mDirection.normalized());
                if (angleToLight <= sl->mMinAngle) {
                    totalColor = totalColor + intersection.mMaterial.mDiffuseColor * sl->mColor * std::max(0.f, deflectionScaling);
                }
                else if (angleToLight > sl->mMinAngle && angleToLight < sl->mMaxAngle) {
                    float range = (sl->mMaxAngle - sl->mMinAngle);
                    float diff = angleToLight - sl->mMinAngle;
                    float intensity = 1 - diff / range;
                    totalColor = totalColor + intensity * intersection.mMaterial.mDiffuseColor * sl->mColor * std::max(0.f, deflectionScaling);
                }
            }
        }
        else if (l->mType == DIRECTIONAL) {
            DirectionalLight* dl = (DirectionalLight*)(l);
            Hit lightIntersect = findIntersection((intersection.mPosition + intersection.mNormal * 0.5f), vee(intersection.mPosition - dl->mDirection, intersection.mPosition + intersection.mNormal * 0.5f).normalized());
            if (!lightIntersect.mIntersected) {
                float deflectionScaling = dot(intersection.mNormal, dl->mDirection.normalized());
                totalColor = totalColor + intersection.mMaterial.mDiffuseColor * dl->mColor * std::max(0.f, deflectionScaling);

                Color ks = intersection.mMaterial.mSpecularColor;
                Dir3D v = (intersection.mRayStartPoint - intersection.mPosition).normalized();
                Dir3D h = (v + dl->mDirection).normalized();
                Dir3D n = intersection.mNormal;
                float p = intersection.mMaterial.mSpecularPower;
                Color I = dl->mColor;
                totalColor = totalColor + ks * I * std::pow(std::max(0.f, dot(n, h)), p);
            }
        }
        else if (l->mType == POINT) {
            PointLight* pl = (PointLight*)(l);
            Dir3D lightDir = pl->mPosition - (intersection.mPosition + intersection.mNormal * 0.0f);
            Hit lightIntersect = findIntersection(intersection.mPosition + intersection.mNormal * 0.05f, pl->mPosition);
            if (!lightIntersect.mIntersected) {
                float dist = (pl->mPosition).distToSqr(intersection.mPosition);
                float coefficient = 1.f / dist;
                float myDot = dot(intersection.mNormal, lightDir.normalized());
                totalColor = totalColor + coefficient * intersection.mMaterial.mDiffuseColor * pl->mColor * std::max(0.f, myDot);

                // specular amount = ks * I * max(0 , dot(n, h)) ^ p
                // ks: specular color
                // v: intersection to eye
                // l: intersection to light
                // h: (v + l).normalized()
                // n: surface normal
                // p: specular coefficient
                // I: light color
                Color ks = intersection.mMaterial.mSpecularColor;
                Dir3D v = (intersection.mRayStartPoint - intersection.mPosition).normalized();
                Dir3D h = (v + lightDir.normalized()).normalized();
                Dir3D n = intersection.mNormal;
                float p = intersection.mMaterial.mSpecularPower;
                Color I = pl->mColor;
                Color specToAdd = ks * I * std::pow(std::max(0.f, dot(n, h)), p);
                //if (specToAdd.r > 0 && specToAdd.g > 0 && specToAdd.b > 0 ) cout << specToAdd << endl;
                totalColor = totalColor + specToAdd;
                
            }
        }        
    }
    
    // Add reflection and refraction
    // pseudocode, delete as complete

    // Ray mirror = Reflect( ray, hit.normal ); DONE
    // contribution += Kr*EvaluateRayTree( scene, mirror )

    // Plane3D hitPlane = dot(intersection.mPosition, intersection.mNormal);
    // Line3D refl = sandwhich(hitPlane, intersection.mRay);
    // float ldotr = std::max(dot(intersection.mRay, refl), 0.0f);

    // Color reflectColor = evaluateRayTree(intersection.mPosition + 0.05f * intersection.mNormal, refl, depth + 1);
    // // TODO: ldotr is currently acting at Kr
    // totalColor = totalColor + ldotr * reflectColor;

    // Ray glass = Refract( ray, hit.normal );
    // contribution += Kt*EvaluateRayTree( scene, glass ); contribution += Ambient(); // superhack!
    // contribution += Emission( hit ); // for area light sources onl
    
    
    //cout << totalColor << endl;
    return totalColor.getTonemapped();
}


// https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c
float randomPixelLocationNoise(float noiseSize) {
    return (float)rand() / (float)(RAND_MAX / (2 * noiseSize)) - noiseSize;
}