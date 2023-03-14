#include "model.hpp"

ModelManager::ModelManager()
{
    vertexBuff.clear();
    indexBuff.clear();
    std::cout << "=======Mesh Manager  initialized=======" << std::endl;

}

ModelManager::~ModelManager(void)
{
    std::cout << "ModelManager destroyed" << std::endl;
}

bool ModelManager::loadModel(std::string file)
{
    modelScene = importer.ReadFile(file, aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_PreTransformVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals |
        aiProcess_Triangulate |
        aiProcess_FixInfacingNormals |
        aiProcess_FindInvalidData |
        aiProcess_ValidateDataStructure | 0

    );

    if (!modelScene)
    {
        MessageBoxA(NULL, importer.GetErrorString(), "Error: ", MB_ICONERROR);
        return false;
    }
    else
        processData();

    return true;
}


bool ModelManager::assimpGetMeshData(const aiMesh* mesh)
{
    aiFace* face;


    for (unsigned int v = 0; v < mesh->mNumVertices; v++)
    {
        vertexBuff.push_back(mesh->mVertices[v].x);
        vertexBuff.push_back(mesh->mVertices[v].y);
        vertexBuff.push_back(mesh->mVertices[v].z);


        vertexBuff.push_back(mesh->mNormals[v].x);
        vertexBuff.push_back(mesh->mNormals[v].y);
        vertexBuff.push_back(mesh->mNormals[v].z);




        if (mesh->HasTextureCoords(0)) {
            vertexBuff.push_back(mesh->mTextureCoords[0][v].x);
            vertexBuff.push_back(mesh->mTextureCoords[0][v].y);
        }
        else
        {
            vertexBuff.push_back(0);
            vertexBuff.push_back(0);
        }
        /*
        vertexBuff.push_back(mesh->mTangents[v].x);
        vertexBuff.push_back(mesh->mTangents[v].y);
        vertexBuff.push_back(mesh->mTangents[v].z);*/

    }

    for (unsigned int f = 0; f < mesh->mNumFaces; f++)
    {
        face = &mesh->mFaces[f];
        indexBuff.push_back(face->mIndices[0]);
        indexBuff.push_back(face->mIndices[1]);
        indexBuff.push_back(face->mIndices[2]);
    }

    return true;
}
bool ModelManager::processData()
{
    bool repeat = true;

    nodeBuff.push_back(modelScene->mRootNode);


    /* if (modelScene->mNumMeshes > 0)
    {
    for (unsigned int m=0;m<modelScene->mNumMeshes;m++)
    this->assimpGetMeshData(modelScene->mMeshes[m]);
    }*/

    // I raise all nodes tree to the root level 
    while (repeat)
    {
        for (unsigned int a = 0; a < nodeBuff.size(); a++)
        {
            modelNode = nodeBuff.at(a);
            if (modelNode->mNumChildren > 0)
                for (unsigned int c = 0; c < modelNode->mNumChildren; c++)
                {
                    nodeBuff.push_back(modelNode->mChildren[c]);

                }

            else repeat = false;
        }
    }

    // Get node information from the root level (all nodes)
    for (unsigned int a = 0; a < nodeBuff.size(); a++)
    {
        modelNode = nodeBuff.at(a);

        if (modelNode->mNumMeshes > 0)
            for (unsigned int b = 0; b < modelNode->mNumMeshes; b++) {
                assimpGetMeshData(modelScene->mMeshes[b]);
                //std::cout << "::::::::PROCESSING =>" << modelScene->mMeshes[b]->mName;
            }
    }
    return true;
}

std::vector<float>* ModelManager::getVertexData()
{
    return &vertexBuff;
}

std::vector<uint16_t>* ModelManager::getIndexData()
{
    return &indexBuff;
}