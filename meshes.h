#ifndef __MESHESMAN_HEADER__
#define __MESHESMAN_HEADER__

#include "main.h"
#include <iostream>
#include <fstream>
//using namespace std;
#include <vector>



typedef struct VertexMeta{
    VertexMeta(){
        gotVertex = false;
        gotNormal = false;
        gotColor = false;
        gotUv = false;
    }

    bool gotVertex;
    glm::vec3 vertex;
    bool gotNormal;
    glm::vec3 normal;
    bool gotColor;
    glm::vec3 color;
    bool gotUv;
    glm::vec2 uv;

    static void setVertexX(VertexMeta* self, float i){
        self->vertex.x = i; self->gotVertex = true;
    }
    static void setVertexY(VertexMeta* self, float i){
        self->vertex.y = i; self->gotVertex = true;
    }
    static void setVertexZ(VertexMeta* self, float i){
        self->vertex.z = i; self->gotVertex = true;
    }

    static void setNormalX(VertexMeta* self, float i){
        self->normal.x = i; self->gotNormal = true;
    }
    static void setNormalY(VertexMeta* self, float i){
        self->normal.y = i; self->gotNormal = true;
    }
    static void setNormalZ(VertexMeta* self, float i){
        self->normal.z = i; self->gotNormal = true;
    }

    static void setColorR(VertexMeta* self, float i){
        self->color.x = i; self->gotColor = true;
    }
    static void setColorG(VertexMeta* self, float i){
        self->color.y = i; self->gotColor = true;
    }
    static void setColorB(VertexMeta* self, float i){
        self->color.z = i; self->gotColor = true;
    }

    static void setUvS(VertexMeta* self, float i){
        self->uv.x = i; self->gotUv = true;
    }
    static void setUvT(VertexMeta* self, float i){
        self->uv.y = i; self->gotUv = true;
    }

}VertexMeta;


typedef void (*aFloatAssignmentFn)(VertexMeta* self, float i);

typedef struct TypedRead{


    typedef enum {
        INTEGER,
        FLOAT
    } READ_DATA_TYPES;

    TypedRead(char* label, const char* fmt){
        destLable = SDL_strdup(label);
        readFmt = SDL_strdup(fmt);
        destArr = nullptr;
        floatValueAssigner=nullptr;
        if( SDL_strcmp(readFmt, "%f") == 0 ){
            readType = READ_DATA_TYPES::FLOAT;
        }else if( SDL_strcmp(readFmt, "%i") == 0 ){
            readType = READ_DATA_TYPES::INTEGER;
        }
    }
    void resolveSetter(aFloatAssignmentFn p){
        floatValueAssigner = p;
    }
    void resolveDest(float* pdestArr, int* pdestArrIdx){
        destArr = pdestArr;
        destArrIdx = pdestArrIdx;
    }
    void setVertexMeta(VertexMeta* vm){
        vertexMeta = vm;
    }
    void parseAndAdd(char* charData, long read_offset, VertexMeta* dataDest){
//        if( destArr==nullptr ){
//            //SDL_Log("processor for type %s %s unregistered!", readFmt, destLable);
//            return;
//        }
        float aFloat=0;
        int anInt;
        int status;
        // float read format....
        if( readType == READ_DATA_TYPES::FLOAT ){
            status = SDL_sscanf(&charData[read_offset], readFmt, &aFloat);
        }else if( readType == READ_DATA_TYPES::INTEGER ){
            status = SDL_sscanf(&charData[read_offset], readFmt, &anInt);
            aFloat = anInt / 255.0f;
        }
        if( floatValueAssigner != nullptr && dataDest != nullptr ){
            floatValueAssigner(dataDest, aFloat);
        }
        if( destArr!=nullptr ){
            destArr[*destArrIdx] = aFloat;
            (*destArrIdx)++;
        }
    }
    char* destLable;
    char* readFmt;
    Uint8 readType;
    aFloatAssignmentFn floatValueAssigner;
    VertexMeta* vertexMeta;
    float* destArr; // its always float no?
    int* destArrIdx;
} TypedRead;


typedef struct Mesh
{
    Mesh(){
        defaults();
    }

    void defaults(){
        color_additive = glm::vec4(0.0, 0.0, 0.0, 1.0);
        //shader=nullptr;
    }

//    Mesh* setShader(Shader* pshader){
//        shader = pshader;
//        return this;
//    }

//    void bindShader(){
//        if( shader!=nullptr ){
//            uniformLocationStruct* uniformLocations = shader->bind();
//            applyUniforms(uniformLocations);
//        }
//    }

    void applyUniforms(uniformLocationStruct *uniformLocations){
        glUniform4f(uniformLocations->color_additive,
                                        color_additive.r,
                                        color_additive.g,
                                        color_additive.b,
                                        color_additive.a);
    }

    // we could store a ref to the shader
    // we can store bindings related to material

    //Shader* shader;
    glm::vec4 color_additive; // MIGHT ALPHA BE OVERRIDE?

    //GLuint triangleStripIndexBuffer;
    unsigned int vertex_array[1];
    unsigned int buffers[4];
    unsigned int vertex_count;

} Mesh;


class Meshes {
protected:

    static bool ms_bInstanceCreated;
    static Meshes* pInstance;
public:
    static Meshes* Singleton();
	Meshes(void); // Default constructor
	~Meshes(void); // Destructor for cleaning up our application

    GLuint GenerateTexture();

    Mesh* LoadObjectPLY(const char* filename);
    Mesh* LoadObjectSTL(const char* filename);



private:
	//GLuint LoadTextureRAW( const char * filename, int wrap );
	



protected:

    //nada
};

#endif// __MESHESMAN_HEADER__
