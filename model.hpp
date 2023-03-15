#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/cimport.h>

#include <Windows.h>
#include <iostream>
#include <stdint.h>
#include <vector>

class ModelManager
{
public:
	ModelManager();
	~ModelManager(void);

	bool loadModel(std::string file);
	bool processData();
	std::vector<float>* getFaceData();
	std::vector<float>* getVertexData();
	std::vector<uint16_t>* getIndexData();

private:
	bool assimpGetMeshData(const aiMesh* mesh);
private:
	Assimp::Importer            importer;
	const aiScene* modelScene;
	const aiNode* modelNode;
	const aiMesh* modelMesh;
	const aiFace* modelFace;
	std::vector<float>			faceBuff;
	std::vector<float>          vertexBuff;
	std::vector<uint16_t>       indexBuff;

	std::vector<const aiNode*>  nodeBuff;
	unsigned int                numNodeBuff;
};