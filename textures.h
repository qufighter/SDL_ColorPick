#ifndef __TEXTUREMAN_HEADER__
#define __TEXTUREMAN_HEADER__

#include "main.h"
#include <iostream>
#include <fstream>
using namespace std;
#include <vector>

class Textures {
public:
	Textures(void); // Default constructor
	//Textures(HWND hwnd); // Constructor for creating our context given a hwnd
	~Textures(void); // Destructor for cleaning up our application

    GLuint GenerateTexture();


   // GLuint LoadTexture(SDL_Surface *surface);
    GLuint LoadTextureFromSdlSurface(SDL_Surface *surface, GLuint& textureid);

    SDL_Surface* LoadSurface(const char* filename);

    GLuint LoadTextureSized(SDL_Surface *surface, GLuint& contained_in_texture_id, GLuint& textureid_centered_on, int size, int *x, int *y, SDL_Color* backgroundColor);
    GLuint LoadTextureSizedFromSdlSurface(SDL_Surface *surface, int widthHeight, int *x, int *y, GLuint& contained_in_texture_id, GLuint& textureid_centered_on, SDL_Color* backgroundColor);
    SDL_Surface* ConvertSurface(SDL_Surface *origSurface);

    GLuint LoadTexture(const char* filename, GLuint& textureid);
	GLuint LoadTexture(const char* filename);
	//GLuint LoadTexture(char* filename);
	GLuint LoadTexture(unsigned char *data, int width, int height);

    void replaceTexture(GLuint textureid);

	void screenshot(char* filename,int width, int height);
	//void screenshot(char filename[160],int width, int height);
//    GLuint screenToTexture(int x, int y, int width, int height, GLuint targetTexture);
//    GLuint screenToTexture(int x, int y, int width, int height, GLuint bufferToRead, GLuint targetTexture);

	void debugDevILerror();

    SDL_Color selectedColor;


private:
	//GLuint LoadTextureRAW( const char * filename, int wrap );
	
	void dataToTgaFile(char filename[160],unsigned char *data,int width, int height);

protected:

};

#endif// __TEXTUREMAN_HEADER__
