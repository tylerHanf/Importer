#include "OBJ_Importer.h"
#include <iostream>
#include <stdlib.h>
#include <sstream>

OBJ_Importer::OBJ_Importer(void) {
	BasePath = "./";
}

OBJ_Importer::OBJ_Importer(std::string basepath) {
	BasePath = basepath;
}

/*
 * Read data from .OBJ file
 * Must push initial data into respective initial vectors
 * 	in order to get correct final data per triangle index
 *
 * Currently only handles triangles with vertex, texture, and normal indices
 */
void OBJ_Importer::ReadFile(const char* filename) {
	std::ifstream file(BasePath + filename);
	if (!file.is_open()) {
		std::cout << "Could not open file." << std::endl;
	}

	std::string line, seg;
	float x, y, z;
	int i, vIndex, tIndex, nIndex, endIndex, componentIndex;
	std::istringstream input;

	//Init Vectors
	std::vector<Vec3> vertices;
	std::vector<Vec2> texels;
	std::vector<Vec3> normals;

	Model model;
	ModelComponent comp;
	model.numComponents = 0;
	componentIndex = -1; // Currently no components

	std::getline(file, line); // Move past first 2 comment lines
	std::getline(file, line);
	std::getline(file, line);
	line.erase(0, 7);
	LoadMaterial(line);

	while (std::getline(file, line)) {
		if (line[0] == 'o') {
			ModelComponent comp;
			++model.numComponents;
			++componentIndex;
			model.components.push_back(comp);
		}

		if (line[0] == 'v' && line[1] == 't') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f %f", &x, &y);
			texels.push_back(Vec2(x, y));
		}

		else if (line[0] == 'v' && line[1] == 'n') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			normals.push_back(Vec3(x, y, z));
		}

		else if (line[0] == 'v') {
			line.erase(0, 2);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			vertices.push_back(Vec3(x, y, z));
		}

		else if (line[0] == 'f') {
			line.erase(0, 2);
			input.str(line.c_str());
			while (std::getline(input, seg, ' ')) {
				sscanf_s(seg.c_str(), "%d %*c %d %*c %d", &vIndex, &tIndex, &nIndex);
				//Get to start of vertex (OBJ index starts at 1)
				vIndex = (vIndex - 1) * 3;
				tIndex = (tIndex - 1) * 2;
				nIndex = (nIndex - 1) * 3;

				//Push Vertices
				Vertices.push_back(vertices[vIndex]);

				//Push Texutres
				Texels.push_back(texels[tIndex]);

				//Push Normals
				Normals.push_back(normals[nIndex]);
			}
			input.clear();
		}

		else {
			continue;
		}
	}
	file.close();
}

bool OBJ_Importer::LoadMaterial(std::string filepath) {
	std::string line, name;
	std::ifstream fs;
	fs.open(BasePath + filepath);

	float x, y, z;
	int illum;

	Material material;

	while (std::getline(fs, line)) {
		if (line[0] == 'n') {
			line.erase(0, 7);
			material.name = line;
		}

		else if (line[0] == 'N' && line[1] == 's') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f", &x);
			material.Ns = x;
		}

		else if (line[0] == 'K' && line[1] == 'a') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			material.Ka = Vec3(x, y, z);
		}

		else if (line[0] == 'K' && line[1] == 'd') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			material.Kd = Vec3(x, y, z);
		}

		else if (line[0] == 'K' && line[1] == 's') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			material.Ks = Vec3(x, y, z);
		}

		else if (line[0] == 'K' && line[1] == 'e') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			material.Ke = Vec3(x, y, z);
		}

		else if (line[0] == 'N' && line[1] == 'i') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f", &x);
			material.Ni = x;
		}

		else if (line[0] == 'd') {
			line.erase(0, 2);
			sscanf_s(line.c_str(), "%f", &x);
			material.d = x;
		}

		else if (line[0] == 'i') {
			line.erase(0, 2);
			sscanf_s(line.c_str(), "%d", &illum);
			material.illum = illum;
		}

	}
}
