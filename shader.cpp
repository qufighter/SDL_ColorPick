#include "shader.h"
#include <string>
#include <fstream>

#include "ColorPick.h"


using namespace std; // Include the standard namespace

/**
	textFileRead loads in a standard text file from a given filename and
	then returns it as a string.
*/
static string textFileRead(const char *fileName) {

    // MAYBE use sdl functions for dealing with files?


	string fileString = string(); // A string for storing the file contents
//    string line = string(); // A string for holding the current line

//    ifstream file(fileName); // Open an input stream with the selected file

    fileString.append("#version 100\n");

    // IF its an FSH and we are on IOS we should prepend the amazing
    // precision mediump float;
    // which is requird by ios in at least some cases but banned on osx
    // cannot fathom it but highp works and is in fact required by our shader.... in order for 2048 size to "work right" on true device.... mediump leaves us off by a whole pixel!
//#ifndef COLORPICK_PLATFORM_DESKTOP
    //if( SHD_TEXTURE_SIZE > 1024 ){
        // todo veerify platform really supports this size?

//#ifdef __ANDROID__
//    fileString.append("precision mediump float;\n");
//#else
        fileString.append("precision highp float;\n");
//#endif

    //}else{
     //   fileString.append("precision mediump float;\n");
    //}
//#endif

//    if (file.is_open()) { // If the file opened successfully
//        while (!file.eof()) { // While we are not at the end of the file
//            getline(file, line); // Get the current line
//              fileString.append(line); // Append the line to our file string
//            fileString.append("\n"); // Appand a new line character
//        }
//        file.close(); // Close the file
//    }

    char* data = (char*)SDL_LoadFile(fileName, NULL);

    fileString.append(data);

    SDL_free(data);

    return fileString; // Return our string
}

static void messageBoxTime(const char* file, char* buffer){
    OpenGLContext* ogg=OpenGLContext::Singleton();
    if( ogg->no_more_shader_message_boxes ){
        return;
    }

    const unsigned int REPORT_SIZE = 512;
    char* shader_error_report = (char*)SDL_malloc( sizeof(char) * REPORT_SIZE );

    SDL_snprintf(shader_error_report, REPORT_SIZE, "%s\n%s", file, buffer);

    int selected;
    SDL_MessageBoxData messagebox;
    SDL_MessageBoxButtonData buttons[] = {
        {   0,  SDL_ASSERTION_RETRY,            "Continue" },
        {   SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, SDL_ASSERTION_IGNORE,           "Ignore All" },
        {   SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,  SDL_ASSERTION_BREAK,            "Report" },
        {   0,  SDL_ASSERTION_ABORT,            "Quit" }
    };

    SDL_zero(messagebox);
    messagebox.flags = SDL_MESSAGEBOX_WARNING;
    messagebox.window = ogg->sdlWindow;
    messagebox.title = "Sorry - Shader Compile Issue on Device";
    messagebox.message = shader_error_report;// "test message1";
    messagebox.numbuttons = SDL_arraysize(buttons);
    messagebox.buttons = buttons;
    if (SDL_ShowMessageBox(&messagebox, &selected) == 0) {
        //SDL_Log("--- itz %d", selected);

        if( selected == SDL_ASSERTION_ABORT ){
            // quit...
            SDL_free(shader_error_report);
            SDL_Quit();
            exit(1);
            return;
        }else if(selected == SDL_ASSERTION_BREAK ){
            // report
            const char* urlBase = "http://www.vidsbee.com/Contact/?browserinfo=App:NativeColorPick";
            SDL_snprintf(shader_error_report, REPORT_SIZE, "%s:%s\n%s", urlBase, file, buffer);
            ogg->doOpenURL(shader_error_report); // TODO: we really need to defer this....
        }else if(selected == SDL_ASSERTION_IGNORE ){
            ogg->no_more_shader_message_boxes=true;
        }else{
            // shrug SDL_ASSERTION_RETRY
        }
    }

    SDL_free(shader_error_report);
}

/**
	Given a shader and the filename associated with it, validateShader will
	then get information from OpenGl on whether or not the shader was compiled successfully
	and if it wasn't, it will output the file with the problem, as well as the problem.
*/
static void validateShader(GLuint shader, const char* file = 0) {
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;
    
    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the shader
    if (length > 0 ){
        cout << "Shader " << shader << " (" << (file?file:"") << ") compile error: " << buffer << endl; // Output the information
        if( buffer[0] != 'S' && buffer[0] != 'C' ){ // If we have any information to display, some devices report "Success.\nWARNING:" others start with "Complete" - known error reports may start with E
            messageBoxTime(file, buffer);
        }
    }
}

/**
	Given a shader program, validateProgram will request from OpenGL, any information
	related to the validation or linking of the program with it's attached shaders. It will
	then output any issues that have occurred.
*/
static void validateProgram(GLuint program) {
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;
    
    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the program
	if (length > 0){ // If we have any information to display
//#ifdef COLORPICK_DEBUG_MODE
//        cout << "Program " << program << " link error: " << buffer << endl; // Output the information
//#endif
        SDL_Log("Program %i link error: %s", program, buffer);
		//DebugMessage((buffer));
	}
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == 0){
        SDL_Log("LINK STATUS is 0, so this probably failed to link completely");
    }

    glValidateProgram(program); // Get OpenGL to try validating the program

    glGetProgramiv(program, GL_VALIDATE_STATUS, &status); // Find out if the shader program validated correctly
    if (status == GL_FALSE){ // If there was a problem validating
//#ifdef COLORPICK_DEBUG_MODE
		cout << "Error validating shader " << program << endl; // Output which program had the error
//#endif
        GLchar errorLog[1024] = {0};
        glGetProgramInfoLog(program, 1024, NULL, errorLog);
        SDL_Log("Details: %s", errorLog);
    }
}

/**
	Default constructor for the Shader class, at the moment it does nothing
*/
Shader::Shader() {
    
}

/**
	Constructor for a Shader object which creates a GLSL shader based on a given
	vertex and fragment shader file.
*/
Shader::Shader(const char *PvsFile, const char *PfsFile) {
    //if (inited) // If we have already initialized the shader
    //    return;
    
    //inited = false; // Declare we have not initialized the shader yet
    vsFile = PvsFile;
    fsFile = PfsFile;
    
    shader_vp = glCreateShader(GL_VERTEX_SHADER); // Create a vertex shader
    shader_fp = glCreateShader(GL_FRAGMENT_SHADER); // Create a fragment shader
    shader_id = glCreateProgram(); // Create a GLSL program
    SDL_Log("Program:%d %s %s", shader_id, vsFile, fsFile);

    reload(); // Initialize the shader
}
/**
	init will take a vertex shader file and fragment shader file, and then attempt to create a valid
	shader program from these. It will also check for any shader compilation issues along the way.
*/
void Shader::reload() {


    //inited = true; // Mark that we have initialized the shader

    
    string vsText = textFileRead(vsFile); // Read in the vertex shader
    string fsText = textFileRead(fsFile); // Read in the fragment shader

//    SDL_Log("SHADER FILE CONTENTS (first blob %s) %s", vsFile, vsText.c_str());
//    SDL_Log("SHADER FILE CONTENTS (second blob %s) %s", fsFile, fsText.c_str());


    const char *vertexText = vsText.c_str();
	const char *fragmentText = fsText.c_str();

    if (vertexText == NULL || fragmentText == NULL) { // If either the vertex or fragment shader wouldn't load
        cout << "Either vertex shader or fragment shader file not found." << endl; // Output the error
        return;
    }

//    GLint vsLen = (GLint)vsText.length();
//    GLint fsLen = (GLint)fsText.length();

    GLint vsLen = (GLint)SDL_strlen(vertexText);
    GLint fsLen = (GLint)SDL_strlen(fragmentText);

    glShaderSource(shader_vp, 1, &vertexText, &vsLen); // Set the source for the vertex shader to the loaded text
    glCompileShader(shader_vp); // Compile the vertex shader
    validateShader(shader_vp, vsFile); // Validate the vertex shader
    
    glShaderSource(shader_fp, 1, &fragmentText, &fsLen); // Set the source for the fragment shader to the loaded text
    glCompileShader(shader_fp); // Compile the fragment shader
    validateShader(shader_fp, fsFile); // Validate the fragment shader
    
	glAttachShader(shader_id, shader_vp); // Attach a vertex shader to the program
    glAttachShader(shader_id, shader_fp); // Attach the fragment shader to the program


    glBindAttribLocation(shader_id, SHADER_POSITION, "position");
    glBindAttribLocation(shader_id, SHADER_COLOR, "color");
    glBindAttribLocation(shader_id, SHADER_TEXTURE, "TexCoordIn");
    glBindAttribLocation(shader_id, SHADER_NORMAL, "normal");


//    glBindAttribLocation(shader_id, ATTRIB_VERTEX, "position");
//    //glBindAttribLocation(program, ATTRIB_COLOR, "color");
//    glBindAttribLocation(shader_id, ATTRIB_TEXCOORD, "TexCoordIn");

//	glBindAttribLocation(shader_id, SHADER_POSITION, "in_Position"); // Bind a constant attribute location for positions of vertices
//	glBindAttribLocation(shader_id, SHADER_COLOR, "in_Color"); // Bind another constant attribute location, this time for color
//	glBindAttribLocation(shader_id, SHADER_TEXTURE, "in_Texture"); // Bind another constant attribute location, this time for color
//	glBindAttribLocation(shader_id, SHADER_NORMAL, "in_Normal"); // Bind another constant attribute location, this time for color

    glLinkProgram(shader_id); // Link the vertex and fragment shaders in the program
    validateProgram(shader_id); // Validate the shader program

	uniformLocations=new uniformLocationStruct();
	//get the uniform locations from the program
//	uniformLocations->globalLightLocation =			glGetUniformLocation(shader_id, "globalLight"); // Get the location of our projection matrix in the shader
	uniformLocations->projectionMatrixLocation =	glGetUniformLocation(shader_id, "projectionMatrix"); // Get the location of our projection matrix in the shader
	uniformLocations->viewMatrixLocation =			glGetUniformLocation(shader_id, "viewMatrix"); // Get the location of our view matrix in the shader
	uniformLocations->modelMatrixLocation =			glGetUniformLocation(shader_id, "modelMatrix"); // Get the location of our model matrix in the shader
    uniformLocations->ui_modelMatrix =            glGetUniformLocation(shader_id, "ui_modelMatrix"); // Get the location of our model matrix in the shader
//	uniformLocations->textureSampler =				glGetUniformLocation(shader_id, "my_texture"); // Get the location of our model matrix in the shader
//	uniformLocations->textureSampler2 =				glGetUniformLocation(shader_id, "my_texture2"); // Get the location of our model matrix in the shader
//	uniformLocations->textureSampler3 =				glGetUniformLocation(shader_id, "my_texture3"); // Get the location of our model matrix in the shader
//	uniformLocations->outalpha =					glGetUniformLocation(shader_id, "output_alpha"); // Get the location of our model matrix in the shader
//	uniformLocations->ambientLightLoc =				glGetUniformLocation(shader_id, "ambient_light"); // Get the location of our model matrix in the shader
//	uniformLocations->textOffset =					glGetUniformLocation(shader_id, "letter_offset"); // Get the location of our model matrix in the shader

    uniformLocations->globalAmbientLight = glGetUniformLocation(shader_id, "global_ambient");

    uniformLocations->widthHeightRatio = glGetUniformLocation(shader_id, "widthHeightRatio");


    uniformLocations->ui_scale = glGetUniformLocation(shader_id, "ui_scale");
    uniformLocations->ui_position = glGetUniformLocation(shader_id, "ui_position");
    uniformLocations->ui_color = glGetUniformLocation(shader_id, "ui_color");
    uniformLocations->ui_foreground_color = glGetUniformLocation(shader_id, "ui_foreground_color");
    uniformLocations->texture_crop = glGetUniformLocation(shader_id, "texture_crop");
    uniformLocations->ui_crop = glGetUniformLocation(shader_id, "ui_crop");
    uniformLocations->ui_crop2 = glGetUniformLocation(shader_id, "ui_crop2");
    uniformLocations->ui_corner_radius = glGetUniformLocation(shader_id, "ui_corner_radius");

    uniformLocations->color_additive = glGetUniformLocation(shader_id, "color_additive");

    uniformLocations->modelMatrixLocation = glGetUniformLocation(shader_id, "modelMatrix");
    uniformLocations->viewMatrixLocation = glGetUniformLocation(shader_id, "viewMatrix");
    uniformLocations->projectionMatrixLocation = glGetUniformLocation(shader_id, "projectionMatrix");

    uniformLocations->fishScale = glGetUniformLocation(shader_id, "fishEyeScale");
    uniformLocations->fishScalePct = glGetUniformLocation(shader_id, "fishEyeScalePercent");
    uniformLocations->textureWidth = glGetUniformLocation(shader_id, "textureWidth");

    uniformLocations->textureSampler = glGetUniformLocation(shader_id, "texture1");
    uniformLocations->textureSampler2 = glGetUniformLocation(shader_id, "texture2");
    uniformLocations->textureSampler3 = glGetUniformLocation(shader_id, "texture3");
    // = glGetUniformLocation(_program, "TexCoordIn");
 //????   uniformLocations->textureCoord = glGetAttribLocation(shader_id, "TexCoordIn");
    uniformLocations->positionOffset = glGetUniformLocation(shader_id, "positionOffset");
    uniformLocations->zoomedPositionOffset = glGetUniformLocation(shader_id, "zoomedPositionOffset");

    uniformLocations->normalLightingMat = glGetAttribLocation(shader_id, "lightingMat");


    // not exactly free ?
    vsText.clear();
    fsText.clear();

}

/**
	Deconstructor for the Shader object which cleans up by detaching the shaders, deleting them
	and finally deleting the GLSL program.
*/
Shader::~Shader() {
	glDetachShader(shader_id, shader_fp); // Detach the fragment shader
    glDetachShader(shader_id, shader_vp); // Detach the vertex shader
    
    glDeleteShader(shader_fp); // Delete the fragment shader
    glDeleteShader(shader_vp); // Delete the vertex shader
    glDeleteProgram(shader_id); // Delete the shader program
}

/**
	id returns the integer value associated with the shader program
*/
unsigned int Shader::id() {
    return shader_id; // Return the shaders identifier
}

/**
	bind attaches the shader program for use by OpenGL
*/
uniformLocationStruct* Shader::bind() { 
    glUseProgram(shader_id);
	return uniformLocations;
}

/**
	unbind deattaches the shader program from OpenGL
*/
void Shader::unbind() {
    glUseProgram(0);
}
