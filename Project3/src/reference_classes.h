
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

	bool operator==(Material other) {
		if (
			mAmbientColor == other.mAmbientColor &&
			mDiffuseColor == other.mDiffuseColor &&
			mSpecularColor == other.mSpecularColor &&
			mTransmissiveColor == other.mTransmissiveColor &&
			mSpecularPower == other.mSpecularPower &&
			mRefractionIndex == other.mRefractionIndex
			) return true;
		return false;
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
	Dir3D mNormal = Dir3D(-1, -1, -1);
	Material mMaterial = Material();
	int mObjectIter = -1;
	Hit(Point3D position, Dir3D normal, Material material = Material(), int objectIter = -1) {
		mPosition = position;
		mObjectIter = objectIter;
		mNormal = normal;
		mMaterial = material;
		mIntersected = true;
	}
	Hit() {};

	bool operator==(Hit toCompare) {
		if (mIntersected == toCompare.mIntersected &&
			mNormal == toCompare.mNormal &&
			mMaterial == toCompare.mMaterial &&
			mObjectIter == toCompare.mObjectIter
			) return true;
		return false;
	}
};
//Various Operator Overloads

struct Node {
	int mIndex;
	int mType; // 0: transmissive 1: reflective -1: none
	Hit mValue;
	Color mColor;
	Node* mTransmission;
	Node* mReflection;
};
//
//struct RayTree
//{
//	int mNumNodes;
//	RayTree() {
//		mRoot = nullptr;
//		mNumNodes = 0;
//	}
//
//	RayTree(Node* root) {
//		root->mIndex = 0;
//		mRoot = root;
//		mNumNodes = 1;
//	}
//
//	RayTree(Hit rootVal) {
//		mRoot = new Node();
//		mRoot->mIndex = 0;
//		mRoot->mValue = rootVal;
//		mNumNodes = 1;
//	}
//
//	~RayTree() {
//		destroy_tree(mRoot);
//	}
//
//	Node* searchUtil(Hit toFind, Node* newRoot, bool visited[])
//	{
//		if (newRoot != NULL)
//		{
//			if (toFind == newRoot->mValue)
//				return newRoot;
//			else {
//				if (!visited[newRoot->mReflection->mIndex]) {
//					visited[newRoot->mIndex] = true;
//					searchUtil(toFind, newRoot->mReflection, visited);
//				}
//				else if (!visited[newRoot->mTransmission->mIndex]) {
//					visited[newRoot->mIndex] = true;
//					searchUtil(toFind, newRoot->mTransmission, visited);
//				}
//			}
//		}
//		else return NULL;
//	}
//
//	Node* search(Hit toFind) {
//		bool* visited = new bool[mNumNodes];
//		for (int i = 0; i < mNumNodes; i++)
//			visited[i] = false;
//
//		Node* res = searchUtil(toFind, mRoot, visited);
//		delete(visited);
//		return res;
//	}
//
//	void addChild(Node* myParent, Node* reflective) {
//		if (myParent->mReflection) {
//			auto r = myParent->mReflection;
//			delete r;
//		}
//
//		myParent->mReflection = reflective;
//		myParent->mReflection->mType = 1;
//		mNumNodes++;
//		reflective->mIndex = mNumNodes;
//	}
//
//	void addChild(Node* myParent, Hit newTransmissive, Hit newReflective) {
//		if (myParent->mTransmission) {
//			auto t = myParent->mTransmission;
//			delete t;
//		}
//
//		if (myParent->mReflection) {
//			auto r = myParent->mReflection;
//			delete r;
//		}
//		myParent->mTransmission = new Node();
//		myParent->mTransmission->mValue = newTransmissive;
//		myParent->mTransmission->mType = 0;
//		mNumNodes++;
//		myParent->mTransmission->mIndex = mNumNodes;
//		myParent->mReflection = new Node();
//		myParent->mReflection->mValue = newReflective;
//		myParent->mReflection->mType = 1;
//		mNumNodes++;
//		myParent->mReflection->mIndex = mNumNodes;
//	};
//
//	void destroy_tree(Node* leaf) {
//		if (leaf != NULL) {
//			destroy_tree(leaf->mTransmission);
//			destroy_tree(leaf->mReflection);
//			delete leaf;
//		}
//	}
//		
//
//	Node* mRoot;
//};

struct Sampling {
	SamplingType mType;
	int sampleSize;

	Sampling(SamplingType type = NONE, int samplingSize = 2) {
		mType = type;
		sampleSize = samplingSize;
	}
};


#endif