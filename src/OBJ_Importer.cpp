#include "OBJ_Importer.h"
#include <iostream>
#include <stdlib.h>

OBJ_Importer::OBJ_Importer(void) {
	BasePath = "./";
}

OBJ_Importer::OBJ_Importer(std::string basepath) {
	BasePath = basepath;
}

bool OBJ_Importer::loadOBJ(std::string objPath) {
	Model model;
	ModelComponent component = { 0, 0, 0, 0 };

	model.numComponents = 0;

	std::fstream fileStream;
	fileStream.open(BasePath + objPath, std::fstream::in);

	if (!fileStream.is_open()) {
		return false;
	}

	char currentLine[MAX_LINE_SIZE];
	char materialPath[MAX_LINE_SIZE];

	fileStream.getline(currentLine, MAX_LINE_SIZE);
	fileStream.getline(currentLine, MAX_LINE_SIZE);
	fileStream.getline(currentLine, MAX_LINE_SIZE);

	// Extract material file path
	for (int i = 0; i < MAX_LINE_SIZE-7; ++i) {
		materialPath[i] = currentLine[i + 7];
	}

	if (!loadMaterial(materialPath)) {
		return false;
	}

	while (!fileStream.eof()) {
		ModelComponent component;
		fileStream.getline(currentLine, MAX_LINE_SIZE);
		getVertices(fileStream, &component);
		getTextCoords(fileStream, &component);
		getNormals(fileStream, &component);
		getMaterial(fileStream, &component);
		fileStream.getline(currentLine, MAX_LINE_SIZE); // Ignore s line
		getFaces(fileStream, &component);
		model.components.push_back(component);
	}

	model.numComponents = model.components.size();
	Models.push_back(model);
	return true;
}

bool OBJ_Importer::loadMaterial(std::string filepath) {
	std::fstream fileStream;

	fileStream.open(BasePath + filepath, std::fstream::in);
	if (!fileStream.is_open()) {
		return false;
	}
	
	char currentLine[MAX_LINE_SIZE];
	std::string materialName;
	char* token = NULL;
	char* nextToken = NULL;
	float x, y, z;
	int numMaterials = 0;

	// Get number of materials
	fileStream.getline(currentLine, MAX_LINE_SIZE);
	fileStream.getline(currentLine, MAX_LINE_SIZE);
	token = strtok_s(currentLine, " ", &nextToken);
	token = strtok_s(NULL, " ", &nextToken);
	numMaterials = atoi(token);

	for (int i = 0; i < numMaterials; ++i) {
		Material material;
		fileStream.getline(currentLine, MAX_LINE_SIZE);
		fileStream.getline(currentLine, MAX_LINE_SIZE);
		materialName = currentLine;
		material.name = materialName.substr(7);

		fileStream.getline(currentLine, MAX_LINE_SIZE); // Ns
		parseFloat(currentLine, x);
		material.Ns = x;

		fileStream.getline(currentLine, MAX_LINE_SIZE); // Ka
		parseVec3(currentLine, x, y, z);
		material.Ka = Vec3(x, y, z);

		fileStream.getline(currentLine, MAX_LINE_SIZE); // Kd
		parseVec3(currentLine, x, y, z);
		material.Kd = Vec3(x, y, z);

		fileStream.getline(currentLine, MAX_LINE_SIZE); // Ks
		parseVec3(currentLine, x, y, z);
		material.Ks = Vec3(x, y, z);

		fileStream.getline(currentLine, MAX_LINE_SIZE); // Ke
		parseVec3(currentLine, x, y, z);
		material.Ke = Vec3(x, y, z);

		fileStream.getline(currentLine, MAX_LINE_SIZE); // Ni
		parseFloat(currentLine, x);
		material.Ni = x;

		fileStream.getline(currentLine, MAX_LINE_SIZE); // d
		parseFloat(currentLine, x);
		material.d = x;

		fileStream.getline(currentLine, MAX_LINE_SIZE); // illum
		parseFloat(currentLine, x);
		material.illum = x;

		Materials.push_back(material);
	}

	return true;
}

bool OBJ_Importer::getVertices(std::fstream& fs, ModelComponent* curComp) {
	char curVert[MAX_LINE_SIZE];
	char* token = NULL;
	char* nextToken = NULL;
	float x, y, z;
	int curLine = 0;

	curComp->vertIndex = Vertices.size();

	curLine = (int)fs.tellg();
	fs.getline(curVert, MAX_LINE_SIZE);
	while (curVert[0] == 'v' && curVert[1] == ' ') {
		parseVec3(curVert, x, y, z);
		Vertices.push_back(Vec3(x, y, z));
		curLine = (int)fs.tellg();
	    fs.getline(curVert, MAX_LINE_SIZE);
	}
	fs.seekg(curLine-1, fs.beg);
	fs.getline(curVert, MAX_LINE_SIZE);

	return true;
}

bool OBJ_Importer::getTextCoords(std::fstream& fs, ModelComponent* curComp) {
	char curTextCoord[MAX_LINE_SIZE];
	char* token = NULL;
	char* nextToken = NULL;
	float x, y;
	int curLine = 0;
	curComp->textCoordIndex = TextCoords.size();

	curLine = (int)fs.tellg();
	fs.getline(curTextCoord, MAX_LINE_SIZE);
	while (curTextCoord[0] == 'v' && curTextCoord[1] == 't') {
		parseVec2(curTextCoord, x, y);
		TextCoords.push_back(Vec2(x, y));
		curLine = (int)fs.tellg();
		fs.getline(curTextCoord, MAX_LINE_SIZE);
	}
	fs.seekg(curLine-1, fs.beg);
	fs.getline(curTextCoord, MAX_LINE_SIZE);

	return true;
}

bool OBJ_Importer::getNormals(std::fstream& fs, ModelComponent* curComp) {
	char curNorm[MAX_LINE_SIZE];
	char* token = NULL;
	char* nextToken = NULL;
	float x, y, z = 0.0;
	int curLine = 0;
	curComp->normIndex = Normals.size();

	curLine = (int)fs.tellg();
	fs.getline(curNorm, MAX_LINE_SIZE);
	while (curNorm[0] == 'v' && curNorm[1] == 'n') {
		parseVec3(curNorm, x, y, z);
		Normals.push_back(Vec3(x, y, z));
		curLine = (int)fs.tellg();
		fs.getline(curNorm, MAX_LINE_SIZE);
	}
	fs.seekg(curLine-1, fs.beg);
	fs.getline(curNorm, MAX_LINE_SIZE);

	return true;
}

bool OBJ_Importer::getMaterial(std::fstream& fs, ModelComponent* curComp) {
	char materialLine[MAX_LINE_SIZE];
	char* token = NULL;
	char* nextToken = NULL;
	fs.getline(materialLine, MAX_LINE_SIZE);
	
	token = strtok_s(materialLine, " ", &nextToken);
	token = strtok_s(NULL, " ", &nextToken);

	// Currently just iterates through materials vector **SLOW**
	for (int i = 0; i < (int)Materials.size(); ++i) {
		if (strcmp(token, Materials[i].name.c_str()) == 0) {
			curComp->materialIndex = i;
		}
	}

	return true;
}

bool OBJ_Importer::getFaces(std::fstream& fs, ModelComponent* curComp) {
	char curFace[MAX_LINE_SIZE];
	char* token = NULL;
	char* vertToken = NULL;
	char* nextToken = NULL;
	char* vertNextToken = NULL;
	int vertIndex, textIndex, normIndex = 0;
	curComp->facesIndex = Faces.size();

	fs.getline(curFace, MAX_LINE_SIZE);
	while (curFace[0] == 'f') {
		token = strtok_s(curFace, " ", &nextToken);
		while (token != NULL) {
			token = strtok_s(NULL, " ", &nextToken);
			vertNextToken = strtok_s(token, "/", &vertNextToken);
			vertIndex = atoi(vertNextToken);
			vertNextToken = strtok_s(NULL, "/", &vertNextToken);
			if (vertNextToken != "") {
				textIndex =  atoi(vertNextToken);
			}
			else {
				textIndex = -1;
			}
			vertNextToken = strtok_s(NULL, "/", &vertNextToken);
			normIndex = atoi(vertNextToken);
			Faces.push_back(Vec3((float)vertIndex, (float)textIndex, (float)normIndex));
		}
		fs.getline(curFace, MAX_LINE_SIZE);
	}

	return true;
}

void OBJ_Importer::parseVec3(char* line, float& x, float& y, float& z) {
	char* token = NULL;
	char* nextToken = NULL;

	token = strtok_s(line, " ", &nextToken); // Get rid of attribute prefix
	token = strtok_s(NULL, " ", &nextToken); // Get X
	x = (float)atof(token);
	token = strtok_s(NULL, " ", &nextToken); // Get Y
	y = (float)atof(token);
	token = strtok_s(NULL, " ", &nextToken); // Get Z
	z = (float)atof(token);
}

void OBJ_Importer::parseVec2(char* line, float& x, float& y) {
	char* token = NULL;
	char* nextToken = NULL;

	token = strtok_s(line, " ", &nextToken); // Get rid of attribute prefix
	token = strtok_s(NULL, " ", &nextToken); // Get X
	x = (float)atof(token);
	token = strtok_s(NULL, " ", &nextToken); // Get Y
	y = (float)atof(token);
}

void OBJ_Importer::parseFloat(char* line, float& x) {
	char* token = NULL;
	char* nextToken = NULL;

	token = strtok_s(line, " ", &nextToken); // Get rid of attribute prefix
	token = strtok_s(NULL, " ", &nextToken); // Get X
	x = (float)atof(token);
}

int OBJ_Importer::charToInt(char char_num) {
	return (int)char_num - (int)'0';
}
