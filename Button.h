#ifndef DEMO_H
#define DEMO_H


#include <SOIL/SOIL.h>

#include <SDL/SDL_mixer.h>
#include <SDL/SDL_thread.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <map>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Game.h"

using namespace glm;

#define FONTSIZE 40
#define FONTNAME "hongkonghustle.ttf"
#define NUM_BUTTON 5
GLuint texture[NUM_BUTTON], hover_texture[NUM_BUTTON], VBO, VBO2, VAO, VAO2, bgVBO, bgVAO, program, background;
float button_width[NUM_BUTTON], button_height[NUM_BUTTON], hover_button_width[NUM_BUTTON], hover_button_height[NUM_BUTTON];
int activeButtonIndex = 0;

#endif