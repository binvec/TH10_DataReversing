#include "util.h"
#include "data.h"
#include <iostream>
#define SDL_MAIN_HANDLED
#include <SDL.h>

void DrawObject(SDL_Renderer* renderer, Object& obj, int r, int g, int b);
void DrawPlayer(SDL_Renderer* renderer, Player& plyr);
void DrawLaser(SDL_Renderer* renderer, Laser& laser);

int main()
{
	HANDLE TH10_Process_Handle = OpenGameProcess("th10.exe");
	if (!TH10_Process_Handle)
	{
		return 1;
	}
	DataReader dr(TH10_Process_Handle);
	
	std::vector<Object> bullets;
	std::vector<Object> enemies;
	std::vector<Item> items;
	std::vector<Laser> lasers;
	Player plyr;

	SDL_Window* win = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GAME_WIDTH, GAME_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	SDL_Event e;
	bool quit = false;

	while (!quit)
	{
		dr.GetBulletsInfo(bullets);
		dr.GetEnemiesInfo(enemies);
		dr.GetItemsInfo(items);
		dr.GetPlayerInfo(plyr);
		dr.GetLasersInfo(lasers);
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				quit = true;
			default:
				break;
			}
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
		SDL_RenderClear(renderer);
		for (auto s : bullets)
		{
			DrawObject(renderer, s, 0, 0, 0);
		}
		for (auto s : enemies)
		{
			DrawObject(renderer, s, 0, 0, 0);
		}
		for (auto s : items)
		{
			int r = 0, g = 0, b = 0;
			switch (s.type)
			{
			case 1:
			case 10:
				r = 255;
				break;
			case 2:
			case 5:
				b = 255;
				break;
			case 3:
			case 9:
				g = 255;
				break;
			case 4:
				r = 255;
				s.w += 3;
				s.h += 3;
				break;
			case 7:
				r = 255;
				b = 255;
				break;
			}
			DrawObject(renderer, s, r, g, b);
		}
		for (auto &s : lasers)
		{
			DrawLaser(renderer, s);
		}
		DrawPlayer(renderer, plyr);
		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
}

void DrawObject(SDL_Renderer* renderer, Object& obj, int r, int g, int b)
{	
	SDL_SetRenderDrawColor(renderer, r, g, b, 0);
	SDL_Rect rect { obj.p.x - 0.5 * obj.w + GAME_X_OFFSET,
				    obj.p.y - 0.5 * obj.h + GAME_Y_OFFSET,
				    obj.w, obj.h
	};
	SDL_RenderDrawRect(renderer, &rect);
	vec2 proj = obj.p + obj.v * 10;
	SDL_RenderDrawLine(renderer, obj.p.x + GAME_X_OFFSET, obj.p.y + GAME_Y_OFFSET, proj.x + +GAME_X_OFFSET, proj.y + GAME_Y_OFFSET);
}

void DrawPlayer(SDL_Renderer* renderer, Player& plyr)
{

	DrawObject(renderer, plyr, 255, 128, 128);
	SDL_SetRenderDrawColor(renderer, 0, 255, 255, 0);
	SDL_Rect rect{ plyr.p.x - 0.5 * plyr.itemObtainRange + GAME_X_OFFSET,
		plyr.p.y - 0.5 * plyr.itemObtainRange + GAME_Y_OFFSET,
		plyr.itemObtainRange, plyr.itemObtainRange
	};
	SDL_RenderDrawRect(renderer, &rect);
}

void DrawLaser(SDL_Renderer* renderer, Laser& laser)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	std::vector<vec2> vertices = laser.getVertices();
	for (int i = 0; i < 4; i++)
	{
		SDL_RenderDrawLine(renderer, 
			vertices[i].x + GAME_X_OFFSET, 
			vertices[i].y + GAME_Y_OFFSET, 
			vertices[(i + 1) % 4].x + GAME_X_OFFSET, 
			vertices[(i + 1) % 4].y + GAME_Y_OFFSET);
	}
}