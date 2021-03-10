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
            //cast 1 ray per pixel in the image plane
            // (u,v) : coordinate on the image plane for the center of the pixel
            float u = ((imgW) * ((i + 0.5) / imgW) - halfW);
            float v = (halfH - (imgH) * ((j + 0.5) / imgH));
            // p : the point u,v in 3D camera coordinates
            Point3D p = d * forward + u * right + v * up + eye;
            //cout << p << endl;
            // rayDir : ray starting at eye and going through the image plane at p
            Dir3D rayDir = (p - eye);
            // rayLine : line version of the ray
            Line3D rayLine = vee(eye, rayDir).normalized();  //Normalizing here is optional
            //TODO starting here is the loop over all objects?

            Hit myHit = findIntersection(eye, rayLine);
            Color color;
            if (myHit.mIntersected) {
                color = getLighting(myHit); //Color(std::abs(myHit.mNormal.x), std::abs(myHit.mNormal.y), std::abs(myHit.mNormal.z)); //TODO do color calculations
                //cout << color << endl;
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
        if (t0 > 0 || t1 > 0) {
            Point3D p1 = rayStart + dir * (maxDepth * 100);
            Point3D p2 = rayStart + dir * (maxDepth * 100);
            if (t0 > 0) {
                p1 = rayStart + t0 * dir;
            }
            if (t1 > 0) {
                p2 = rayStart + t1 * dir;
            }
            Dir3D norm1 = (p1 - sphereCenter).normalized();
            Dir3D norm2 = (p2 - sphereCenter).normalized();
            if (t0 > t1) return Hit(p2, norm2);
            return Hit(p1, norm1);
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

Color getLighting(Hit intersection) {
    Color totalColor = Color(0, 0, 0);
    for (Light* l : lights) {
        //cout << "light type = " << std::to_string(l->mType) << endl;
        if (l->mType == AMBIENT) {
            Color myMult = l->mColor * intersection.mMaterial.mAmbientColor;
            totalColor = totalColor + myMult;
        } else if (l->mType == SPOT) {
            SpotLight* sl = (SpotLight*)(l); 
            Hit lightIntersect = findIntersection((intersection.mPosition + intersection.mNormal * 0.05f), vee(sl->mPosition, (intersection.mPosition + intersection.mNormal * 0.05f)).normalized());
            if (lightIntersect.mIntersected) {
                Dir3D lightToSphere = (intersection.mPosition) - sl->mPosition;
                float angle = acos(dot(lightToSphere.normalized(), sl->mDirection.normalized())) * 180.0 / M_PI;
                float deflectionScaling = dot(intersection.mNormal, -1.f * sl->mDirection.normalized());
                //cout << "spotlight! angle " << std::to_string(angle) << endl;
                // if (lightToSphere.magnitude() < 5.0f) {
                        // cout << "very small: " << std::to_string(lightToSphere.magnitudeSqr()) << endl;

                    // }
                if (angle < (sl->mMinAngle / 2.f)) {
                    //cout << "like point!" << endl;
                    //cout << "dot: " << std::to_string(deflectionScaling) << endl;
                    
                    totalColor = totalColor + intersection.mMaterial.mDiffuseColor * sl->mColor * std::max(0.f, deflectionScaling);
                }
                else if (angle > (sl->mMinAngle / 2.f) && angle < (sl->mMaxAngle / 2.f)) {
                    float range = (sl->mMaxAngle / 2.f - sl->mMinAngle / 2.f);
                    float diff = sl->mMaxAngle / 2.f - abs(angle);

                    //cout << "linear fall off!" << endl;
                    if ((diff / range) > 1) {
                        cout << "dot: " << std::to_string((diff / range)) << endl;
                    }
                    totalColor = totalColor + 0.8f * (diff / range) * intersection.mMaterial.mDiffuseColor * sl->mColor * std::max(0.f, deflectionScaling);
                }
            }
        }
        else if (l->mType == DIRECTIONAL) {
            DirectionalLight* dl = (DirectionalLight*)(l);
            Hit lightIntersect = findIntersection((intersection.mPosition + intersection.mNormal * 0.5f), vee(intersection.mPosition, intersection.mPosition + dl->mDirection).normalized());
            if (!lightIntersect.mIntersected) {
                float deflectionScaling = dot(intersection.mNormal, dl->mDirection.normalized());
                totalColor = totalColor + intersection.mMaterial.mDiffuseColor * dl->mColor * std::max(0.f, deflectionScaling);
            }
        }
        else if (l->mType == POINT) {
            PointLight* pl = (PointLight*)(l);
            Dir3D lightDir = pl->mPosition - (intersection.mPosition + intersection.mNormal * 0.5f);
            Hit lightIntersect = findIntersection((intersection.mPosition + intersection.mNormal * 0.05f), vee(pl->mPosition, intersection.mPosition).normalized());
            if (!lightIntersect.mIntersected) {
                float coefficient = 1.f / lightDir.magnitudeSqr();
                float deflectionScaling = dot(intersection.mNormal, lightDir.normalized());
                totalColor = totalColor + coefficient * intersection.mMaterial.mDiffuseColor * pl->mColor * std::max(0.f, deflectionScaling);

                Dir3D v = (eye - intersection.mPosition).normalized();
                Dir3D h = (v + lightDir.normalized()).normalized();
                deflectionScaling = dot(intersection.mNormal, h);
                totalColor = totalColor + coefficient * intersection.mMaterial.mSpecularColor * pl->mColor * std::pow(std::max(0.f, deflectionScaling), intersection.mMaterial.mSpecularPower);
            }
        }
        
    }

    return totalColor.getTonemapped();
}