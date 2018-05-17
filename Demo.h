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
#define NUM_LOGO 1

struct Character {
	GLuint TextureID; // ID handle of the glyph texture
	ivec2 Size; // Size of glyph
	ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance; // Offset to advance to next glyph
};

class Demo :
	public Engine::Game
{
public:
	Demo();
	~Demo();
	virtual void Init();
	virtual void DeInit();
	virtual void Update(float deltaTime);
	virtual void Render();
private:
	void InitText();
	void RenderText();
	void InitButton();
	void RenderButton(GLfloat x, GLfloat y);
	/*void initCredit();
	void renderCredit();*/
	/*void initLeaderboard();
	void renderLeaderboard();*/
	/*void initSetting();
	void renderSetting();*/
	void BuildLogo();
	void DrawLogo();
	map<GLchar, Character> Characters;
	GLuint texture[NUM_BUTTON], hover_texture[NUM_BUTTON], logo_texture, VBO, VBO2, VAO, VAO2, logoVBO, logoVAO, logoEBO, program, program2;
	float button_width[NUM_BUTTON], button_height[NUM_BUTTON], hover_button_width[NUM_BUTTON], hover_button_height[NUM_BUTTON], logo_width, logo_height;
	int activeButtonIndex = 0;
};
#endif

