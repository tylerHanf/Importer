#include "OBJ_Importer.h"
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include "Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace Importer;

OBJ_Importer::OBJ_Importer(void) {
	BasePath = "./";
	TextureBasePath = "./";
}

OBJ_Importer::OBJ_Importer(std::string basepath) {
	BasePath = basepath;
	TextureBasePath = "./";
}

OBJ_Importer::OBJ_Importer(std::string basepath, std::string textureBasePath) {
	BasePath = basepath;
	TextureBasePath = textureBasePath;
}

/*
 * Read data from .OBJ file
 * Must push initial data into respective initial vectors
 * 	in order to get correct final data per triangle index
 * Is OpenGL implementation agnostic, it does not care about interleaved/indexed/etc.
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
	std::vector<int>  indices;

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

		// TODO: Make indexed
		else if (line[0] == 'f') {
			line.erase(0, 2);
			input.str(line.c_str());
			while (std::getline(input, seg, ' ')) {
				sscanf_s(seg.c_str(), "%d %*c %d %*c %d", &vIndex, &tIndex, &nIndex);

				//Get to start of vertex (OBJ index starts at 1, need to subtract 1
				vIndex = vIndex - 1;
				tIndex = tIndex - 1;
				nIndex = nIndex - 1;

				//Push Vertices
				Vertices.push_back(vertices[vIndex][0]);
				Vertices.push_back(vertices[vIndex][1]);
				Vertices.push_back(vertices[vIndex][2]);

				//Push Texutres
				Texels.push_back(texels[tIndex][0]);
				Texels.push_back(texels[tIndex][1]);

				//Push Normals
				Normals.push_back(normals[nIndex][0]);
				Normals.push_back(normals[nIndex][1]);
				Normals.push_back(normals[nIndex][2]);
			}
			input.clear();
		}
		else if (line[0] == 'u') {
			
		}
		else {
			continue;
		}
	}
	file.close();
}

bool OBJ_Importer::LoadMaterial(std::string filepath) {
	std::string line, name;
	name.reserve(256);
	std::ifstream fs;
	std::string str = "dummy string";
	fs.open(BasePath + filepath);
	if (!fs.is_open()) {
		Logger::LogPrint("Failed to open material file: %s", filepath);
	}

	float x, y, z;
	int illum;

	Material material;

	while (std::getline(fs, line)) {
		// newmtl
		if (line[0] == 'n') {
			line.erase(0, 7);
			material.name = line;
			Logger::LogPrint("Name: %s", line.c_str());
		}
		// Ns 
		else if (line[0] == 'N' && line[1] == 's') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f", &x);
			material.Ns = x;
			Logger::LogPrint("Ns: %s", line.c_str());
		}
		// Ka
		else if (line[0] == 'K' && line[1] == 'a') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			material.Ka = Vec3(x, y, z);
			Logger::LogPrint("Ka: %s", line.c_str());
		}
		// Kd
		else if (line[0] == 'K' && line[1] == 'd') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			material.Kd = Vec3(x, y, z);
			Logger::LogPrint("Kd: %s", line.c_str());
		}
		// Ks
		else if (line[0] == 'K' && line[1] == 's') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			material.Ks = Vec3(x, y, z);
			Logger::LogPrint("Ks: %s", line.c_str());
		}
		// Ke
		else if (line[0] == 'K' && line[1] == 'e') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			material.Ke = Vec3(x, y, z);
			Logger::LogPrint("Ke: %s", line.c_str());
		}
		// Ni
		else if (line[0] == 'N' && line[1] == 'i') {
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f", &x);
			material.Ni = x;
			Logger::LogPrint("Ni: %s", line.c_str());
		}
		// d
		else if (line[0] == 'd') {
			line.erase(0, 2);
			sscanf_s(line.c_str(), "%f", &x);
			material.d = x;
			Logger::LogPrint("d: %s", line.c_str());
		}
		// illum
		else if (line[0] == 'i') {
			line.erase(0, 6);
			sscanf_s(line.c_str(), "%d", &illum);
			material.illum = illum;
			Logger::LogPrint("i: %s", line.c_str());
		}
		/*
		// map_d
		else if (line[0] == 'm' && line[4] == 'd') { 
			line.erase(0, 4);
			sscanf_s(line.c_str(), "%s", name, 256);
		}
		// map_Ns
		else if (line[0] == 'm' && line[4] == 'N') { 
			line.erase(0, 4);
			sscanf_s(line.c_str(), "%s", name, 256);
		}
		// map_Ka
		else if (line[0] == 'm' && line[5] == 'a') {
			line.erase(0, 6);
			sscanf_s(line.c_str(), "%s");
		}
		*/
		// map_Kd
		else if (line[0] == 'm' && line[5] == 'd') {
			Logger::LogPrint("material: %s", line.c_str());
			line.erase(0, 7);
			//sscanf_s(line.c_str(), "%s", name, 256);
			LoadPNG(line, material.Kd_map);
		}
		/*
		// map_Ks
		else if (line[0] == 'm' && line[5] == 's') {
		}
		// map_Ke
		else if (line[0] == 'm' && line[5] == 'e') {
		}
		// map_bump and bump
		else if (line[0] == 'm' && line[4] == 'b' || line[0] == 'b') {
		}
		// disp
		else if (line[0] == 'd' && line[1] == 'i') {
		}
		// decal
		else if (line[0] == 'd' && line[1] == 'e') {
		}
		*/
	}

	Materials.push_back(material);
	return true;
}

bool OBJ_Importer::LoadPNG(std::string filename, Texture& texture) {
	Logger::LogPrint("%s", filename.c_str());
	texture.data = stbi_load((TextureBasePath + filename).c_str(), 
		                      &texture.width, &texture.height, &texture.channels, 0);
	if (texture.data == NULL) {
		Logger::LogPrint("Failed to load .png");
	}

	return true;
}
