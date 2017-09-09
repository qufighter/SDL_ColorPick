#ifndef __SHADER_HEADER__
#define __SHADER_HEADER__

#include "main.h"


struct uniformLocationStruct
{
	GLint   globalLightLocation,
            ambientLightLoc,
            projectionMatrixLocation,
            viewMatrixLocation,
            modelMatrixLocation,
            textureSampler,
            textureSampler2,
            textureSampler3,
            ui_scale,
            ui_position,
            ui_color,
            ui_foreground_color,
            widthHeightRatio,
            fishScale,
            fishScalePct,
            textureCoord,
            texture_crop,
            normalLightingMat,
            outalpha,
            textOffset;
	uniformLocationStruct(){
		//constructor (set all to -1, however this occurs naturally)
	}
};

/**
	Shader is a class designed to allow us to load and use a GLSL shader program in
	our OpenGL application. It allows us to provide filenames for the vertex and 
	fragment shaders, and then creates the shader. It also lets us bind and 
	unbind the GLSL shader program as required.
*/
class Shader {
public:
    Shader(); // Default constructor
    Shader(const char *vsFile, const char *fsFile); // Constructor for creating a shader from two shader filenames
    ~Shader(); // Deconstructor for cleaning up
    
    void reload(); // Initialize our shader file if we have to
    
    uniformLocationStruct* bind(); // Bind our GLSL shader program
    void unbind(); // Unbind our GLSL shader program
    
    unsigned int id(); // Get the identifier for our program
    
	uniformLocationStruct* uniformLocations;

private:
    const char *vsFile;
    const char *fsFile;
    unsigned int shader_id; // The shader program identifier
    unsigned int shader_vp; // The vertex shader identifier
    unsigned int shader_fp; // The fragment shader identifier
    
    bool inited; // Whether or not we have initialized the shader
};

#endif// __SHADER_HEADER__