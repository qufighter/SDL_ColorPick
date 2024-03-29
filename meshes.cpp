#include "meshes.h"
//#include "ColorPick.h"
#include <string>

bool Meshes::ms_bInstanceCreated = false;
Meshes* Meshes::pInstance = NULL;

Meshes* Meshes::Singleton() {
    if(!ms_bInstanceCreated){
        pInstance = new Meshes();
        ms_bInstanceCreated=true;
    }
    return pInstance;
}

/**
Default constructor
*/
Meshes::Meshes(void) {

    hasLoadedToProcess = false;
    mesh3d_enabled=true;
    allMeshes.reserve(4); // maybe we will load 4 models someday!
}
/**
Destructor
*/
Meshes::~Meshes(void) {
	
}


GLuint Meshes::GenerateTexture(){
    GLuint textureid;
    glGenTextures(1, &textureid);
    return textureid;
}

#ifndef MAXFLOAT
#define MAXFLOAT    0x1.fffffep+127f
#endif

// make the following into member functions?
float readFloat32(SDL_RWops* fileref){
    Uint8* bytes = new Uint8[4];
    bytes[0] = SDL_ReadU8(fileref);
    bytes[1] = SDL_ReadU8(fileref);
    bytes[2] = SDL_ReadU8(fileref);
    bytes[3] = SDL_ReadU8(fileref);
    int sign = 1 - (2 * (bytes[3] >> 7));
    int exponent = (((bytes[3] << 1) & 0xff) | (bytes[2] >> 7)) - 127;
    int mantissa = ((bytes[2] & 0x7f) << 16) | (bytes[1] << 8) | bytes[0];
    if (exponent == 128) {
        if (mantissa != 0) {
            return 0; // NaN
        } else {
            return sign * MAXFLOAT;
        }
    }
    if (exponent == -127) { // Denormalized
        return sign * mantissa * SDL_powf(2, -126 - 23);
    }
    return sign * (1 + mantissa * SDL_powf(2, -23)) * SDL_powf(2, exponent);
}

glm::vec3 readVec3(SDL_RWops* fileref){
    float x = readFloat32(fileref);
    float y = readFloat32(fileref);
    return glm::vec3(x, y, readFloat32(fileref));
}

void logLoadingMessage(const char* filename){
    SDL_Log("\n========================================\n\n"\
            " -> Loading Mesh <- \n\n"
            " -> %s <- \n\n"
            "========================================\n\n", filename);
}


void buildMesh(Mesh* mesh, int vertex_items, float* vertices, float* normals, float* colors, float* texCoords){

#ifndef COLORPICK_OPENGL_ES2
    glGenVertexArrays(1, &mesh->vertex_array[0]); // Create our Vertex Array Object
    debugGLerror("mesh glGenVertexArrays");

    glBindVertexArray(mesh->vertex_array[0]); // Bind our Vertex Array Object so we can use it
    debugGLerror("mesh glBindVertexArray");
#endif
    //    if( glIsVertexArray(mesh->vertex_array[0]) != GL_TRUE ){
    //        SDL_Log("Um this is not logical...");
    //    }


    glGenBuffers(4, &mesh->buffers[0]); // Generate our two Vertex Buffer Object


    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[SHADER_POSITION]); // Bind our Vertex Buffer Object
    glBufferData(GL_ARRAY_BUFFER, vertex_items * sizeof(GLfloat), vertices, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
    glVertexAttribPointer((GLuint)SHADER_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
    glEnableVertexAttribArray(SHADER_POSITION); // Enable the first our Vertex Array Object
    mesh->has_vertices=true;

    // the following works... we have not been using colors in our shader though (we code the color valeus elsewhere), so lets save some GPU memory...
    if( colors != nullptr ){
        glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[SHADER_COLOR]); // Bind our second Vertex Buffer Object
        glBufferData(GL_ARRAY_BUFFER, vertex_items * sizeof(GLfloat), colors, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
        glVertexAttribPointer((GLuint)SHADER_COLOR, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
        glEnableVertexAttribArray(SHADER_COLOR); // Enable the second vertex attribute array
        mesh->has_colors=true;
    }

    // warning pertaining to mesh->has_colors: we have some issues with this on Linix test platforms... where even though we do not enable this,
    // for one reason or some other (possibly some bug in our code) we are getting non zero data for color attribute
    // since we don't use this feature or textures, maybe it does not matter and our current fix will suffice for now
    // but one option may be to pass the boolean values for mesh->has_colors and mesh->has_texCoords to the vtx shader... and onwards
    // to make the appropriate determination then....


    if( texCoords != nullptr ){
        glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[SHADER_TEXTURE]); // Bind our second Vertex Buffer Object
        glBufferData(GL_ARRAY_BUFFER, vertex_items * sizeof(GLfloat), texCoords, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
        glVertexAttribPointer((GLuint)SHADER_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
        glEnableVertexAttribArray(SHADER_TEXTURE); // Enable the second vertex attribute array
        mesh->has_texCoords=true;
    }

    glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[SHADER_NORMAL]); // Bind our second Vertex Buffer Object
    glBufferData(GL_ARRAY_BUFFER, vertex_items * sizeof(GLfloat), normals, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
    glVertexAttribPointer((GLuint)SHADER_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
    glEnableVertexAttribArray(SHADER_NORMAL); // Enable the second vertex attribute array
    mesh->has_normals=true;

#ifndef COLORPICK_OPENGL_ES2
    glBindVertexArray(0); // Disable our Vertex Buffer Object
#endif
    
    mesh->vertex_count = vertex_items / 3;  // 3 componenets (xyz) per vertex, so the count is....
    mesh->is_fully_loaded = true;
}

static Uint32 post_meshes_loaded_event(Uint32 interval, void* parm){
    SDL_Event event;
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = USER_EVENT_ENUM::MESH_FILE_READ;
    //    userevent.data1 = (void*)&pickerForPercentV;
    //    userevent.data2 = &self->lastPickPercent; // &percent; // waste arg
    event.type = SDL_USEREVENT;
    event.user = userevent;
    SDL_PushEvent(&event);
    return 0; // end timer
    //return interval;
}

static int LoadObjectPLYThread(void* data){
    Mesh* mesh = (Mesh*)data;
    SDL_Log("Thread Here: Mesh vert count and loaded status %s %i %i",mesh->filename,mesh->vertex_count,mesh->is_fully_loaded);

    logLoadingMessage(mesh->filename);
    char* charData = (char*)SDL_LoadFile(mesh->filename, NULL);

    // TODO maybe we don't need fileString and can just use the c string?
    std::string *fileString = new std::string();
    fileString->append(charData);

    long header_end = fileString->find("end_header");
    if( header_end < 0 ){ // its not a PLY file....
    }
    long data_start = header_end += 11;

    // who owns headerString ?
    std::string headerString = fileString->substr(0, data_start);

    int vertexCount=0;
    int faceCount=0;

    int retVal = 0;
    int anIntValue=0;

    char* char1 = (char*)SDL_malloc( sizeof(char) * 25 );
    char* char2 = (char*)SDL_malloc( sizeof(char) * 25 );

    //int retVal = SDL_sscanf(charData, "ply\nformat ascii %f\ncomment", &aValue);
    //    retVal = SDL_sscanf(&charData[1], "ly\nformat ascii %f\ncomment", &aFloatValue);
    //    SDL_Log("We got a floatly value %i %f ", retVal, aFloatValue);
    //
    ////%s would capture fileString
    ////%*s would suprress capture of that string...
    //
    //    retVal = SDL_sscanf(&charData[1], "%*s\nformat ascii %f\ncomment", &aFloatValue);
    //    SDL_Log("We got a floatly value %i %f ", retVal, aFloatValue);
    //    retVal = SDL_sscanf(&charData[headerString.find("format")], "format ascii %f\ncomment", &aFloatValue);
    //    SDL_Log("We got a floatly value %i %f ", retVal, aFloatValue);


    long next_element_start = headerString.find("element");
    long next_prop_start = headerString.find("property");

    bool elementVertex = false;
    bool elementFace = false;

    // if there is more than 12 elements for each vertex it will not work.
    TypedRead* reads = (TypedRead*)SDL_malloc( sizeof(TypedRead) * 12 );
    int reads_next_index = 0;

    while( next_element_start > -1 || next_prop_start > -1 ){

        if( next_prop_start > -1 && (next_prop_start < next_element_start || next_element_start < 0 ) ){

            // keep consuming properties....
            retVal = SDL_sscanf(&charData[next_prop_start], "property %s %s\n",char1,char2);
            //SDL_Log("Property inloop = We got a2 str value %i %s %s", retVal, char1, char2);
            next_prop_start = headerString.find("property", next_prop_start+1);

            if( elementVertex ){
                // we are in the vertex section, so lets consume those and build our format.....
                if( SDL_strcmp(char1, "float") == 0 ){
                    reads[reads_next_index++] = TypedRead(char2, "%f");
                }else if( SDL_strcmp(char1, "uchar") == 0 ){
                    reads[reads_next_index++] = TypedRead(char2, "%i");
                }
            }else if(elementFace){
                // arguably the property data here is not useful???
                // if its not "property list uchar uint vertex_indices" we should probably do something here...
                // so far we parsed "list uchar"
            }

        }else{
            // consume element...
            retVal = SDL_sscanf(&charData[next_element_start], "element %s %i\n", char1, &anIntValue);
            //SDL_Log("Element inloop: We got a int value %i %s %i ", retVal, char1, anIntValue);
            next_element_start = headerString.find("element", next_element_start+1);
            elementVertex = SDL_strcmp(char1, "vertex") == 0;
            elementFace = SDL_strcmp(char1, "face") == 0;
            if( elementVertex ){
                vertexCount = anIntValue;
            }else if(elementFace){
                faceCount = anIntValue;
            }

        }
    }


    //int numVerticies = vertexCount; // simply not true....
    int numVerticies = faceCount * 3;

    int vertex_items = numVerticies  * 3;
    int uv_items = numVerticies * 2;

    // we will over-allocate these arrays just in case we have face4 and need double...

    // its probably always ( faceCount * 3 ) then *2 to over alloc.... vertex_items is actually excessive alloc

    float* vertices = new float[vertex_items*2];
    float* colors = new float[vertex_items*2];
    float* normals = new float[vertex_items*2];
    float* texCoords = new float[uv_items*2];
    //    float *vertices = (float*)SDL_malloc( sizeof(float) * 18 );// new float[18];    // Vertices for our square

    int vIdx =0;
    int nIdx =0;
    int cIdx =0;
    int tIdx =0;

    // this loop should be pretty quick.... one loop... lots of checks though
    for( int read=0; read<reads_next_index; read++ ){
        if( SDL_strcmp(reads[read].destLable, "x") == 0 ){
            //reads[read].resolveDest(vertices, &vIdx);
            reads[read].resolveSetter(VertexMeta::setVertexX);
        }else if( SDL_strcmp(reads[read].destLable, "y") == 0 ){
            //reads[read].resolveDest(vertices, &vIdx);
            reads[read].resolveSetter(VertexMeta::setVertexY);
        }else if( SDL_strcmp(reads[read].destLable, "z") == 0 ){
            //reads[read].resolveDest(vertices, &vIdx);
            reads[read].resolveSetter(VertexMeta::setVertexZ);
        }else if( SDL_strcmp(reads[read].destLable, "nx") == 0 ){
            //reads[read].resolveDest(normals, &nIdx);
            reads[read].resolveSetter(VertexMeta::setNormalX);
        }else if( SDL_strcmp(reads[read].destLable, "ny") == 0 ){
            //reads[read].resolveDest(normals, &nIdx);
            reads[read].resolveSetter(VertexMeta::setNormalY);
        }else if( SDL_strcmp(reads[read].destLable, "nz") == 0 ){
            //reads[read].resolveDest(normals, &nIdx);
            reads[read].resolveSetter(VertexMeta::setNormalZ);
        }else if( SDL_strcmp(reads[read].destLable, "red") == 0 ){
            //reads[read].resolveDest(colors, &cIdx);
            reads[read].resolveSetter(VertexMeta::setColorR);
        }else if( SDL_strcmp(reads[read].destLable, "green") == 0 ){
            //reads[read].resolveDest(colors, &cIdx);
            reads[read].resolveSetter(VertexMeta::setColorG);
        }else if( SDL_strcmp(reads[read].destLable, "blue") == 0 ){
            //reads[read].resolveDest(colors, &cIdx);
            reads[read].resolveSetter(VertexMeta::setColorB);
        }else if( SDL_strcmp(reads[read].destLable, "s") == 0 ){
            //reads[read].resolveDest(texCoords, &tIdx);
            reads[read].resolveSetter(VertexMeta::setUvS);
        }else if( SDL_strcmp(reads[read].destLable, "t") == 0 ){
            //reads[read].resolveDest(texCoords, &tIdx);
            reads[read].resolveSetter(VertexMeta::setUvT);
        }else{
            SDL_Log("WARNING: unhandled label %s", reads[read].destLable);
        }
    }


    VertexMeta* vertexMetadata = (VertexMeta*)SDL_malloc( sizeof(VertexMeta) * vertexCount );

    // TODO WHEN face4 indicies we can't flat add these... each 4 need to be added 2x to form 2 triangles...
    // we need to parseAndAdd the row (vertex) to another list of verticies from which we will finally ....
    // we DID parse faceCount
    // we still wont know if its face3 or face4 though until after this loop...

    long next_data_start = data_start;
    for( int row=0; row<vertexCount; row++ ){
        long last_row_position_start=next_data_start;
        long row_position_start=next_data_start;
        vertexMetadata[row] = VertexMeta(); // make sure "constructor" is called?

        for( int read=0; read<reads_next_index; read++ ){

            reads[read].parseAndAdd(charData, row_position_start, &vertexMetadata[row]);
            last_row_position_start = row_position_start;

            //if(  read<reads_next_index -1 )
            row_position_start = fileString->find(" ", row_position_start+1);
        }
        next_data_start = fileString->find("\n", last_row_position_start+1);
    }

    VertexMeta* v[4];

    for( int row=0; row<faceCount; row++ ){
        long last_row_position_start=next_data_start;
        long row_position_start=next_data_start;
        // we are processing faces now....
        // the first int is the face count...

        retVal = SDL_sscanf(&charData[row_position_start], "%i", &anIntValue);
        //SDL_Log("reading faces, we got face count... %i %i", retVal, anIntValue);
        row_position_start = fileString->find(" ", row_position_start+1);
        int faces = anIntValue;

        for( int face=0; face<faces; face++ ){
            retVal = SDL_sscanf(&charData[row_position_start], "%i", &anIntValue);
            //SDL_Log("reading face, we got int... %i %i", retVal, anIntValue);
            v[face] = &vertexMetadata[anIntValue];
            last_row_position_start = row_position_start;
            //if( face<faces -1 )
            row_position_start = fileString->find(" ", row_position_start+1);
        }

        if( faces >= 3 ){
            // we push one triangle....

            //if( v[0]->gotVertex ){ // ply always has vertex....
            vertices[vIdx++]=v[0]->vertex.x;
            vertices[vIdx++]=v[0]->vertex.y;
            vertices[vIdx++]=v[0]->vertex.z;
            vertices[vIdx++]=v[1]->vertex.x;
            vertices[vIdx++]=v[1]->vertex.y;
            vertices[vIdx++]=v[1]->vertex.z;
            vertices[vIdx++]=v[2]->vertex.x;
            vertices[vIdx++]=v[2]->vertex.y;
            vertices[vIdx++]=v[2]->vertex.z;
            //}

            if( v[0]->gotNormal ){
                normals[nIdx++]=v[0]->normal.x;
                normals[nIdx++]=v[0]->normal.y;
                normals[nIdx++]=v[0]->normal.z;
                normals[nIdx++]=v[1]->normal.x;
                normals[nIdx++]=v[1]->normal.y;
                normals[nIdx++]=v[1]->normal.z;
                normals[nIdx++]=v[2]->normal.x;
                normals[nIdx++]=v[2]->normal.y;
                normals[nIdx++]=v[2]->normal.z;
            }

            if( v[0]->gotColor ){
                colors[cIdx++]=v[0]->color.x;
                colors[cIdx++]=v[0]->color.y;
                colors[cIdx++]=v[0]->color.z;
                colors[cIdx++]=v[1]->color.x;
                colors[cIdx++]=v[1]->color.y;
                colors[cIdx++]=v[1]->color.z;
                colors[cIdx++]=v[2]->color.x;
                colors[cIdx++]=v[2]->color.y;
                colors[cIdx++]=v[2]->color.z;
            }

            if( v[0]->gotUv ){
                texCoords[tIdx++]=v[0]->uv.x;
                texCoords[tIdx++]=v[0]->uv.y;
                texCoords[tIdx++]=v[1]->uv.x;
                texCoords[tIdx++]=v[1]->uv.y;
                texCoords[tIdx++]=v[2]->uv.x;
                texCoords[tIdx++]=v[2]->uv.y;
            }
        }

        if( faces == 4 ){
            // push 2 triangles to make face complete.....
            // we need to resize our allocations :/ (we doubled them already)

            // p2 (second triangle)
            //if( v[0]->gotVertex ){ // ply always has vertex....
            vertices[vIdx++]=v[0]->vertex.x;
            vertices[vIdx++]=v[0]->vertex.y;
            vertices[vIdx++]=v[0]->vertex.z;
            vertices[vIdx++]=v[2]->vertex.x;
            vertices[vIdx++]=v[2]->vertex.y;
            vertices[vIdx++]=v[2]->vertex.z;
            vertices[vIdx++]=v[3]->vertex.x;
            vertices[vIdx++]=v[3]->vertex.y;
            vertices[vIdx++]=v[3]->vertex.z;
            //}
            if( v[0]->gotNormal ){
                normals[nIdx++]=v[0]->normal.x;
                normals[nIdx++]=v[0]->normal.y;
                normals[nIdx++]=v[0]->normal.z;
                normals[nIdx++]=v[2]->normal.x;
                normals[nIdx++]=v[2]->normal.y;
                normals[nIdx++]=v[2]->normal.z;
                normals[nIdx++]=v[3]->normal.x;
                normals[nIdx++]=v[3]->normal.y;
                normals[nIdx++]=v[3]->normal.z;
            }
            if( v[0]->gotColor ){
                colors[cIdx++]=v[0]->color.x;
                colors[cIdx++]=v[0]->color.y;
                colors[cIdx++]=v[0]->color.z;
                colors[cIdx++]=v[2]->color.x;
                colors[cIdx++]=v[2]->color.y;
                colors[cIdx++]=v[2]->color.z;
                colors[cIdx++]=v[3]->color.x;
                colors[cIdx++]=v[3]->color.y;
                colors[cIdx++]=v[3]->color.z;
            }
            if( v[0]->gotUv ){
                texCoords[tIdx++]=v[0]->uv.x;
                texCoords[tIdx++]=v[0]->uv.y;
                texCoords[tIdx++]=v[2]->uv.x;
                texCoords[tIdx++]=v[2]->uv.y;
                texCoords[tIdx++]=v[3]->uv.x;
                texCoords[tIdx++]=v[3]->uv.y;
            }
        }


        next_data_start = fileString->find("\n", last_row_position_start+1);
    }

    mesh->vertices = vertices;
    mesh->colors = colors;
    mesh->normals = normals;
    mesh->texCoords = texCoords;
    mesh->vIdx = vIdx;
    mesh->nIdx = nIdx;
    mesh->cIdx = cIdx;
    mesh->tIdx = tIdx;

    // we can have combined face4 and face3.....
    //    if( isFace4 ){
    //        vertex_items *= 2; // too simplistic??? depends on how its initally set though... would be ok now
    //       // vertex_items = (( faceCount * 3 ) * 3) * 2; // we could always run this....
    //    }

    SDL_free(charData);
    SDL_free(char1);
    SDL_free(char2);

    //SDL_strstr(const char *haystack, <#const char *needle#>)

    fileString->clear(); // is tghe string used?
	fileString->shrink_to_fit();

    headerString.clear(); // not really free?

    SDL_free(reads);
    SDL_free(vertexMetadata);

    mesh->file_loaded=true;
    Meshes::Singleton()->hasLoadedToProcess = true;

    //Meshes::Singleton()->completeMeshLoading(); // on a timer ????
    int my_timer_id = SDL_AddTimer(Meshes::load_delay_ms, post_meshes_loaded_event, (void*)mesh);

    return 0;
}





static int LoadObjectSTLThread(void* data){
    Mesh* mesh = (Mesh*)data;

    SDL_Log("Thread Here: Mesh vert count and loaded status %s %i %i",mesh->filename,mesh->vertex_count,mesh->is_fully_loaded);

    logLoadingMessage(mesh->filename);

    SDL_RWops* fileref = SDL_RWFromFile(mesh->filename, "r");
    if( fileref == NULL ){
        SDL_Log("Error: fle not read: %s", mesh->filename);
        return 1;
    }

    Sint64 filesize = SDL_RWsize(fileref); // this filesize is in ? units... not a count of u8? (actually it seems to count u8s)
    int eachElementSize = sizeof(Sint32); // 4 bytes

    // padding 80 bytes
    int currentPosition = 0;
    while( currentPosition < 80 && currentPosition < filesize ){
        SDL_ReadLE32(fileref);
        currentPosition+=eachElementSize;
    }

    int numFaces = 0;
    while( currentPosition < 81 && currentPosition < filesize ){
        numFaces = SDL_ReadLE32(fileref);
        currentPosition+=eachElementSize;
    }

    // time to allocate things.... we know our faces now :)
    // and we have face normals - which are in fact USELESS for lighing right?

    int numVerticies = numFaces * 3;

    int vertex_items = numVerticies * 3;
    //int normal_items = numFaces * 3;

    float* vertices = new float[vertex_items];
    float* colors = new float[vertex_items];
    float* normals = new float[vertex_items];
    //float* texCoords = new float[uv_items*2];


    int vIdx =0;
    int nIdx =0;
    int cIdx =0;

    //int triangeCount = 0;

    while( currentPosition < filesize ){

        glm::vec3 normal = readVec3(fileref);
        glm::vec3 p1 = readVec3(fileref);
        glm::vec3 p2 = readVec3(fileref);
        glm::vec3 p3 = readVec3(fileref);
        Sint16 byteCount = SDL_ReadLE16(fileref); // is this "extra bytes count" ?

        vertices[vIdx++]=p1.x;
        vertices[vIdx++]=p1.y;
        vertices[vIdx++]=p1.z;
        vertices[vIdx++]=p2.x;
        vertices[vIdx++]=p2.y;
        vertices[vIdx++]=p2.z;
        vertices[vIdx++]=p3.x;
        vertices[vIdx++]=p3.y;
        vertices[vIdx++]=p3.z;

        //        glm::vec3 computedNormal = glm::triangleNormal(p1,p2,p3);
        ////        normal *= -1;
        //
        //        if( normal != computedNormal ){
        //
        //            SDL_Log("Normal mismatch...");
        //        }

        //repeats!
        //        normals[nIdx++]=p1.x+normal.x;
        //        normals[nIdx++]=p1.x+normal.y;
        //        normals[nIdx++]=p1.z+normal.z;
        //        normals[nIdx++]=p2.x+normal.x;
        //        normals[nIdx++]=p2.y+normal.y;
        //        normals[nIdx++]=p2.z+normal.z;
        //        normals[nIdx++]=p3.x+normal.x;
        //        normals[nIdx++]=p3.y+normal.y;
        //        normals[nIdx++]=p3.z+normal.z;

        //

        //face normals are flat?
        normals[nIdx++]=normal.x;
        normals[nIdx++]=normal.y;
        normals[nIdx++]=normal.z;
        normals[nIdx++]=normal.x;
        normals[nIdx++]=normal.y;
        normals[nIdx++]=normal.z;
        normals[nIdx++]=normal.x;
        normals[nIdx++]=normal.y;
        normals[nIdx++]=normal.z;



        colors[cIdx++]=0;
        colors[cIdx++]=0;
        colors[cIdx++]=0;
        colors[cIdx++]=0;
        colors[cIdx++]=0;
        colors[cIdx++]=0;
        colors[cIdx++]=0;
        colors[cIdx++]=0;
        colors[cIdx++]=0;

        //        colors[cIdx++]=((rand() % 100) / 100.0f);
        //        colors[cIdx++]=((rand() % 100) / 100.0f);
        //        colors[cIdx++]=((rand() % 100) / 100.0f);
        //        colors[cIdx++]=((rand() % 100) / 100.0f);
        //        colors[cIdx++]=((rand() % 100) / 100.0f);
        //        colors[cIdx++]=((rand() % 100) / 100.0f);
        //        colors[cIdx++]=((rand() % 100) / 100.0f);
        //        colors[cIdx++]=((rand() % 100) / 100.0f);
        //        colors[cIdx++]=((rand() % 100) / 100.0f);

        //        colors[cIdx++]=((triangeCount % 255) / 255.0f);
        //        colors[cIdx++]=((triangeCount % 255) / 255.0f);
        //        colors[cIdx++]=((triangeCount % 255) / 255.0f);
        //        colors[cIdx++]=((triangeCount % 255) / 255.0f);
        //        colors[cIdx++]=((triangeCount % 255) / 255.0f);
        //        colors[cIdx++]=((triangeCount % 255) / 255.0f);
        //        colors[cIdx++]=((triangeCount % 255) / 255.0f);
        //        colors[cIdx++]=((triangeCount % 255) / 255.0f);
        //        colors[cIdx++]=((triangeCount % 255) / 255.0f);
        //        triangeCount++;

        //        SDL_Log("We have a triangle here:\n\tnormal: %f %f %f \n\tp1: %f %f %f \n\tp2: %f %f %f \n\tp3: %f %f %f \n bytes: %i",
        //                    normal.x,
        //                    normal.y,
        //                    normal.z,
        //                    p1.x   ,
        //                    p1.y   ,
        //                    p1.z   ,
        //                    p2.x   ,
        //                    p2.y   ,
        //                    p2.z   ,
        //                    p3.x   ,
        //                    p3.y   ,
        //                    p3.z   ,
        //                    byteCount
        //                );


        currentPosition += 12 * eachElementSize;
        currentPosition += sizeof(Sint16); // byteCount
    }
    SDL_RWclose(fileref);

    mesh->vertices = vertices;
    mesh->colors = colors;
    mesh->normals = normals;
    //mesh->texCoords = nullptr;
    mesh->vIdx = vIdx;
    mesh->nIdx = nIdx;
    mesh->cIdx = cIdx;
    //mesh->tIdx = tIdx;

    mesh->file_loaded=true;
    Meshes::Singleton()->hasLoadedToProcess = true;

    //Meshes::Singleton()->completeMeshLoading(); // on a timer ????
    int my_timer_id = SDL_AddTimer(Meshes::load_delay_ms, post_meshes_loaded_event, (void*)mesh);

    return 0;
}


void Meshes::completeMeshLoading(){

    // todo: we could check a bool hasLoadedToProcess first.... otherwise we may end up here 3x

    if( !hasLoadedToProcess ) return;
    hasLoadedToProcess = false;
    for( std::vector<Mesh*>::iterator i=allMeshes.begin(); i!=allMeshes.end(); ++i){
        Mesh* mesh = (*i);

        if( mesh->file_loaded && !mesh->is_fully_loaded ){
            mesh->file_loaded = false; // lets try to only get here ONCE for any given mesh...

                // omg um, why not just pass in mesh? ? ? ?

                buildMesh(mesh, mesh->vIdx,
                          mesh->vertices,
                          mesh->normals,
                          mesh->cIdx > 0 ? mesh->colors : nullptr,
                          mesh->tIdx > 0 ? mesh->texCoords: nullptr);


            if( mesh->vertices != nullptr ){ delete [] mesh->vertices;} // Delete our vertices from memory
            if( mesh->normals != nullptr  ){ delete [] mesh->normals;}
            if( mesh->colors != nullptr   ){ delete [] mesh->colors;}
            if( mesh->texCoords != nullptr){ delete [] mesh->texCoords;}
        }

        SDL_Log("Mesh vert count and loaded status %s %i %i",mesh->filename,mesh->vertex_count,mesh->is_fully_loaded);
    }
}


Mesh* Meshes::LoadObjectPLY(const char* filename) {
    Mesh* mesh = new Mesh(filename);
    allMeshes.push_back(mesh);

#ifndef NO_MESH_LOADING
    if( mesh3d_enabled ){
        SDL_Thread *thread;
        thread = SDL_CreateThread(LoadObjectPLYThread, "MeshLoadPLY", (void *)mesh);
        SDL_DetachThread(thread);
        if (NULL == thread) {
            SDL_Log("Mesh Loading: SDL_CreateThread failed (falling back to non threaded loading...): %s\n", SDL_GetError());
            LoadObjectPLYThread((void *)mesh);
            completeMeshLoading();
            SDL_Log("non threaded loading done...");
        }
    }
#endif


    //    buildMesh(mesh, vIdx,
    //              vertices,
    //              normals,
    //              cIdx > 0 ? colors : nullptr,
    //              tIdx > 0 ? texCoords: nullptr);
    //
    //
    //
    //    delete [] vertices; // Delete our vertices from memory
    //    delete [] normals;
    //    delete [] colors;
    //    delete [] texCoords;

    return mesh;
}


Mesh* Meshes::LoadObjectSTL(const char* filename) {
    // insipired by STLLoader.js - this is by no means a complete STL loader...
    // also STL doesn't support UVs... (what good is that?)
    // the normals seem to be broken, though they pretty much match what glm computes
    Mesh* mesh = new Mesh(filename);
    allMeshes.push_back(mesh);

    SDL_Thread *thread;
    thread = SDL_CreateThread(LoadObjectSTLThread, "MeshLoadSTL", (void *)mesh);
    SDL_DetachThread(thread);
    if (NULL == thread) {
        printf("Mesh Loading: SDL_CreateThread failed: %s\n", SDL_GetError());
    }

    //    buildMesh(mesh, vertex_items, vertices, normals, colors, nullptr);
    //                                                    //colors
    //
    //    delete [] vertices; // Delete our vertices from memory
    //    delete [] normals;
    //    delete [] colors;

    return mesh;
}
