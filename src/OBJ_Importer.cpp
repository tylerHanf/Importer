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
	int i, vIndex, tIndex, nIndex, endIndex, componentIndex = 1;
	std::istringstream input;

	//Init Vectors
	std::vector<Vec3> vertices;
	std::vector<Vec2> texels;
	std::vector<Vec3> normals;
	std::vector<int>  indices;

	Model model;
	model.numComponents = 0;
	ModelComponent comp;
	comp.type = NONE;
	comp.numVerts = 0;
	comp.numTextCoords = 0;
	comp.numNorms = 0;
	comp.totalNumPoints = 0;
	comp.materialIndex = -1;
	int vertIndex, textCoordIndex, normIndex, materialIndex;
	bool hasVerts = false;
	bool hasTextCoords = false;
	bool hasNorms = false;
	componentIndex = -1; // Currently no components

	std::getline(file, line); // Move past first 2 comment lines
	std::getline(file, line);
	std::getline(file, line);
	line.erase(0, 7);
	LoadMaterials(line);

	while (std::getline(file, line)) {
		if (line[0] == 'o') {
			SetComponent(&comp, hasVerts, hasTextCoords, hasNorms);
			if (comp.type != NONE) {
				++model.numComponents;
				model.components.push_back(comp);
				// Set indices
				hasVerts = false;
				hasTextCoords = false;
				hasNorms = false;
				comp.type = NONE;
				comp.numTextCoords = 0;
				comp.numTextCoords = 0;
				comp.numNorms = 0;
				comp.totalNumPoints = 0;
				comp.vertIndex = -1;
				comp.textCoordIndex = -1;
				comp.normIndex = -1;
				comp.materialIndex = -1;
			}
		}

		if (line[0] == 'v' && line[1] == 't') {
			if (comp.numTextCoords == 0)
				hasTextCoords = true;
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f %f", &x, &y);
			texels.push_back(Vec2(x, y));
			++comp.numTextCoords;
			comp.totalNumPoints += 2;
		}

		else if (line[0] == 'v' && line[1] == 'n') {
			if (comp.numNorms == 0)
				hasNorms = true;
			line.erase(0, 3);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			normals.push_back(Vec3(x, y, z));
			++comp.numNorms;
			comp.totalNumPoints += 3;
		}

		else if (line[0] == 'v') {
			if (comp.numVerts == 0)
				hasVerts = true;
			
			line.erase(0, 2);
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			vertices.push_back(Vec3(x, y, z));
			++comp.numVerts;
			comp.totalNumPoints += 3;
		}

		// TODO: Make indexed
		else if (line[0] == 'f') {
			line.erase(0, 2);
			input.str(line.c_str());
			while (std::getline(input, seg, ' ')) {
				vIndex = -1;
				nIndex = -1;
				tIndex = -1;
				if (strstr(seg.c_str(), "//") != NULL) {
					sscanf_s(seg.c_str(), "%d %*2c %d", &vIndex, &nIndex);
				}
				else {
					sscanf_s(seg.c_str(), "%d %*c %d %*c %d", &vIndex, &tIndex, &nIndex);
				}

				//Get to start of vertex (OBJ index starts at 1, need to subtract 1
				vIndex = vIndex - 1;
				tIndex = tIndex - 1;
				nIndex = nIndex - 1;

				//Push Vertices
				if (vertices.size() > 0) {
					Vertices.push_back(vertices[vIndex][0]);
					Vertices.push_back(vertices[vIndex][1]);
					Vertices.push_back(vertices[vIndex][2]);
				}

				//Push Texutres
				if (texels.size() > 0) {
					Texels.push_back(texels[tIndex][0]);
					Texels.push_back(texels[tIndex][1]);
				}

				//Push Normals
				if (normals.size() > 0) {
					Normals.push_back(normals[nIndex][0]);
					Normals.push_back(normals[nIndex][1]);
					Normals.push_back(normals[nIndex][2]);
				}
			}

			//vertices.clear();
			//texels.clear();
			//normals.clear();
			input.clear();
		}
		else if (line[0] == 'u') {
			line.erase(0, 7);
			// Find material by iterating through material list and comparing names
			for (int i = 0; i < Materials.size(); ++i) {
				if (Materials[i].name.compare("Material") == 0) {
					comp.materialIndex = i;
				}
			}
		}
		else {
			continue;
		}
	}
	SetComponent(&comp, hasVerts, hasTextCoords, hasNorms);
	++model.numComponents;
	model.components.push_back(comp);
	Models.push_back(model);
	file.close();
}

void OBJ_Importer::SetComponent(ModelComponent* comp, bool hasVerts, bool hasTextCoords, bool hasNorms) {
	if (hasVerts) {
		comp->vertIndex = Vertices.size() - (comp->numVerts * 3);
		comp->type = VERT;
	}
	if (hasTextCoords) {
		comp->textCoordIndex = Texels.size() - (comp->numTextCoords * 2);
		if (comp->type == VERT) { comp->type = VERT_TEXTURE; }
		else { comp->type = TEXTURE;  }
	}
	if (hasNorms) {
		comp->normIndex = Normals.size() - (comp->numNorms * 2);
		if (comp->type == VERT_TEXTURE) { comp->type = VERT_TEXTURE_NORMAL;  }
		else if (comp->type == VERT) { comp->type = VERT_NORMAL;  }
		else { comp->type = NORMAL;  }
	}
}

bool OBJ_Importer::LoadMaterials(std::string filepath) {
	std::string line, name;
	name.reserve(256);
	std::ifstream fs;
	std::string str;
	fs.open(BasePath + filepath);
	if (!fs.is_open()) {
		Logger::LogPrint("Failed to open material file: %s", filepath);
	}

	float x, y, z;
	int illum;

	Material material;
	
	int numMaterials = 0;
	while (std::getline(fs, line)) {
		// newmtl
		if (line[0] == 'n') {
			++numMaterials;
			if (numMaterials > 1) {
				Materials.push_back(material);
			}
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
		// map_Kd
		else if (line[0] == 'm' && line[5] == 'd') {
			Logger::LogPrint("material: %s", line.c_str());
			line.erase(0, 7);
			material.Kd_map.channels = 0;
			material.Kd_map.height = 0;
			material.Kd_map.width = 0;
			LoadPNG(line, material.Kd_map);
		}
	}
	Materials.push_back(material);

	return true;
}

bool OBJ_Importer::LoadPNG(std::string filename, Texture& texture) {
	Logger::LogPrint("%s", filename.c_str());
	unsigned char* result = stbi_load((TextureBasePath + filename).c_str(), 
									   &texture.width, &texture.height, &texture.channels, 0);
	int size = texture.width * texture.height * texture.channels;
	texture.data = std::vector<unsigned char>(result, result+size);

	if (texture.data.size() == 0) {
		Logger::LogPrint("Failed to load .png");
	}

	return true;
}

Data OBJ_Importer::InterleaveData(void) {
	Model model;
	ModelComponent comp;
	Data data;
	data.layout = INTERLEAVED;
	int i, j, k, amount;
	for (i=0; i < Models.size(); ++i) {
		model = Models[i];
		for (j = 0; j < model.numComponents; ++j) {
			comp = model.components[j];
			Models[i].components[j].interleaveDataIndex = data.buffers[comp.type].size();
			switch (comp.type) {
			case VERT:
				for (k = 0; k < comp.numVerts; ++k)
					data.buffers[VERT].push_back(Vertices[comp.vertIndex + k]);
				break;
			case TEXTURE:
				for (k = 0; k < comp.numTextCoords; ++k)
					data.buffers[TEXTURE].push_back(Texels[comp.textCoordIndex + k]);
				break;
			case NORMAL:
				for (k = 0; k < comp.numNorms; ++k)
					data.buffers[NORMAL].push_back(Normals[comp.normIndex + k]);
				break;
			case VERT_TEXTURE:
				for (k = 0; k < comp.numVerts; ++k) {
					data.buffers[VERT_TEXTURE].push_back(Vertices[comp.vertIndex + k]);
					data.buffers[VERT_TEXTURE].push_back(Texels[comp.textCoordIndex + k]);
				}
				break;
			case VERT_NORMAL:
				for (k = 0; k < comp.numVerts; ++k) {
					data.buffers[VERT_NORMAL].push_back(Vertices[comp.vertIndex + k]);
					data.buffers[VERT_NORMAL].push_back(Normals[comp.normIndex + k]);
				}
				break;
			case VERT_TEXTURE_NORMAL:
				for (k = 0; k < comp.numVerts; ++k) {
					data.buffers[VERT_TEXTURE_NORMAL].push_back(Vertices[comp.vertIndex + k]);
					data.buffers[VERT_TEXTURE_NORMAL].push_back(Texels[comp.textCoordIndex + k]);
					data.buffers[VERT_TEXTURE_NORMAL].push_back(Normals[comp.normIndex + k]);
				}
				break;
			default:
				break;
			}
		}
	}
	return data;
}
