#pragma once
#include <SDL.h>
#include <vector>

struct Vector2 {
	float x;
	float y;
};

struct Ball {
	Vector2 vel;
	Vector2 pos;
};

class Game
{
public:

	Game();

	bool Initialize();

	void RunLoop();

	void ShutDown();
private:

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;

	bool mIsRunning;

	Vector2 mPaddlePos;
	Vector2 mPaddlePos2;

	Uint32 mTicksStart;

	int mPaddleDir;
	int mPaddleDir2;

	Ball vecInitEx;

	std::vector<Ball> mBalls = { vecInitEx, vecInitEx };
};