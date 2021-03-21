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
    float d = halfH / tanf(halfAngleVFOV * (pi / 180.0f));

    Image outputImg = Image(img_width, img_height);
    auto t_start = std::chrono::high_resolution_clock::now();
    //#pragma omp parallel for
    for (int i = 0 / 2; i < img_width; i++) {
        //#pragma omp parallel for
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

                        Color color = evaluateRayTree(eye, rayDir, 0);
                    
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


            } 
            else {
                //cast 1 ray per pixel in the image plane
                // (u,v) : coordinate on the image plane for the center of the pixel
                float u = (halfW - (imgW) * ((i + 0.5) / imgW));
                float v = (halfH - (imgH) * ((j + 0.5) / imgH));
                // p : the point u,v in 3D camera coordinates
                Point3D p = d * forward + u * right + v * up + eye;
                //cout << p << endl;
                // rayDir : ray starting at eye and going through the image plane at p
                Dir3D rayDir = (p - eye);
                Color color = evaluateRayTree(eye, rayDir, 0);
                
                outputImg.setPixel(i, j, color);
            }            
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
        if ((rayStart - p1).magnitude() >= (rayStart - p2).magnitude()) return Hit(p2, norm2);
        return Hit(p1, norm1);    
    }
    return Hit();
}

Hit rayTriIntersect(Point3D rayStart, Line3D rayLine, Triangle tri, Material m) {
    Plane3D triPlane = vee(verts[tri.mVert1].mPosition, verts[tri.mVert2].mPosition, verts[tri.mVert3].mPosition);
    HomogeneousPoint3D hitPoint = wedge(rayLine, triPlane);
    Point3D point = Point3D(hitPoint.x / hitPoint.w, hitPoint.y / hitPoint.w, hitPoint.z / hitPoint.w);
    
    if (dot(rayLine.dir(), (point - rayStart)) > 0 && !pointInTriangle(point, verts[tri.mVert1].mPosition, verts[tri.mVert2].mPosition, verts[tri.mVert3].mPosition)) return Hit(); //ray is parallel
    //cout << hitPoint << endl;
    if (tri.mUseBarycentric) {
        Coord3D bary = getBarycentricCoord(point, tri);
        int scale1 = 1;
        //if (dot(norms[tri.mNorm1], rayLine.dir()) > 0) scale1 = -1;
        int scale2 = 1;
        //if (dot(norms[tri.mNorm2], rayLine.dir()) > 0) scale2 = -1;
        int scale3 = 1;
        //if (dot(norms[tri.mNorm3], rayLine.dir()) > 0) scale3 = -1;
        Dir3D myNorm = scale1 * bary.mU * norms[tri.mNorm1] + scale2 * bary.mV * norms[tri.mNorm2] + scale3 * bary.mW * norms[tri.mNorm3];
        return Hit(point, myNorm.normalized(), m);
    }
    int scale = 1;
    if (dot(tri.mNormal, rayLine.dir()) > dot(-1 * tri.mNormal, rayLine.dir())) scale = -1;
    return Hit(point, scale * tri.mNormal, m); //intersect with flat triangle
}

Hit rayTriIntersect(Point3D rayStart, Point3D rayEnd, Triangle tri, Material m) {
    Line3D rayLine = vee(rayStart, rayEnd);
    Plane3D triPlane = vee(verts[tri.mVert1].mPosition, verts[tri.mVert2].mPosition, verts[tri.mVert3].mPosition);

    //check if the start and end are on opposite sides of the tri plane
    bool sign1 = (dot(rayStart - Point3D(0, 0, 0), tri.mNormal) < 0.0f);
    bool sign2 = (dot(rayEnd - Point3D(0, 0, 0), tri.mNormal) < 0.0f);
    if ((sign1 == true && sign2 == true) || (sign1 == false && sign2 == false)) return Hit();

    HomogeneousPoint3D hitPoint = wedge(rayLine, triPlane);
    Point3D point = Point3D(hitPoint.x / hitPoint.w, hitPoint.y / hitPoint.w, hitPoint.z / hitPoint.w);

    if (dot(rayLine.dir(), (point - rayStart)) > 0 && !pointInTriangle(point, verts[tri.mVert1].mPosition, verts[tri.mVert2].mPosition, verts[tri.mVert3].mPosition)) return Hit(); //ray is parallel
    if (tri.mUseBarycentric) {
        Coord3D bary = getBarycentricCoord(point, tri);
        int scale1 = 1;
        if (dot(norms[tri.mNorm1], (rayStart - rayEnd)) > 0.05) scale1 = -1;
        int scale2 = 1;
        if (dot(norms[tri.mNorm2], (rayStart - rayEnd)) > 0.05) scale2 = -1;
        int scale3 = 1;
        if (dot(norms[tri.mNorm3], (rayStart - rayEnd)) > 0.05) scale3 = -1;
        Dir3D myNorm = scale1 * bary.mU * norms[tri.mNorm1] + scale2 * bary.mV * norms[tri.mNorm2] + scale3 * bary.mW * norms[tri.mNorm3];
        return Hit(point, myNorm, m);
    }
    int scale = 1;
    if (dot(tri.mNormal, (rayStart - rayEnd)) > 0.05) scale = -1;
    return Hit(point, scale * tri.mNormal, m); //intersect with flat triangle
}

Hit findIntersection(Point3D rayStart, Line3D rayLine) {
    Hit closestHit = Hit();
    float currMinDist = 10000000000000;
    
    for (Object* o : objects) {

        Point3D p1 = o->mCenter + Dir3D(-o->mBoundingWidth / 2, o->mBoundingHeight / 2, -o->mBoundingDepth / 2);
        Point3D p2 = o->mCenter + Dir3D(o->mBoundingWidth / 2, o->mBoundingHeight / 2, -o->mBoundingDepth / 2);
        Point3D p3 = o->mCenter + Dir3D(o->mBoundingWidth / 2, o->mBoundingHeight / 2, o->mBoundingDepth / 2);
        Point3D p4 = o->mCenter + Dir3D(-o->mBoundingWidth / 2, o->mBoundingHeight / 2, o->mBoundingDepth / 2);
        Point3D p5 = o->mCenter + Dir3D(-o->mBoundingWidth / 2, -o->mBoundingHeight / 2, -o->mBoundingDepth / 2);
        Point3D p6 = o->mCenter + Dir3D(o->mBoundingWidth / 2, -o->mBoundingHeight / 2, -o->mBoundingDepth / 2);
        Point3D p7 = o->mCenter + Dir3D(o->mBoundingWidth / 2, -o->mBoundingHeight / 2, o->mBoundingDepth / 2);
        Point3D p8 = o->mCenter + Dir3D(-o->mBoundingWidth / 2, -o->mBoundingHeight / 2, o->mBoundingDepth / 2);
        /*cout << "center: " << o->mCenter << endl;
        cout << "p1: " << p1 << endl;
        cout << "p2: " << p2 << endl;
        cout << "p3: " << p3 << endl;
        cout << "p4: " << p4 << endl;
        cout << "p5: " << p5 << endl;
        cout << "p6: " << p6 << endl;
        cout << "p7: " << p7 << endl;
        cout << "p8: " << p8 << endl;*/

        Plane3D leftPlane = vee(p1, p4, p5);
        Plane3D rightPlane = vee(p2, p3, p7);
        Plane3D bottomPlane = vee(p5, p6, p7);
        Plane3D topPlane = vee(p1, p2, p3);
        Plane3D backPlane = vee(p4, p3, p7);
        Plane3D frontPlane = vee(p1, p2, p6);

        Point3D leftIntersect = Point3D(wedge(rayLine, leftPlane));
        Point3D rightIntersect = Point3D(wedge(rayLine, rightPlane));
        Point3D bottomIntersect = Point3D(wedge(rayLine, bottomPlane));
        Point3D topIntersect = Point3D(wedge(rayLine, topPlane));
        Point3D backIntersect = Point3D(wedge(rayLine, backPlane));
        Point3D frontIntersect = Point3D(wedge(rayLine, frontPlane));

        //if (pointInQuad(frontIntersect, p1, p2, p6, p5)) cout << "front: true" << endl;
        //else cout << "front: false" << endl;
        //if (pointInQuad(backIntersect, p4, p3, p7, p8)) cout << "back: true" << endl;
        //else cout << "back: false" << endl;
        //if (pointInQuad(topIntersect, p1, p2, p3, p4)) cout << "top: true" << endl;
        //else cout << "top: false" << endl;
        //if (pointInQuad(bottomIntersect, p5, p6, p7, p8)) cout << "bottom: true" << endl;
        //else cout << "bottom: false" << endl;
        //if (pointInQuad(rightIntersect, p2, p3, p7, p6)) cout << "right: true" << endl;
        //else cout << "right: false" << endl;
        //if (pointInQuad(leftIntersect, p1, p4, p8, p5)) cout << "left: true" << endl;
        //else cout << "left: false" << endl;

        if (pointInQuad(frontIntersect, p1, p2, p6, p5) ||
            pointInQuad(backIntersect, p4, p3, p7, p8) ||
            pointInQuad(topIntersect, p1, p2, p3, p4) ||
            pointInQuad(bottomIntersect, p5, p6, p7, p8) ||
            pointInQuad(rightIntersect, p2, p3, p7, p6) ||
            pointInQuad(leftIntersect, p1, p4, p8, p5))
        {
            if (o->mType == TRIMESH) {
                TriMesh* tm = (TriMesh*)o;
                for (Triangle tri : tm->mTriangles) {
                    if (tri.mNormal == 0.f && tri.mNormal.y == 0.f && tri.mNormal.z == 0.f) {
                        Point3D avgPos = (verts[tri.mVert1].mPosition + verts[tri.mVert2].mPosition + verts[tri.mVert3].mPosition) / 3.f;

                        Dir3D edge1 = verts[tri.mVert2].mPosition - verts[tri.mVert1].mPosition;
                        Dir3D edge2 = verts[tri.mVert3].mPosition - verts[tri.mVert1].mPosition;
                        Dir3D norm = cross(edge1, edge2).normalized();

                        if (dot(rayLine.dir(), norm) > 0) norm = norm * -1.f;

                        tri.mCenterPoint = avgPos;
                        tri.mNormal = norm;
                    }

                    Hit tempHit = rayTriIntersect(rayStart, rayLine, tri, o->mMaterial);
                    if (tempHit.mIntersected) {
                        if (dot(tempHit.mPosition - rayStart, rayLine.dir()) >= 0) {
                            float tempDist = (tempHit.mPosition - rayStart).magnitude();
                            if (tempDist < currMinDist) {
                                currMinDist = tempDist;
                                tempHit.mMaterial = o->mMaterial;
                                closestHit = tempHit;
                                //cout << "hit at " << tempHit.mPosition << endl;
                            }
                        }
                    }
                }
            }
            else if (o->mType == SPHERE) {
                Sphere* s = (Sphere*)o;
                Hit tempHit = raySphereIntersect_fast(rayStart, rayLine, s->mCenter, s->mRadius);
                if (tempHit.mIntersected) {
                    float tempDist = (tempHit.mPosition - rayStart).magnitude();
                    if (tempDist < currMinDist) {
                        currMinDist = tempDist;
                        tempHit.mMaterial = o->mMaterial;
                        closestHit = tempHit;
                    }
                }
            }
        }

        
    }
    
    return closestHit;
}

Hit findIntersection(Point3D p1, Point3D p2) {
    Hit closestHit = Hit();
    float currMinDist = 10000000000;
    for (Object* o : objects) {

        Plane3D leftPlane = vee(o->mCenter - Dir3D(o->mBoundingWidth / 2, 0, 0), o->mCenter - Dir3D(o->mBoundingWidth / 2, 1, 0), o->mCenter - Dir3D(o->mBoundingWidth / 2, 1, 1));
        Plane3D rightPlane = vee(o->mCenter + Dir3D(o->mBoundingWidth / 2, 0, 0), o->mCenter + Dir3D(o->mBoundingWidth / 2, 1, 0), o->mCenter + Dir3D(o->mBoundingWidth / 2, 1, 1));
        Plane3D bottomPlane = vee(o->mCenter - Dir3D(0, o->mBoundingHeight / 2, 0), o->mCenter - Dir3D(1, o->mBoundingHeight / 2, 0), o->mCenter - Dir3D(1, o->mBoundingHeight / 2, 1));
        Plane3D topPlane = vee(o->mCenter + Dir3D(0, o->mBoundingHeight / 2, 0), o->mCenter + Dir3D(1, o->mBoundingHeight / 2, 0), o->mCenter + Dir3D(1, o->mBoundingHeight / 2, 1));
        Plane3D backPlane = vee(o->mCenter - Dir3D(0, 0, o->mBoundingDepth / 2), o->mCenter - Dir3D(1, 0, o->mBoundingDepth / 2), o->mCenter - Dir3D(1, 1, o->mBoundingDepth / 2));
        Plane3D frontPlane = vee(o->mCenter + Dir3D(0, 0, o->mBoundingDepth / 2), o->mCenter + Dir3D(1, 0, o->mBoundingDepth / 2), o->mCenter + Dir3D(1, 1, o->mBoundingDepth / 2));

        Point3D leftIntersect = Point3D(wedge(p2 - p1, leftPlane));
        Point3D rightIntersect = Point3D(wedge(p2 - p1, rightPlane));
        Point3D bottomIntersect = Point3D(wedge(p2 - p1, bottomPlane));
        Point3D topIntersect = Point3D(wedge(p2 - p1, topPlane));
        Point3D backIntersect = Point3D(wedge(p2 - p1, backPlane));
        Point3D frontIntersect = Point3D(wedge(p2 - p1, frontPlane));

        Point3D p1 = o->mCenter + Dir3D(-o->mBoundingWidth / 2, o->mBoundingHeight / 2, -o->mBoundingDepth / 2);
        Point3D p2 = o->mCenter + Dir3D(o->mBoundingWidth / 2, o->mBoundingHeight / 2, -o->mBoundingDepth / 2);
        Point3D p3 = o->mCenter + Dir3D(o->mBoundingWidth / 2, o->mBoundingHeight / 2, o->mBoundingDepth / 2);
        Point3D p4 = o->mCenter + Dir3D(-o->mBoundingWidth / 2, o->mBoundingHeight / 2, o->mBoundingDepth / 2);
        Point3D p5 = o->mCenter + Dir3D(-o->mBoundingWidth / 2, -o->mBoundingHeight / 2, -o->mBoundingDepth / 2);
        Point3D p6 = o->mCenter + Dir3D(o->mBoundingWidth / 2, -o->mBoundingHeight / 2, -o->mBoundingDepth / 2);
        Point3D p7 = o->mCenter + Dir3D(o->mBoundingWidth / 2, -o->mBoundingHeight / 2, o->mBoundingDepth / 2);
        Point3D p8 = o->mCenter + Dir3D(-o->mBoundingWidth / 2, -o->mBoundingHeight / 2, o->mBoundingDepth / 2);

        if (pointInQuad(frontIntersect, p1, p2, p6, p5) &&
            pointInQuad(backIntersect, p4, p3, p7, p8) &&
            pointInQuad(topIntersect, p1, p2, p3, p4) &&
            pointInQuad(bottomIntersect, p5, p6, p7, p8) &&
            pointInQuad(rightIntersect, p2, p3, p7, p6) &&
            pointInQuad(leftIntersect, p1, p4, p8, p5))
        {

            if (o->mType == TRIMESH) {
                TriMesh* tm = (TriMesh*)o;
                for (Triangle tri : tm->mTriangles) {
                    if (tri.mNormal == 0.f && tri.mNormal.y == 0.f && tri.mNormal.z == 0.f) {
                        Point3D avgPos = (verts[tri.mVert1].mPosition + verts[tri.mVert2].mPosition + verts[tri.mVert3].mPosition) / 3.f;

                        Dir3D edge1 = verts[tri.mVert2].mPosition - verts[tri.mVert1].mPosition;
                        Dir3D edge2 = verts[tri.mVert3].mPosition - verts[tri.mVert1].mPosition;
                        Dir3D norm = cross(edge1, edge2).normalized();

                        if (dot((p2 - p1), norm) > 0) norm = norm * -1.f;

                        tri.mCenterPoint = avgPos;
                        tri.mNormal = norm;
                    }

                    Hit tempHit = rayTriIntersect(p2, p1, tri, o->mMaterial);
                    if (tempHit.mIntersected) {
                        float tempDist = tempHit.mPosition.distTo(p1);
                        if (tempDist < p1.distTo(p2)) {
                            if (tempDist < currMinDist) {
                                currMinDist = tempDist;
                                tempHit.mMaterial = o->mMaterial;
                                closestHit = tempHit;
                            }
                        }
                    }
                }
            }
            else if (o->mType == SPHERE) {
                Sphere* s = (Sphere*)o;
                Hit tempHit = lineSegmentSphereIntersect_fast(p1, p2, s->mCenter, s->mRadius);
                if (tempHit.mIntersected) {
                    float tempDist = tempHit.mPosition.distTo(p1);
                    if (tempDist <= p2.distTo(p1)) {
                        if (tempDist < currMinDist) {
                            currMinDist = tempDist;
                            tempHit.mMaterial = o->mMaterial;
                            closestHit = tempHit;
                        }
                    }
                }

            }
        }
    }
    //cout << closestHit.mPosition.distTo(p2) << endl;
    return closestHit;
}

Dir3D getReflectRay(Hit hit, Dir3D ray) {
    return 2 * dot(hit.mNormal, -1 * ray) * hit.mNormal + ray;
}

Dir3D getRefrectRay(Dir3D normal, Dir3D ray, float n) {
    float rayNormalDot = dot(ray, normal);
    Dir3D refractDir = n * (ray - normal * rayNormalDot);
    float val = 1 - (n * n * (1 - rayNormalDot * rayNormalDot));
    if (val > 0) {
        //not total internal reflection
        return refractDir - normal * sqrt(val);
    }
    return Dir3D();
}

Color getLighting(Hit hit, Point3D rayStart, Dir3D ray, int currentDepth) {
    Color totalColor = Color(0, 0, 0);
    for (Light* l : lights) {
        //cout << "light type = " << std::to_string(l->mType) << endl;
        if (l->mType == AMBIENT) {
            Color myMult = l->mColor * hit.mMaterial.mAmbientColor;
            totalColor = totalColor + myMult; 
        } else if (l->mType == SPOT) {
            SpotLight* sl = (SpotLight*)(l); 
            Hit lightIntersect = findIntersection((hit.mPosition + hit.mNormal * 0.005f), sl->mPosition);
            if (!lightIntersect.mIntersected) {
                Dir3D lightToSphere = (hit.mPosition) - sl->mPosition;
                float angleToLight = acos(dot(lightToSphere.normalized(), sl->mDirection.normalized())) * 180.0 / pi;
                float deflectionScaling = dot(hit.mNormal, -1.f * sl->mDirection.normalized());
                float dist = (sl->mPosition).distToSqr(hit.mPosition);
                float coefficient = 1.f / dist;
                if (angleToLight <= sl->mMinAngle) {
                    totalColor = totalColor + coefficient * hit.mMaterial.mDiffuseColor * sl->mColor * std::max(0.f, deflectionScaling);
                    Color ks = hit.mMaterial.mSpecularColor;
                    Dir3D v = (eye - hit.mPosition).normalized();
                    Dir3D h = (v + sl->mDirection.normalized()).normalized();
                    Dir3D r = 2 * dot(hit.mNormal, sl->mDirection.normalized()) * hit.mNormal - sl->mDirection.normalized();
                    Dir3D n = hit.mNormal;
                    float p = hit.mMaterial.mSpecularPower;
                    Color I = sl->mColor;
                    totalColor = totalColor + coefficient * ks * I * std::pow(std::max(0.f, dot(n, h)), p);
                }
                else if (angleToLight > sl->mMinAngle && angleToLight < sl->mMaxAngle) {
                    float range = (sl->mMaxAngle - sl->mMinAngle);
                    float diff = angleToLight - sl->mMinAngle;
                    float intensity = 1 - diff / range;
                    totalColor = totalColor + coefficient * intensity * hit.mMaterial.mDiffuseColor * sl->mColor * std::max(0.f, deflectionScaling);
                }
            }
        }
        else if (l->mType == DIRECTIONAL) {
            DirectionalLight* dl = (DirectionalLight*)(l);
            Hit lightIntersect = findIntersection((hit.mPosition + hit.mNormal * 0.5f), vee(hit.mPosition - dl->mDirection, hit.mPosition + hit.mNormal * 0.5f).normalized());
            if (!lightIntersect.mIntersected) {
                float deflectionScaling = dot(hit.mNormal, dl->mDirection.normalized());
                totalColor = totalColor + hit.mMaterial.mDiffuseColor * dl->mColor * std::max(0.f, deflectionScaling);

                Color ks = hit.mMaterial.mSpecularColor;
                Dir3D v = (eye - hit.mPosition).normalized();
                Dir3D h = (v + dl->mDirection).normalized();
                Dir3D r = 2 * dot(hit.mNormal, dl->mDirection.normalized()) * hit.mNormal - dl->mDirection.normalized();
                Dir3D n = hit.mNormal;
                float p = hit.mMaterial.mSpecularPower;
                Color I = dl->mColor;
                totalColor = totalColor + ks * I * std::pow(std::max(0.f, dot(n, r)), p);
            }
        }
        else if (l->mType == POINT) {
            PointLight* pl = (PointLight*)(l);
            Dir3D lightDir = pl->mPosition - (hit.mPosition);
            Hit lightIntersect = findIntersection(pl->mPosition, hit.mPosition + hit.mNormal*0.005);
            if (!lightIntersect.mIntersected) {
                float dist = (pl->mPosition).distToSqr(hit.mPosition);
                float coefficient = 1.f / dist;
                float myDot = dot(hit.mNormal, lightDir.normalized());
                totalColor = totalColor + coefficient * hit.mMaterial.mDiffuseColor * pl->mColor * std::max(0.f, myDot);

                // specular amount = ks * I * max(0 , dot(n, h)) ^ p
                // ks: specular color
                // v: intersection to eye
                // l: intersection to light
                // h: (v + l).normalized()
                // n: surface normal
                // p: specular coefficient
                // I: light color
                Color ks = hit.mMaterial.mSpecularColor;
                Dir3D v = (eye - hit.mPosition).normalized();
                Dir3D h = (v + lightDir.normalized()).normalized();
                Dir3D r = 2 * dot(hit.mNormal, lightDir.normalized()) * hit.mNormal - lightDir.normalized();
                Dir3D n = hit.mNormal;
                float p = hit.mMaterial.mSpecularPower;
                Color I = pl->mColor;
                totalColor = totalColor + coefficient * ks * I * std::pow(std::max(0.f, dot(n, h)), p);
                
            }
           
        }
        
    }

    if (currentDepth < maxDepth) {
        //REFLECT 
        Dir3D reflectDir = getReflectRay(hit, ray);
        totalColor = totalColor + hit.mMaterial.mSpecularColor * evaluateRayTree(hit.mPosition + 0.005f * reflectDir, reflectDir, currentDepth);


        //REFRACT
        if (hit.mMaterial.mTransmissiveColor.getIntensity() > 0) {
            float kr = 1, kg = 1, kb = 1;
            Color refractedColor = Color(0, 0, 0);

            if (dot(hit.mNormal, ray) < 0) {
                //into object
                Dir3D refractDir = getRefrectRay(hit.mNormal, ray, hit.mMaterial.mRefractionIndex);
                if (refractDir.magnitudeSqr() > 0.05) {
                    refractedColor = evaluateRayTree(hit.mPosition - 0.05f * hit.mNormal, refractDir, currentDepth);
                }
            }
            else {
                //out of object
                Dir3D refractDir = getRefrectRay(-1.f * hit.mNormal, ray, 1 / hit.mMaterial.mRefractionIndex);
                if (refractDir.magnitudeSqr() > 0.05) {
                    float t = rayStart.distTo(hit.mPosition);
                    //beers law values
                    kr = std::exp(-hit.mMaterial.mAmbientColor.r * t);
                    kg = std::exp(-hit.mMaterial.mAmbientColor.g * t);
                    kb = std::exp(-hit.mMaterial.mAmbientColor.b * t);
                    refractedColor = evaluateRayTree(hit.mPosition + 0.05f * hit.mNormal, refractDir, currentDepth);
                }
            }

            totalColor = totalColor + Color(kr, kg, kb) * hit.mMaterial.mTransmissiveColor * refractedColor;
        }
    }
    
    return totalColor.getTonemapped();
}

Color evaluateRayTree(Point3D rayStart, Dir3D ray, int currentDepth) {
    Line3D rayLine = vee(rayStart, ray).normalized();
    Hit myHit = findIntersection(rayStart, rayLine);
    if (myHit.mIntersected) {
        return getLighting(myHit, rayStart, ray, currentDepth+1); 
    }
    else {
        return background;
    }
}


// https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c
float randomPixelLocationNoise(float noiseSize) {
    return (float)rand() / (float)(RAND_MAX / (2 * noiseSize)) - noiseSize;
}

Coord3D getBarycentricCoord(Point3D p, Triangle tri) {
    Point3D a = verts[tri.mVert1].mPosition;
    Point3D b = verts[tri.mVert2].mPosition;
    Point3D c = verts[tri.mVert3].mPosition;

    float totalArea = triangleArea(a, b, c);
    float area1 = triangleArea(b, c, p); //area pbc
    float area2 = triangleArea(c, a, p); //area pca
    float area3 = triangleArea(a, b, p); //area pab
    //cout << "bary = (" << area1 / totalArea << ", " << area2 / totalArea << ", " << area3 / totalArea << ")" << endl;
    return Coord3D(area1 / totalArea, area2 / totalArea, area3 / totalArea);
}