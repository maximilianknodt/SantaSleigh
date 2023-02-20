//
//  Model.cpp
//  ogl4
//
//  Created by Philipp Lensing on 21.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Model.h"
#include "phongshader.h"
#include <list>
#include <cfloat>
#include <fstream>
#include <filesystem>

Model::Model() : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
{
}
Model::Model(const char *ModelFile, bool FitSize) : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0)
{
    bool ret = load(ModelFile, FitSize);
    if (!ret)
        throw std::exception();
}

Model::~Model()
{
    delete this->pMeshes;
    delete this->pMaterials;
    deleteNodes(&RootNode);
}

void Model::deleteNodes(Node *pNode)
{
    if (!pNode)
        return;

    for (unsigned int i = 0; i < pNode->ChildCount; ++i)
        deleteNodes(&(pNode->Children[i]));
    if (pNode->ChildCount > 0)
        delete[] pNode->Children;
    if (pNode->MeshCount > 0)
        delete[] pNode->Meshes;
}

bool Model::load(const char *ModelFile, bool FitSize)
{
    const aiScene *pScene = aiImportFile(ModelFile, aiProcessPreset_TargetRealtime_Fast | aiProcess_TransformUVCoords);

    if (pScene == NULL || pScene->mNumMeshes <= 0)
        return false;

    Filepath = ModelFile;
    Path = Filepath;
    size_t pos = Filepath.rfind('/');
    if (pos == std::string::npos)
        pos = Filepath.rfind('\\');
    if (pos != std::string::npos)
        Path.resize(pos + 1);

    loadMeshes(pScene, FitSize);
    loadMaterials(pScene);
    loadNodes(pScene);

    std::cout << "Nodes: " << pScene->mRootNode->mNumChildren << std::endl;

    return true;
}

/// <summary>
/// Laedt Meshes (Polygonale Modelle) aus aiScene::Mesh in Model::Mesh und MeshCount
/// </summary>
/// mesh::mVertices = Position
/// mesh::mNormals = Normalen
/// mesh::mTextureCoords = UV-Texturkoordinaten
/// <param name="pScene"></param>
/// <param name="FitSize"></param>
void Model::loadMeshes(const aiScene *pScene, bool FitSize) {
    this->calcBoundingBox(pScene, this->BoundingBox);

    this->MeshCount = pScene->mNumMeshes;
    this->pMeshes = new Mesh[this->MeshCount];

    // Jedes Mesh der Szene behandeln
    for (int i = 0; i < this->MeshCount; i++) {

        Mesh &mesh = this->pMeshes[i];
        aiMesh *aiMesh = pScene->mMeshes[i];

        mesh.MaterialIdx = aiMesh->mMaterialIndex;
        unsigned int aiVertCount = aiMesh->mNumVertices;

        // Der Vertexbuffer des aktuellen Model::Mesh wird mit den Normalen-,
        // Texturkoordinaten- und Positionsvectoren des aiMeshes befuellt
        // Auf die Reihenfolge achten: Normale, Textur, Position
        mesh.VB.begin();
        for (int j = 0; j < aiVertCount; j++) {
            if (aiMesh->HasNormals()){
                aiVector3D aiVNorm = aiMesh->mNormals[j];
                mesh.VB.addNormal(aiVNorm.x, aiVNorm.y, aiVNorm.z);
            }

            if (aiMesh->HasTextureCoords(0)){
                aiVector3D aiVTxt = aiMesh->mTextureCoords[0][j];
                mesh.VB.addTexcoord0(aiVTxt.x, -aiVTxt.y);
            }

            if (aiMesh->HasTangentsAndBitangents()) {
                aiVector3D tangent = aiMesh->mTangents[j];
                aiVector3D bitangent = aiMesh->mBitangents[j];

                mesh.VB.addTexcoord1(tangent.x, tangent.y, tangent.z);
                mesh.VB.addTexcoord2(bitangent.x, bitangent.y, bitangent.z);
            }

            if (aiMesh->HasPositions()){
                aiVector3D aiVPos = aiMesh->mVertices[j];
                mesh.VB.addVertex(aiVPos.x, aiVPos.y, aiVPos.z);
            }
        }
        mesh.VB.end();

        // Der Indexbuffer des aktuellen Model::Mesh wird mit den Indices fuer die Dreiecke befuellt
        unsigned int faceCount = aiMesh->mNumFaces;
        mesh.IB.begin();
        for (int j = 0; j < faceCount; j++) {
            aiFace face = aiMesh->mFaces[j];
            int faceInds = face.mNumIndices;

            // Immer 3 Punkte fuer ein Dreieck notwendig
            if (faceInds < 3)
                continue;

            // Dreiecke erzeugen (0-1-2, 0-2-3, 0-4-5, ...)
            for (int v = 0; v < faceInds - 2; v++) {
                mesh.IB.addIndex(face.mIndices[0]);
                mesh.IB.addIndex(face.mIndices[v + 1]);
                mesh.IB.addIndex(face.mIndices[v + 2]);
            }
        }
        mesh.IB.end();

        this->pMeshes[i] = mesh;
    }

    // Skallierung des Meshes (default true)
    if (FitSize) {
        Matrix matrix;
        float max = 10;
        float min = 0;

        float bboxDiagonale = (Vector(max) - Vector(min)).length();
        float modelDiagonale = (this->BoundingBox.Max - this->BoundingBox.Min).length();

        float factor = bboxDiagonale / modelDiagonale;

        matrix.scale(factor);

        this->transform(matrix);
    }
}

void Model::loadMaterials(const aiScene *pScene)
{
    this->MaterialCount = pScene->mNumMaterials;
    this->pMaterials = new Material[this->MaterialCount];

    aiMaterial *aiMat;
    Material mat;

    for (int i = 0; i < this->MaterialCount; i++)
    {
        aiMat = pScene->mMaterials[i];
        aiColor3D color;

        // Farbe fuer Diffuse, Specular, Ambient Componente in color speicher und in 'material' zuordnen
        aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        mat.DiffColor = Color(color.r, color.g, color.b);

        aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color);
        mat.SpecColor = Color(color.r, color.g, color.b);

        aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color);
        mat.AmbColor = Color(color.r, color.g, color.b);

        // Dateipfad der Diffuse-Texture holen und dem Material zuweisen
        aiString assetPath;
        aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &assetPath);
        std::string apString = assetPath.C_Str();
        std::string ap = this->Path + apString;
        mat.DiffTex = Texture().LoadShared(ap.c_str());

        if (!apString.empty()) {
            std::string normalMapPath = apString;
            // Finde den letzten Punkt im Pfad und speichere die Position
            size_t lastDotPos = normalMapPath.find_last_of(".");
            // Dateiname ohne Dateiendung
            std::string pathWithoutFilename = normalMapPath.substr(0, lastDotPos);
            // Dateiendung
            std::string filenameWithoutPath = normalMapPath.substr(lastDotPos, normalMapPath.size() - 1);
            // Dateiname + '_n' + Dateiendung
            normalMapPath = pathWithoutFilename + "_n" + filenameWithoutPath;
            
            std::string anp = this->Path + normalMapPath;

            // Wenn die Datei existiert, dann wird sie geladen, sonst die default Texture laden
            std::filesystem::path p(anp);
            bool exists = std::filesystem::exists(p);
            if (exists) { mat.NormalMapTex = Texture().LoadShared(anp.c_str()); }
            else {  mat.NormalMapTex = Texture().defaultNormalTex(); }
        }

        this->pMaterials[i] = mat;
    }
}

void Model::calcBoundingBox(const aiScene *pScene, AABB &Box)
{
    float minX, minY, minZ, maxX, maxY, maxZ;
    minX = minY = minZ = FLT_MAX;
    maxX = maxY = maxZ = FLT_MIN;

    MeshCount = pScene->mNumMeshes;
    for (size_t i = 0; i < MeshCount; i++) {
        for (size_t j = 0; j < pScene->mMeshes[i]->mNumVertices; j++) {
            aiVector3D v = pScene->mMeshes[i]->mVertices[j];
            if (minX > v.x) minX = v.x;
            if (minY > v.y) minY = v.y;
            if (minZ > v.z) minZ = v.z;
            if (maxX < v.x) maxX = v.x;
            if (maxY < v.y) maxY = v.y;
            if (maxZ < v.z) maxZ = v.z;
        }
    }

    Box = AABB(minX, minY, minZ, maxX, maxY, maxZ);
}

void Model::transformBoundingBox(Matrix m) {
    this->BoundingBox = this->boundingBox().transform(m);
}

void Model::loadNodes(const aiScene *pScene)
{
    deleteNodes(&RootNode);
    copyNodesRecursive(pScene->mRootNode, &RootNode);
}

void Model::copyNodesRecursive(const aiNode *paiNode, Node *pNode)
{
    pNode->Name = paiNode->mName.C_Str();
    pNode->Trans = convert(paiNode->mTransformation);

    if (paiNode->mNumMeshes > 0)
    {
        pNode->MeshCount = paiNode->mNumMeshes;
        pNode->Meshes = new int[pNode->MeshCount];
        for (unsigned int i = 0; i < pNode->MeshCount; ++i)
            pNode->Meshes[i] = (int)paiNode->mMeshes[i];
    }

    if (paiNode->mNumChildren <= 0)
        return;

    pNode->ChildCount = paiNode->mNumChildren;
    pNode->Children = new Node[pNode->ChildCount];
    for (unsigned int i = 0; i < paiNode->mNumChildren; ++i)
    {
        copyNodesRecursive(paiNode->mChildren[i], &(pNode->Children[i]));
        pNode->Children[i].Parent = pNode;
    }
}

void Model::applyMaterial(unsigned int index)
{
    if (index >= MaterialCount)
        return;

    PhongShader *pPhong = dynamic_cast<PhongShader *>(shader());
    if (!pPhong)
    {
        // std::cout << "Model::applyMaterial(): WARNING Invalid shader-type. Please apply PhongShader for rendering models.\n";
        return;
    }

    Material *pMat = &pMaterials[index];
    pPhong->ambientColor(pMat->AmbColor);
    pPhong->diffuseColor(pMat->DiffColor);
    pPhong->specularExp(pMat->SpecExp);
    pPhong->specularColor(pMat->SpecColor);
    pPhong->diffuseTexture(pMat->DiffTex);
    pPhong->normalTexture(pMat->NormalMapTex);
}

void Model::draw(const BaseCamera &Cam)
{
    if (!pShader)
    {
        std::cout << "BaseModel::draw() no shader found" << std::endl;
        return;
    }

    pShader->modelTransform(transform());

    std::list<Node *> DrawNodes;
    DrawNodes.push_back(&RootNode);

    while (!DrawNodes.empty())
    {
        Node *pNode = DrawNodes.front();
        Matrix GlobalTransform;

        if (pNode->Parent != NULL)
            pNode->GlobalTrans = pNode->Parent->GlobalTrans * pNode->Trans;
        else
            pNode->GlobalTrans = transform() * pNode->Trans;

        pShader->modelTransform(pNode->GlobalTrans);

        for (unsigned int i = 0; i < pNode->MeshCount; ++i)
        {
            Mesh &mesh = pMeshes[pNode->Meshes[i]];
            mesh.VB.activate();
            mesh.IB.activate();
            applyMaterial(mesh.MaterialIdx);
            pShader->activate(Cam);
            glDrawElements(GL_TRIANGLES, mesh.IB.indexCount(), mesh.IB.indexFormat(), 0);
            mesh.IB.deactivate();
            mesh.VB.deactivate();
        }

        for (unsigned int i = 0; i < pNode->ChildCount; ++i)
            DrawNodes.push_back(&(pNode->Children[i]));

        DrawNodes.pop_front();
    }
}

Matrix Model::convert(const aiMatrix4x4 &m)
{
    return Matrix(m.a1, m.a2, m.a3, m.a4,
                  m.b1, m.b2, m.b3, m.b4,
                  m.c1, m.c2, m.c3, m.c4,
                  m.d1, m.d2, m.d3, m.d4);
}
