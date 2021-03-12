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

    Node* rootNode = new Node();
    Hit rootHit = Hit(eye, Dir3D(), Material());
    rootHit.mIntersected = false;
    rootNode->mValue = rootHit;
    mRayTree = new RayTree(rootNode);

    Image outputImg = Image(img_width, img_height);
    auto t_start = std::chrono::high_resolution_clock::now();
    for (int i = 0 / 2; i < img_width; i++) {
        for (int j = 0 / 2; j < img_height; j++) {
            //cast 1 ray per pixel in the image plane
            // (u,v) : coordinate on the image plane for the center of the pixel
            float u = (halfW - (imgW) * ((i + 0.5) / imgW));
            float v = (halfH - (imgH) * ((j + 0.5) / imgH));
            // p : the point u,v in 3D camera coordinates
            Point3D p = d * forward + u * right + v * up + eye;
            
            // rayDir : ray starting at eye and going through the image plane at p
            Dir3D rayDir = (p - eye);
            // rayLine : line version of the ray
            Line3D rayLine = vee(eye, rayDir).normalized();  //Normalizing here is optional

            Hit myHit = findIntersection(eye, rayLine);
            Color color;
            if (myHit.mIntersected) {
                //TODO: make this recursive
                //cout << "( " << std::to_string(i) << ", " << std::to_string(j) << " )" << endl;
                color = getLighting(myHit, 0);

            }
            else color = background;
            outputImg.setPixel(i, j, color);
        }
    }
    auto t_end = std::chrono::high_resolution_clock::now();
    printf("Rendering took %.2f ms\n", std::chrono::duration<double, std::milli>(t_end - t_start).count());

    outputImg.write(imgName.c_str());
    return 0;
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
            return Hit(p, norm);
        }
        else if (t0 > 0 && t1 < 0 && t0 < tEnd) {
            Point3D p = p1 + t0 * dir;
            Dir3D norm = (p - sphereCenter).normalized();
            return Hit(p, norm);
        }
        else if (t1 > 0 && t0 < 0 && t1 < tEnd) {
            Point3D p = p1 + t1 * dir;
            Dir3D norm = (p - sphereCenter).normalized();
            return Hit(p, norm);
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
            return Hit(p, norm);
        }
        else if (t0 > 0 && t1 < 0) {
            Point3D p = rayStart + t0 * dir;
            Dir3D norm = (p - sphereCenter).normalized();
            return Hit(p, norm);
        }
        else if (t1 > 0 && t0 < 0) {
            Point3D p = rayStart + t1 * dir;
            Dir3D norm = (p - sphereCenter).normalized();
            return Hit(p, norm);
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
        if ((rayStart - p1).magnitude() > (rayStart - p2).magnitude()) return Hit(p2, norm2);
        return Hit(p1, norm1);    
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

Color getLighting(Node* parentNode, int currentLevel) {    
    //cout << currentLevel << endl;
    Color totalColor = Color(0, 0, 0);
    for (Light* l : lights) {
        //cout << "light type = " << std::to_string(l->mType) << endl;
        if (l->mType == AMBIENT) {
            Color myMult = l->mColor * parentNode->mValue.mMaterial.mAmbientColor;
            totalColor = totalColor + myMult; 
        } else if (l->mType == SPOT) {
            SpotLight* sl = (SpotLight*)(l); 
            Hit lightIntersect = findIntersection(sl->mPosition, (parentNode->mValue.mPosition + parentNode->mValue.mNormal * 0.05f));
            if (!lightIntersect.mIntersected) {
                Dir3D lightToSphere = (parentNode->mValue.mPosition) - sl->mPosition;
                float angleToLight = acos(dot(lightToSphere.normalized(), sl->mDirection.normalized())) * 180.0 / M_PI;
                float deflectionScaling = dot(parentNode->mValue.mNormal, -1.f * sl->mDirection.normalized());
                if (angleToLight <= sl->mMinAngle) {
                    totalColor = totalColor + parentNode->mValue.mMaterial.mDiffuseColor * sl->mColor * std::max(0.f, deflectionScaling);
                }
                else if (angleToLight > sl->mMinAngle && angleToLight < sl->mMaxAngle) {
                    float range = (sl->mMaxAngle - sl->mMinAngle);
                    float diff = angleToLight - sl->mMinAngle;
                    float intensity = 1 - diff / range;
                    totalColor = totalColor + intensity * parentNode->mValue.mMaterial.mDiffuseColor * sl->mColor * std::max(0.f, deflectionScaling);
                }
            }
            else totalColor = Color(1, 0, 0);
        }
        else if (l->mType == DIRECTIONAL) {
            DirectionalLight* dl = (DirectionalLight*)(l);
            Hit lightIntersect = findIntersection((parentNode->mValue.mPosition + parentNode->mValue.mNormal * 0.5f + dl->mDirection * 100000.f), vee(parentNode->mValue.mPosition, parentNode->mValue.mPosition + dl->mDirection).normalized());
            if (!lightIntersect.mIntersected) {
                float deflectionScaling = dot(parentNode->mValue.mNormal, dl->mDirection.normalized());
                totalColor = totalColor + parentNode->mValue.mMaterial.mDiffuseColor * dl->mColor * std::max(0.f, deflectionScaling);

                Color ks = parentNode->mValue.mMaterial.mSpecularColor;
                Dir3D v = (eye - parentNode->mValue.mPosition).normalized();
                Dir3D h = (v + dl->mDirection).normalized();
                Dir3D n = parentNode->mValue.mNormal;
                float p = parentNode->mValue.mMaterial.mSpecularPower;
                Color I = dl->mColor;
                totalColor = totalColor + ks * I * std::pow(std::max(0.f, dot(n, h)), p);
            }
        }
        else if (l->mType == POINT) {
            PointLight* pl = (PointLight*)(l);
            Dir3D lightDir = pl->mPosition - (parentNode->mValue.mPosition + parentNode->mValue.mNormal * 0.0f);
            Hit lightIntersect = findIntersection(parentNode->mValue.mPosition + parentNode->mValue.mNormal * 0.05f, pl->mPosition);
            if (!lightIntersect.mIntersected) {
                float dist = (pl->mPosition).distToSqr(parentNode->mValue.mPosition);
                float coefficient = 1.f / dist;
                float myDot = dot(parentNode->mValue.mNormal, lightDir.normalized());
                totalColor = totalColor + coefficient * parentNode->mValue.mMaterial.mDiffuseColor * pl->mColor * std::max(0.f, myDot);

                // specular amount = ks * I * max(0 , dot(n, h)) ^ p
                // ks: specular color
                // v: intersection to eye
                // l: intersection to light
                // h: (v + l).normalized()
                // n: surface normal
                // p: specular coefficient
                // I: light color
                Color ks = parentNode->mValue.mMaterial.mSpecularColor;
                Dir3D v = (eye - parentNode->mValue.mPosition).normalized();
                Dir3D h = (v + lightDir.normalized()).normalized();
                Dir3D n = parentNode->mValue.mNormal;
                float p = parentNode->mValue.mMaterial.mSpecularPower;
                Color I = pl->mColor;
                totalColor = totalColor + coefficient * ks * I * std::pow(std::max(0.f, dot(n, h)), p);
            }
        }
        
    }
    if (mRecursiveDepth > 0 && currentLevel < mRecursiveDepth - 1) {
        Dir3D v = (parentNode->mValue.mPosition - eye);
        Dir3D rayDir = 2 * dot(parentNode->mValue.mNormal, v) * parentNode->mValue.mNormal - v;

        Line3D rayLine = vee(eye, rayDir).normalized();  //Normalizing here is optional

        Hit myHit = findIntersection(eye, rayLine);
        if (myHit.mIntersected) {
            int i = currentLevel;

            Node* reflected = new Node();
            reflected->mType = 1;
            reflected->mValue = myHit;
            parentNode->mColor = getLighting(reflected, i + 1);
            mRayTree->addChild(parentNode, reflected);
            
            //totalColor = totalColor + getLighting(myHit, i + 1);
        }
    }
    return totalColor.getTonemapped();
}