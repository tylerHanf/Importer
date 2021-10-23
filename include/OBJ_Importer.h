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

namespace Importer {

	enum BufferType { VERT=0, 
		              TEXTURE=1, 
		              NORMAL=2, 
		              VERT_TEXTURE=3, 
		              VERT_NORMAL=4, 
		              VERT_TEXTURE_NORMAL=5,
					  NUM_TYPES=6,
					  NONE=16,
					};

	enum DataLayout { INDIVIDUAL,
					  CONSECUTIVE,
					  INTERLEAVED,
					};

	struct Texture {
		int width;
		int height;
		int channels;
		std::vector<unsigned char> data;
	};

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
		Texture Kd_map;
	};

	struct ModelComponent {
		BufferType type;
		bool hasMaterial;

		int vertIndex;
		int numVerts;
		int textCoordIndex;
		int numTextCoords;
		int normIndex;
		int numNorms;
		int totalNumPoints;
		int materialIndex;
		int interleaveDataIndex;
		int consecutiveDataIndex;
	};

	struct Model {
		int numComponents;
		std::vector<ModelComponent> components;
	};

	struct Data {
		DataLayout layout;
		std::vector<float> buffers[NUM_TYPES];
	};

	// TODO: Make a master file for importing multiple .objs
	class OBJ_Importer {
	public:
		OBJ_Importer(void);
		OBJ_Importer(std::string BasePath);
		OBJ_Importer(std::string BasePath, std::string TextureBasePath);
		//Reads in various data from .obj file
		void ReadFile(const char* filename);
		// Clears all buffer data and resets importer state except for base paths
		void CleanBuffers(void);
		void SetBasePath(std::string basePath);
		void SetTextureBasePath(std::string textureBasePath);

		Data InterleaveData(void);

		std::vector<float> Vertices;
		std::vector<float> Texels;
		std::vector<float> Normals;
		std::vector<int>   Indices;
		std::vector<Material> Materials;
		std::vector<Model> Models;

		std::string BasePath;
		std::string TextureBasePath;

	private:
		void SetComponent(ModelComponent* comp, bool hasVerts, bool hasTextCoords, bool hasNorms);
		bool LoadMaterials(std::string filepath);
		bool LoadPNG(std::string filename, Texture& texture);
	};
}
