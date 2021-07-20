#pragma once

#include <fstream>
#include <string>
#include <vector>
#include "Vec3.h"
#include "Vec2.h"

#define MAX_LINE_SIZE 256
#define MAX_ARRAY_SIZE 10000
#define MAX_COMPONENTS 20
#define OBJ_FLOAT_LEN 8

struct Material {
	std::string name;
	float Ns;
	Vec3  Ka;
	Vec3  Kd;
	Vec3  Ks;
	Vec3  Ke;
	float Ni;
	float d;
	float illum;
};

struct ModelComponent {
	int vertIndex;
	int textCoordIndex;
	int normIndex;
	int indicesIndex;
	int facesIndex;
	int materialIndex;
};

struct Model {
	int numComponents;
	std::vector<ModelComponent> components;
};

class OBJ_Importer {
public:
	OBJ_Importer(void);
	OBJ_Importer(std::string BasePath);
	//Reads in various data from .obj file
	void ReadFile(const char* filename);
	//Gets the filepath of the texture associated with the object

private:
	std::vector<float> Vertices;
	std::vector<float> Texels;
	std::vector<float> Normals;
	std::vector<Model> Models;
	std::vector<Material> Materials;
	std::string BasePath;

	bool LoadMaterial(std::string filepath);
	bool LoadPNG(std::string filename);
};
