#ifndef TEXTURE_H
#define TEXTURE_H

// Load a .BMP file using our custom loader
GLuint loadBMP(const char * imagePath);

// Load a .DDS file using GLFW's own loader
GLuint loadDDS(const char* imagePath);

#endif