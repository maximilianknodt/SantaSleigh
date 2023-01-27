#include "Terrain.h"
#include "rgbimage.h"
#include "Terrainshader.h"

Terrain::Terrain(const char* HeightMap, const char* DetailMap1, const char* DetailMap2) : Size(10,1,10)
{
    if(HeightMap && DetailMap1 && DetailMap2)
    {
        bool loaded = load( HeightMap, DetailMap1, DetailMap2);
        if(!loaded)
            throw std::exception();
    }
}

Terrain::~Terrain()
{
    
}

Vector Terrain::generateVector(const RGBImage* image, int x, int y) {
    Color c = image->getPixelColor(x, y);
    float yCor = this->height() * c.R;
    float xCor = (float) x / (float)image->width() * (float)width() - (float)width() / 2.0f; //max width
    float zCor = (float) y / (float)image->height() * (float)depth() - (float)depth() / 2.0f; //max depth
    return Vector(xCor, yCor, zCor);
}

bool Terrain::load( const char* HeightMap, const char* DetailMap1, const char* DetailMap2)
{
    if( !HeightTex.load(HeightMap) )
        return false;
    if( !DetailTex[0].load(DetailMap1) )
        return false;
    if( !DetailTex[1].load(DetailMap2) )
        return false;
    
    Texture terrain(HeightMap);
    const RGBImage* image = terrain.getRGBImage();
    
    // Excercise 2
    if(!MixTex.isValid()) {
        RGBImage result(image->width(), image->height());
        RGBImage::SoebelFilter(result, *image, 10.0f);
        MixTex.create(result);
        //result.saveToDisk("testimage.bmp");
    }
    
    VB.begin();
    for(int y = 0; y < image->height(); y++) {
        for(int x = 0; x < image->width(); x++) {
            Vector v = generateVector(image, x, y);
            
            std::vector<Vector> vertices;
            if(x > 0) vertices.push_back(generateVector(image, x - 1, y) - v); // a
            if(x > 0 && y > 0) vertices.push_back(generateVector(image, x - 1, y - 1) - v); // b
            if(y > 0) vertices.push_back(generateVector(image, x, y - 1) - v); // c
            if(x < image->width() - 1) vertices.push_back(generateVector(image, x + 1, y) - v); // d
            if(x < image->width() - 1 && y < image->height() - 1) vertices.push_back(generateVector(image, x + 1, y + 1) - v); // e
            if(y < image->height() - 1) vertices.push_back(generateVector(image, x, y + 1) - v); // f
            
            Vector normals(0.0f,0.0f,0.0f);
            for (int i = 1; i < vertices.size(); i++) {
                normals += vertices[i - 1].cross(vertices[i]);
            }
            normals += vertices[vertices.size() - 1].cross(vertices[0]);
            normals.normalize();
            
            VB.addNormal(normals*-1);
            VB.addTexcoord0(((float)x)/image->width(), ((float)y)/image->height());
            VB.addVertex(v);
        }
    }
    VB.end();
    
    IB.begin();
    for(int y = 0; y < image->height() - 1; y++) {
        for(int x = 0; x < image->width() - 1; x++) {
            unsigned int index = y + x * image->height();
            IB.addIndex(index);
            IB.addIndex(index + 1 + image->width());
            IB.addIndex(index + 1);
            
            IB.addIndex(index);
            IB.addIndex(index + image->width());
            IB.addIndex(index + image->width() + 1);
        }
    }
    IB.end();
    return true;
}

void Terrain::shader( BaseShader* shader, bool deleteOnDestruction )
{
    BaseModel::shader(shader, deleteOnDestruction);
}

void Terrain::draw(const BaseCamera& Cam)
{
    applyShaderParameter();
    BaseModel::draw(Cam);
    
    VB.activate();
    IB.activate();
    glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);
    IB.deactivate();
    VB.deactivate();
}

void Terrain::applyShaderParameter()
{
    TerrainShader* Shader = dynamic_cast<TerrainShader*>(BaseModel::shader());
    if(!Shader)
        return;
    
    Shader->mixTex(&MixTex);
    for(int i=0; i<2; i++)
        Shader->detailTex(i,&DetailTex[i]);
    Shader->scaling(Size);
    Shader->kFaktor(100);
}
