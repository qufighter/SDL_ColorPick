#ifndef __MESHESMAN_HEADER__
#define __MESHESMAN_HEADER__

#include "main.h"
#include <iostream>
#include <fstream>
//using namespace std;
#include <vector>


// this is a utility used to read metadata about a particular vertex from files.. the setters are handlers bound to particular positions in PLY file
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


// this describes each position in the PLY format, and may be useful in other formats too
// the format described at the top is used througout the file... this allows the types specified in the header
// to be parsed, creating many VertexMeta objects (formerly we'd populate destArr directly but that did not allow for vertex re-use druing face4>triangle construction)
typedef struct TypedRead{


    typedef enum {
        INTEGER,
        FLOAT
    } READ_DATA_TYPES;

    TypedRead(char* label, const char* fmt){
        destLable = SDL_strdup(label);
        readFmt = SDL_strdup(fmt);
        //destArr = nullptr;
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
//        destArr = pdestArr;
//        destArrIdx = pdestArrIdx;
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
//        if( destArr!=nullptr ){ // currently disabled....
//            destArr[*destArrIdx] = aFloat;
//            (*destArrIdx)++;
//        }
    }
    char* destLable;
    char* readFmt;
    Uint8 readType;
    aFloatAssignmentFn floatValueAssigner;
    VertexMeta* vertexMeta;
//    float* destArr; // its always float no?
//    int* destArrIdx;
} TypedRead;


typedef struct Mesh
{
    Mesh(const char* pfilename){
        filename = pfilename;
        defaults();
    }

    void defaults(){
        color_additive = glm::vec4(0.0, 0.0, 0.0, 1.0);
        is_fully_loaded = false;
        file_loaded = false; // after we load the file, we clean up, so this will be false
        vertex_count=0;
        vIdx = 0;
        nIdx = 0;
        cIdx = 0;
        tIdx = 0;
        has_vertices=false;
        has_colors=false;
        has_normals=false;
        has_texCoords=false;
        vertices=nullptr;
        colors=nullptr;
        normals=nullptr;
        texCoords=nullptr;
        buffers[SHADER_POSITION] = 0;
        buffers[SHADER_COLOR] = 0;
        buffers[SHADER_TEXTURE] = 0;
        buffers[SHADER_NORMAL] = 0;

    }

    // we don't have the ref to shader, so this doesn't work.  some refactor maybe
//    Mesh* setShader(Shader* pshader){
//        shader = pshader;
//        return this;
//    }

    // we'd need to return uniformLocationStruct*.. sort of nasty
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


    void bindSpecific(bool enabled, GLuint SHADER_INDEX,  GLint size){
        if( enabled ){
            glBindBuffer(GL_ARRAY_BUFFER, this->buffers[SHADER_INDEX]); // Bind our second Vertex Buffer Object
            glVertexAttribPointer(/*(GLuint)*/SHADER_INDEX, size, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
            glEnableVertexAttribArray(SHADER_INDEX);
        }else{
            glDisableVertexAttribArray(SHADER_INDEX);
        }
    }

    void bind(){
#ifdef COLORPICK_OPENGL_ES2
        bindSpecific(has_vertices, SHADER_POSITION, 3);
        bindSpecific(has_colors, SHADER_COLOR, 3);
        bindSpecific(has_normals, SHADER_NORMAL, 3);
        bindSpecific(has_texCoords, SHADER_TEXTURE, 2);
#else
        glBindVertexArray(vertex_array[0]);

#endif
        // todo, alternate bindings ???
    }

    void render(){
        if( is_fully_loaded ){
            bind();
            //glBindVertexArray(vertex_array[0]);
            glDrawArrays(GL_TRIANGLES, 0, vertex_count);
        }else{
            // SDL_Log("Mesh not yet loaded... cannot render it!");
        }
    }

    // we could store a ref to the shader
    // we can store bindings related to material
    const char* filename;

    //Shader* shader;
    glm::vec4 color_additive; // MIGHT ALPHA BE OVERRIDE?

    //GLuint triangleStripIndexBuffer;
    unsigned int vertex_array[1];  // this really does NOT need to be an array... but its ok
    unsigned int buffers[4];
    unsigned int vertex_count;

    // we also need some temp variables to hold the arrays while we load... we can delete these later....
    float* vertices;
    float* colors;
    float* normals;
    float* texCoords;
    int vIdx;
    int nIdx;
    int cIdx;
    int tIdx;

    bool has_vertices;
    bool has_colors;
    bool has_normals;
    bool has_texCoords;

    bool file_loaded;
    bool is_fully_loaded;


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

    // shall we have a vector of our mesh's ?
    std::vector<Mesh*> allMeshes;

    void completeMeshLoading();

    static const int load_delay_ms = 30;
    bool hasLoadedToProcess;

    bool mesh3d_enabled=true;

private:
	//GLuint LoadTextureRAW( const char * filename, int wrap );
	



protected:

    //nada
};

#endif// __MESHESMAN_HEADER__
