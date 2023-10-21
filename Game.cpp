#include "Game.h"
#include <SDL.h>



const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
{
    mIsRunning = true;
    mTicksStart = 0;
    mPaddleDir = 0;
}

bool Game::Initialize()
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow(
        "Pong",
        100,
        10,
        1024,
        768,
        0
    );
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(
        mWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!mRenderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    mPaddlePos.x = 10.0f;
    mPaddlePos.y = 768.0f / 2.0f;
    mPaddlePos2.x = 1014.0f;
    mPaddlePos2.y = 768.0f / 2.0f;

    mBalls[0].pos.x = 1024.0f / 2.0f;
    mBalls[0].pos.y = 768.0f / 2.0f;
    mBalls[0].vel.x = -200.0f;
    mBalls[0].vel.y = 235.0f;

    return true;
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ShutDown()
{
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            mIsRunning = false;
            break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
    {
        mIsRunning = false;
    }


    mPaddleDir = 0;
    if (state[SDL_SCANCODE_W])
    {
        mPaddleDir -= 1;
    }
    if (state[SDL_SCANCODE_S])
    {
        mPaddleDir += 1;
    }

    mPaddleDir2 = 0;
    if (state[SDL_SCANCODE_UP])
    {
        mPaddleDir2 -= 1;
    }
    if (state[SDL_SCANCODE_DOWN])
    {
        mPaddleDir2 += 1;
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksStart + 16));

    float deltaTime = (SDL_GetTicks() - mTicksStart) / 1000.0f;

    if (deltaTime > 0.05f)
    {
        deltaTime = 0.05f;
    }

    mTicksStart = SDL_GetTicks();

    if (mPaddleDir != 0)
    {
        mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
        // make sure paddle doesnt move off screen
        if (mPaddlePos.y < (paddleH / 2.0f + thickness))
        {
            mPaddlePos.y = paddleH / 2.0f + thickness;
        }
        else if (mPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
        {
            mPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
        }
    }

    if (mPaddleDir2 != 0)
    {
        mPaddlePos2.y += mPaddleDir2 * 300.0f * deltaTime;
        // make sure paddle doesnt move off screen
        if (mPaddlePos2.y < (paddleH / 2.0f + thickness))
        {
            mPaddlePos2.y = paddleH / 2.0f + thickness;
        }
        else if (mPaddlePos2.y > (768.0f - paddleH / 2.0f - thickness))
        {
            mPaddlePos2.y = 768.0f - paddleH / 2.0f - thickness;
        }
    }

    for (int i = 0; i < mBalls.size(); ++i)
    {
        mBalls[i].pos.x += mBalls[i].vel.x * deltaTime;
        mBalls[i].pos.y += mBalls[i].vel.y * deltaTime;
    }

    for (int i = 0; i < mBalls.size(); ++i)
    {

        float diff = mPaddlePos.y - mBalls[i].pos.y;
        diff = (diff > 0.0f) ? diff : -diff;
        if (
            diff <= paddleH / 2.0f &&
            mBalls[i].pos.x <= 25.0f && mBalls[i].pos.x >= 20.0f &&
            mBalls[i].vel.x < 0.0f)
        {
            mBalls[i].vel.x *= -1.0f;
        }
        else if (mBalls[i].pos.x < 0.0f) // OLD: ...pos.x <= 0.0f
        {
            /* A weird glitch occured where the program would start and then immediately end with the old code. 
            * For some unexplained reason, the ball was rendered in the center, but the apllication window closed
            * because of this parameter. 
             */
            mIsRunning = false;
        }

        float diff2 = mPaddlePos2.y - mBalls[i].pos.y;
        diff2 = (diff2 < 1024.0f) ? diff2 : -diff2;
        if (
            diff2 <= paddleH / 2.0f &&
            mBalls[i].pos.x >= 999.0f && mBalls[i].pos.x <= 1014.0f &&
            mBalls[i].vel.x > 0.0f)
        {
            mBalls[i].vel.x *= -1;
        }
        else if (mBalls[i].pos.x >= 1024.0f)
        {
           mIsRunning = false;
        }

        if (mBalls[i].pos.y <= thickness && mBalls[i].vel.y < 0.0f)
        {
            mBalls[i].vel.y *= -1;
        }
        else if (mBalls[i].pos.y >= (768 - thickness) && mBalls[i].vel.y > 0.0f)
        {
            mBalls[i].vel.y *= -1;
        }

        if (mBalls[i].pos.y <= thickness && mBalls[i].vel.y < 0.0f)
        {
            mBalls[i].vel.y *= -1;
        }
    }
}

void Game::GenerateOutput()
{
    SDL_SetRenderDrawColor(
        mRenderer,
        0,
        0,
        255,
        255
    );

    SDL_RenderClear(mRenderer);


    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

    SDL_Rect wall{
        0,
        0,
        1024,
        thickness
    };
    SDL_RenderFillRect(mRenderer, &wall); // top wall

    wall.y = 768 - thickness;
    SDL_RenderFillRect(mRenderer, &wall); // bottom wall

    for (int i = 0; i < mBalls.size(); ++i)
    {
        SDL_Rect ball{
            static_cast<int> (mBalls[i].pos.x - thickness / 2),
            static_cast<int> (mBalls[i].pos.y - thickness / 2),
            thickness,
            thickness
        };
        SDL_RenderFillRect(mRenderer, &ball); // ball 0
    }


    SDL_Rect paddle{
        static_cast<int>(10.0f),
        static_cast<int>(mPaddlePos.y - paddleH / 2),
        thickness,
        static_cast<int>(paddleH)
    };
    SDL_RenderFillRect(mRenderer, &paddle);

    SDL_Rect paddle2{
        static_cast<int>(999.0f),
        static_cast<int>(mPaddlePos2.y - paddleH / 2),
        thickness,
        static_cast<int>(paddleH)
    };
    SDL_RenderFillRect(mRenderer, &paddle2);


    SDL_RenderPresent(mRenderer);
}