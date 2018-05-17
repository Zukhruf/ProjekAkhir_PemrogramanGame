#include "Game.h"


Engine::Game::Game()
{
}


Engine::Game::~Game()
{
}

void Engine::Game::Start(string windowTitle, unsigned int screenWidth, unsigned int screenHeight, bool vsync, WindowFlag windowFlag, unsigned int targetFrameRate, float timeScale)
{
	//Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//Tell SDL that we want a double buffered window so we don't get any flickering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->timeScale = timeScale;

	if (targetFrameRate > 0) {
		targetFrameTime = 1000.0f / targetFrameRate;
	}


	Uint32 flags = SDL_WINDOW_OPENGL;

	if (WindowFlag::EXCLUSIVE_FULLSCREEN == windowFlag) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	if (WindowFlag::FULLSCREEN == windowFlag) {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	if (WindowFlag::BORDERLESS == windowFlag) {
		flags |= SDL_WINDOW_BORDERLESS;
	}

	// Setup SDL Window
	SDL_Window* window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
	if (window == nullptr) {
		Err("Failed to create SDL window!");
	}

	//Set up our OpenGL context
	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr) {
		Err("Failed to create SDL_GL context!");
	}

	//Set up glew (optional but recommended)
	GLenum glewStat = glewInit();
	if (glewStat != GLEW_OK) {
		Err("Failed to initialize glew!");
	}

	//Set VSYNC
	SDL_GL_SetSwapInterval(vsync);

	this->state = State::RUNNING;

	Init();

	// Init delta time calculation
	last = SDL_GetTicks();

	//Will loop until we set _gameState to EXIT
	while (State::RUNNING == state) {
		float deltaTime = GetDeltaTime();
		GetFPS();
		PollInput();
		Update(deltaTime);
		Render();
		SDL_GL_SwapWindow(window);
		LimitFPS();
		PrintFPS();
	}

	DeInit();

}


float Engine::Game::GetDeltaTime()
{
	unsigned int time = SDL_GetTicks();
	unsigned int delta = time - lastFrame;
	lastFrame = time;

	return delta * timeScale;
}

void Engine::Game::GetFPS()
{
	if (SDL_GetTicks() - last > 1000) {
		fps = _fps;
		_fps = 0;
		last += 1000;
	}
	_fps++;
}

void Engine::Game::PrintFPS() {
	//print only once every 60 frames
	static int frameCounter = 0;
	frameCounter++;
	if (frameCounter == 60) {
		cout << "FPS: " << fps << endl;
		frameCounter = 0;
	}
}

void Engine::Game::OpenGameController()
{
	if (SDL_NumJoysticks() > 0) {
		if (SDL_IsGameController(0))
		{
			controller = SDL_GameControllerOpen(0);
			if (controller != nullptr)cout << "Game Controller is Detected" << endl;
		}
	}
}

void Engine::Game::CloseGameController()
{
	if (controller != nullptr) {
		SDL_GameControllerClose(0);
		cout << "Game Controller is Removed" << endl;
	}
}

// -------------- Input Handling --------------------

void Engine::Game::PollInput()
{
	SDL_Event evt;

	//Will keep looping until there are no more events to process
	while (SDL_PollEvent(&evt)) {
		switch (evt.type) {
		case SDL_QUIT:
			state = State::EXIT;
			break;
		case SDL_MOUSEMOTION:
			SetMouseCoords((float)evt.motion.x, (float)evt.motion.y);
			break;
		case SDL_KEYDOWN:
			PressKey(evt.key.keysym.sym);
			break;
		case SDL_KEYUP:
			ReleaseKey(evt.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			PressKey(evt.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			ReleaseKey(evt.button.button);
			break;
		case SDL_CONTROLLERDEVICEADDED:
			OpenGameController();
			break;
		case SDL_CONTROLLERDEVICEREMOVED:
			CloseGameController();
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			PressKey(evt.cbutton.button);
			break;
		case SDL_CONTROLLERBUTTONUP:
			ReleaseKey(evt.cbutton.button);
			break;
		}
	}
}

void Engine::Game::PressKey(unsigned int keyID) {
	// Here we are treating _keyMap as an associative array.
	// if keyID doesn't already exist in _keyMap, it will get added
	auto it = _mapNames.find(keyID);
	if (it != _mapNames.end()) {
		string mapName = it->second;
		_keyMap[mapName] = true;
	}

}

void Engine::Game::ReleaseKey(unsigned int keyID) {
	auto it = _mapNames.find(keyID);
	if (it != _mapNames.end()) {
		string mapName = it->second;
		_keyMap[mapName] = false;
	}
}

void Engine::Game::SetMouseCoords(float x, float y) {
	_mouseCoords.x = x;
	_mouseCoords.y = y;
}

bool Engine::Game::IsKeyDown(string name) {
	// We dont want to use the associative array approach here
	// because we don't want to create a key if it doesnt exist.
	// So we do it manually
	auto it = _keyMap.find(name);
	if (it != _keyMap.end()) {
		// Found the key
		return it->second;
	}
	else {
		// Didn't find the key
		return false;
	}
}

bool Engine::Game::IsKeyUp(string name) {
	// Check if it is pressed this frame, and wasn't pressed last frame
	if (IsKeyDown(name) == true && WasKeyDown(name) == false) {
		return true;
	}
	return false;
}

bool Engine::Game::WasKeyDown(string name) {
	// We dont want to use the associative array approach here
	// because we don't want to create a key if it doesnt exist.
	// So we do it manually
	auto it = _previousKeyMap.find(name);
	if (it != _previousKeyMap.end()) {
		// Found the key
		return it->second;
	}
	else {
		// Didn't find the key
		return false;
	}
}

void Engine::Game::InputMapping(string mappingName, unsigned int keyId)
{
	_mapNames.insert(pair<unsigned int, string>(keyId, mappingName));
}

//Prints out an error message and exits the game
void Engine::Game::Err(string errorString)
{
	cout << errorString << endl;
	SDL_Quit();
	exit(1);
}

void Engine::Game::LimitFPS()
{
	//Limit the FPS to the max FPS
	SDL_Delay((Uint32)(targetFrameTime));

}

void Engine::Game::CheckShaderErrors(GLuint shader, string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			Err("| ERROR::::SHADER-COMPILATION-ERROR of type: " + type + "|\n" + infoLog + "\n| -- --------------------------------------------------- -- |");
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			Err("| ERROR::::PROGRAM-LINKING-ERROR of type: " + type + "|\n" + infoLog + "\n| -- --------------------------------------------------- -- |");
		}
	}
}

GLuint Engine::Game::BuildShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	string vertexCode;
	string fragmentCode;
	string geometryCode;
	ifstream vShaderFile;
	ifstream fShaderFile;
	ifstream gShaderFile;
	// ensures ifstream objects can throw exceptions:
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	gShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// If geometry shader path is present, also load a geometry shader
		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (ifstream::failure e)
	{
		Err("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar * fShaderCode = fragmentCode.c_str();
	// 2. Compile shaders
	GLuint vertex, fragment;
	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	CheckShaderErrors(vertex, "VERTEX");
	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	CheckShaderErrors(fragment, "FRAGMENT");
	// If geometry shader is given, compile geometry shader
	GLuint geometry;
	if (geometryPath != nullptr)
	{
		const GLchar * gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		CheckShaderErrors(geometry, "GEOMETRY");
	}
	// Shader Program
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	if (geometryPath != nullptr)
		glAttachShader(program, geometry);
	glLinkProgram(program);
	CheckShaderErrors(program, "PROGRAM");
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr)
		glDeleteShader(geometry);
	return program;

}

void Engine::Game::UseShader(GLuint program)
{
	// Uses the current shader
	glUseProgram(program);
}

unsigned int Engine::Game::GetScreenHeight() {
	return this->screenHeight;
}

unsigned int Engine::Game::GetScreenWidth() {
	return this->screenWidth;
}



