#pragma once

#include <fstream>
#include <string>
#include <vector>
#include "Vec3.h"

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
	OBJ_Importer(std::string basePath);

	bool loadOBJ(std::string objPath);
	
private:
	std::vector<Model> models;
	std::vector<Vec3> Vertices;
	std::vector<Vec3> Normals;
	std::vector<Vec2> TextCoords;
	std::vector<Vec3> Faces;
	std::vector<Material> Materials;
	std::vector<Model> Models;

	std::string BasePath;

	bool loadMaterial(std::string filepath);
	bool getVertices(std::fstream& fs, ModelComponent* curComp);
	bool getTextCoords(std::fstream& fs, ModelComponent* curComp);
	bool getNormals(std::fstream& fs, ModelComponent* curComp);
	bool getMaterial(std::fstream& fs, ModelComponent* curComp);
	bool getFaces(std::fstream& fs, ModelComponent* curComp);
	void parseVec3(char* line, float& x, float& y, float& z);
	void parseVec2(char* line, float& x, float& y);
	void parseFloat(char* line, float& val);

	static int  charToInt(char char_num);
};
