
#ifndef REFERENCE_CLASSES_H
#define REFERENCE_CLASSES_H

#include <cmath>
#include <string>
#include <iostream>
#include <fstream>

enum LightType { AMBIENT, POINT, DIRECTIONAL, SPOT };
enum SamplingType { NONE, JITTERED };

// ---------------------------------
//  Define Primatives
// ---------------------------------
struct Material {
	Color mAmbientColor, mDiffuseColor, mSpecularColor, mTransmissiveColor;
	float mSpecularPower, mRefractionIndex;
	Material(Color ambient, Color diffuse, Color specular, float specPow, Color transmissive, float refrectIndex) {
		mAmbientColor = ambient;
		mDiffuseColor = diffuse;
		mSpecularColor = specular;
		mTransmissiveColor = transmissive;
		mSpecularPower = specPow;
		mRefractionIndex = refrectIndex;
	}

	Material(float ar, float ag, float ab, 
		float dr, float dg, float db, 
		float sr, float sg, float sb, float ns, 
		float tr, float tg, float tb, float ior) {
		mAmbientColor = Color(ar, ag, ab);
		mDiffuseColor = Color(dr, dg, db);
		mSpecularColor = Color(sr, sg, sb);
		mTransmissiveColor = Color(tr, tg, tb);
		mSpecularPower = ns;
		mRefractionIndex = ior;
	}

	Material() {
		mAmbientColor = Color(0,0,0);
		mDiffuseColor = Color(1,1,1);
		mSpecularColor = Color(0,0,0);
		mTransmissiveColor = Color(0,0,0);
		mSpecularPower = 5;
		mRefractionIndex = 1;
	}
};

struct Object {
	Material mMaterial;
	Point3D mCenter;
	Object(Material m, Point3D position) {
		mMaterial = m;
		mCenter = position;
	}
};

struct Sphere : Object {
	float mRadius;
	Sphere(Material m, Point3D position, float radius) : Object (m, position) {
		mRadius = radius;
	
	}
};

struct Triangle {
	Point3D mP1, mP2, mP3;
	Material mMaterial;
	Plane3D plane;

	Triangle(Material m, Point3D p1, Point3D p2, Point3D p3) {
		mMaterial = m;
		mP1 = p1;
		mP2 = p2;
		mP3 = p3;
		plane = vee(p1,p2,p3);
	}

	Plane3D getPlane() {
		return plane;
	}

	bool pointInside(Point3D p) {
		bool dir1 = (dot(vee(mP1,mP2,p),plane) > 0);
		bool dir2 = (dot(vee(mP2,mP3,p),plane) > 0);
		bool dir3 = (dot(vee(mP3,mP1,p),plane) > 0);
		
		return (dir1 + dir2 + dir3) == 3 || (dir1 + dir2 + dir3) == 0;
	}

	Dir3D getNormal(Point3D hitPoint, Dir3D ray) {
		Dir3D norm1 = cross((mP2 - mP1), (mP3 - mP1));
        Dir3D normal = (dot(norm1, ray) < 0) ? norm1 : norm1 * -1;

		return normal.normalized();
	}
};

struct NormalTriangle : Triangle {

	Dir3D normal1, normal2, normal3;

	NormalTriangle(Material m, Point3D p1, Point3D p2, Point3D p3, Dir3D norm1, Dir3D norm2, Dir3D norm3) : Triangle(m, p1, p2, p3) {
		normal1 = norm1;
		normal2 = norm2;
		normal3 = norm3;
	}

	// TODO: Add barycentric coord stuff for this. To interpolate between the normals and stuff
	Dir3D getNormal(Point3D hitPoint, Dir3D ray) {
		// Dir3D norm1 = cross((mP2 - mP1), (mP3 - mP1));
        // Dir3D normal = (dot(norm1, ray) < 0) ? norm1 : norm1 * -1;

		Point3D sumPoint = mP1 + mP2 + mP3;
		Point3D percentOfEach = hitPoint / sumPoint;
		sumPoint.print();
		percentOfEach.print();

		return (normal1 * percentOfEach.x + normal2 * percentOfEach.y + normal3 * percentOfEach.z).normalized(); 

		// return normal.normalized();
	}
};

struct Light { //basically ambient
	LightType mType = AMBIENT;
	Color mColor;
	Light(Color lightColor) {
		mColor = lightColor;
	}
};

struct PointLight : Light {
	Point3D mPosition;
	PointLight(Color lightColor, Point3D position) : Light(lightColor) {
		mPosition = position;
		mType = POINT;
	}
};

struct SpotLight : Light {
	Point3D mPosition;
	Dir3D mDirection;
	float mMinAngle, mMaxAngle;
	SpotLight(Color lightColor, Point3D position, Dir3D direction, float minAngle, float maxAngle) : Light(lightColor) {
		mDirection = direction;
		mPosition = position;

		mMinAngle = minAngle;
		mMaxAngle = maxAngle;

		mType = SPOT;
	}
};

struct DirectionalLight : Light {
	Dir3D mDirection;
	DirectionalLight(Color lightColor, Dir3D direction) : Light(lightColor) {
		mDirection = direction;
		mType = DIRECTIONAL;
	}
};

struct Hit {
	bool mIntersected = false;
	Point3D mPosition = Point3D(-1, -1, -1);
	Point3D mRayStartPoint = Point3D(0, 0, 0);
	Line3D mRay = Line3D(-1, -1, -1);
	Dir3D mNormal = Dir3D(-1, -1, -1);
	Material mMaterial = Material();
	int mObjectIter = -1;
	Hit(Point3D position, Point3D startPosition, Line3D ray, Dir3D normal, Material material = Material(), int objectIter = -1) {
		mPosition = position;
		mObjectIter = objectIter;
		mNormal = normal;
		mMaterial = material;
		mIntersected = true;
		mRayStartPoint = startPosition;
		mRay = ray;
	}
	Hit() {};
};
//Various Operator Overloads


struct Sampling {
	SamplingType mType;
	int sampleSize;

	Sampling(SamplingType type = NONE, int samplingSize = 2) {
		mType = type;
		sampleSize = samplingSize;
	};
};


#endif