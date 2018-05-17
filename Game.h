#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

using namespace std;
using namespace glm;

enum class State { RUNNING, EXIT };
enum class WindowFlag { WINDOWED, FULLSCREEN, EXCLUSIVE_FULLSCREEN, BORDERLESS };

namespace Engine {
	class Game
	{
	public:
		Game();
		~Game();
		void Start(string title, unsigned int width, unsigned int height, bool vsync, WindowFlag windowFlag, unsigned int targetFrameRate, float timeScale);
		// Input Handling
		void PressKey(unsigned int keyID);
		void ReleaseKey(unsigned int keyID);
		void SetMouseCoords(float x, float y);
		// Returns true if the key is held down
		bool IsKeyDown(string name);
		// Returns true if the key was just pressed
		bool IsKeyUp(string name);
		// getters
		vec2 GetMouseCoords() const { return _mouseCoords; }
		// Returns true if the key is held down
		bool WasKeyDown(string name);
		void InputMapping(string mappingName, unsigned int keyId);

	protected:
		virtual void Init() = 0;
		virtual void DeInit() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void Render() = 0;
		GLuint BuildShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
		void UseShader(GLuint program);
		unsigned int GetScreenHeight();
		unsigned int GetScreenWidth();
		void Err(string errorString);

	private:
		unordered_map<unsigned int, string> _mapNames;
		unordered_map<string, bool> _keyMap;
		unordered_map<string, bool> _previousKeyMap;
		vec2 _mouseCoords;
		SDL_GameController *controller;
		unsigned int screenWidth, screenHeight, lastFrame = 0, last = 0, _fps = 0, fps = 0;
		float targetFrameTime = 0, timeScale;
		State state;
		float GetDeltaTime();
		void GetFPS();
		void PollInput();
		void LimitFPS();
		void CheckShaderErrors(GLuint shader, string type);
		void PrintFPS();
		void OpenGameController();
		void CloseGameController();
	};
}
#endif

