#define _CRT_SECURE_NO_WARNINGS 1
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL_mixer.h>
//����
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT  600
// ����������
#define BAR_WIDTH  200
#define BAR_HEIGHT  30
#define BAR_X 550
#define BAR_Y 50
// ��Һ�ƽ̨��һЩ����
const int PLAYER_RADIUS = 25;
int PLATFORM_RADIUS = 70;
const float GRAVITY = 0.5f;
const float JUMP_STRENGTH = 10.0f;
const float MAX_CHARGE = 2.0f; // �����������
int robot = 1;
double fin = 0;
// ��ҽṹ��
typedef struct {
	float x, y;
	float velocity;
	bool jumping;
	float charge; // ����ֵ
} Player;

// ƽ̨�ṹ��
typedef struct {
	float x, y;
	int radius;
} Platform;

SDL_Color color = { 0, 0, 0, 255 };
TTF_Font* font;
int score = 0;
int maxscore = 0;
int life = 1;

void renderChargeBar(SDL_Renderer* renderer, float charge) {
	// ���������������ı���
	SDL_Rect barBg = { BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT };
	SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // ��ɫ����
	SDL_RenderFillRect(renderer, &barBg);

	// ����������������ǰ��
	SDL_Rect barFg = { BAR_X, BAR_Y, (int)(BAR_WIDTH * (charge / MAX_CHARGE)), BAR_HEIGHT };
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // ��ɫ����
	SDL_RenderFillRect(renderer, &barFg);
}
void initPlayer(Player* player) {
	player->x = WINDOW_WIDTH / 2;
	player->y = WINDOW_HEIGHT - PLAYER_RADIUS;
	player->velocity = 0;
	player->jumping = false;
	player->charge = 0; // ��ʼ������ֵ
}

void initPlatform(Platform* platform, float x, float y, int z) {
	platform->x = x;
	platform->y = y;
	platform->radius = z;
}

void jump(Player* player) {
	if (robot == 1&&!player->jumping && player->charge > 0) {
		player->jumping = true;
		player->velocity = JUMP_STRENGTH * player->charge; // ��������ֵ��Ծ
		player->charge = 0; // ��Ծ����������ֵ
		//player->max = (player->velocity * player->velocity) / (2 * GRAVITY);
	}
	else if (robot == 0 && !player->jumping && player->charge >= fin) {
		player->jumping = true;
		player->velocity = JUMP_STRENGTH * player->charge; // ��������ֵ��Ծ
		player->charge = 0; // ��Ծ����������ֵ
	}
}

bool checkArrive(Player* player, Platform* platform) {
	//���
	float dx = player->x - platform->x;
	float dy = player->y - platform->y;
	float distance = sqrt(dx * dx + dy * dy);
	return distance < platform->radius;
	//return distance < platform->radius + PLAYER_RADIUS;
}

bool checkArrivekiss(Player* player, Platform* platform) {
	//���
	float dx = player->x - platform->x;
	float dy = player->y - platform->y;
	float distance = sqrt(dx * dx + dy * dy);
	return distance < platform->radius / 2.0;
}
void updatePlayer(Player* player) {
	if (player->jumping) {
		if (player->velocity - GRAVITY >= 0) {
			player->velocity -= GRAVITY; // ����
			player->y -= player->velocity; // �����ƶ�
		}
		else
			player->velocity = 0;
	}
	else {
		if (robot == 1) {
			// ������û����Ծ���ҿո�������£���������ֵ
			if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_SPACE]) {
				player->charge += 0.1f; // ÿ֡���ӵ�����ֵ
				if (player->charge > MAX_CHARGE) {
					player->charge = MAX_CHARGE; // �����������ֵ
				}
			}
		}
		else {
			if (player->charge < fin)
				player->charge += 0.05f;
		}

	}
}

void PrintInfo_Start(TTF_Font* font, SDL_Renderer* renderer) {
	SDL_Surface* text = TTF_RenderUTF8_Blended(font, "Press 'SPACE' to start !", color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text);
	SDL_Rect srect = { 200,200,text->w ,text->h };
	SDL_RenderCopy(renderer, texture, NULL, &srect);
	SDL_FreeSurface(text);
	SDL_DestroyTexture(texture);
}

void PrintInfo_1(TTF_Font* font, SDL_Renderer* renderer) {
	SDL_Surface* text = TTF_RenderUTF8_Blended(font, "in game,press 1 to hosted mode", color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text);
	SDL_Rect srect = { 150,250,text->w ,text->h };
	SDL_RenderCopy(renderer, texture, NULL, &srect);
	SDL_FreeSurface(text);
	SDL_DestroyTexture(texture);
}

void PrintInfo_End(TTF_Font* font, SDL_Renderer* renderer) {
	SDL_Surface* text = TTF_RenderUTF8_Blended(font, "You Died,Press 'SPACE' to Restart !", color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text);
	SDL_Rect srect = { 100,250,text->w ,text->h };
	SDL_RenderCopy(renderer, texture, NULL, &srect);
	SDL_FreeSurface(text);
	SDL_DestroyTexture(texture);
}

void PrintInfo_Robot(TTF_Font* font, SDL_Renderer* renderer) {
	SDL_Color colorro = { 0, 0, 255, 255 };
	SDL_Surface* text = TTF_RenderUTF8_Blended(font, "robot operating", colorro);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text);
	SDL_Rect srect = { 50,250,text->w ,text->h };
	SDL_RenderCopy(renderer, texture, NULL, &srect);
	SDL_FreeSurface(text);
	SDL_DestroyTexture(texture);
}

void Draw_Platform(Platform* platform, SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	for (int w = 0; w < platform->radius * 2; w++) {
		for (int h = 0; h < platform->radius * 2; h++) {
			int dx = platform->radius - w;
			int dy = platform->radius - h;
			if (dx * dx + dy * dy <= platform->radius * platform->radius) {
				SDL_RenderDrawPoint(renderer, platform->x + dx, platform->y + dy);
			}
		}
	}
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	for (int w = 0; w < platform->radius * 2; w++) {
		for (int h = 0; h < platform->radius * 2; h++) {
			int dx = platform->radius - w;
			int dy = platform->radius - h;
			if (dx * dx + dy * dy <= ((platform->radius / 2.0) * (platform->radius / 2.0))) {
				SDL_RenderDrawPoint(renderer, platform->x + dx, platform->y + dy);
			}
		}
	}
}

void Draw_Rectangle(Platform* platform, SDL_Renderer* renderer) {
	SDL_Rect rect = { platform->x - platform->radius,platform->y - platform->radius, 2 * platform->radius, 2 * platform->radius };
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	for (int w = 0; w < platform->radius * 2; w++) {
		for (int h = 0; h < platform->radius * 2; h++) {
			int dx = platform->radius - w;
			int dy = platform->radius - h;
			if (dx * dx + dy * dy <= ((platform->radius / 2.0) * (platform->radius / 2.0))) {
				SDL_RenderDrawPoint(renderer, platform->x + dx, platform->y + dy);
			}
		}
	}
}

void print_score(TTF_Font* font, SDL_Renderer* renderer) {
	char s[10086] = { 0 };
	sprintf(s, "SCORE : %d", score);
	SDL_Surface* text = TTF_RenderUTF8_Blended(font, s, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text);
	SDL_Rect srect = { 0,0,text->w ,text->h };
	SDL_RenderCopy(renderer, texture, NULL, &srect);
	SDL_FreeSurface(text);
	SDL_DestroyTexture(texture);
}

void print_scoremax(TTF_Font* font, SDL_Renderer* renderer) {
	char s[10086] = { 0 };
	sprintf(s, "MAXSCORE : %d", maxscore);
	SDL_Surface* text = TTF_RenderUTF8_Blended(font, s, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text);
	SDL_Rect srect = { 0,50,text->w ,text->h };
	SDL_RenderCopy(renderer, texture, NULL, &srect);
	SDL_FreeSurface(text);
	SDL_DestroyTexture(texture);
}

void print_scoreEnd(TTF_Font* font, SDL_Renderer* renderer) {
	char s[10086] = { 0 };
	sprintf(s, "SCORE : %d", score);
	SDL_Surface* text = TTF_RenderUTF8_Blended(font, s, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text);
	SDL_Rect srect = { 300,300,text->w ,text->h };
	SDL_RenderCopy(renderer, texture, NULL, &srect);
	SDL_FreeSurface(text);
	SDL_DestroyTexture(texture);
}
void print_Life(TTF_Font* font, SDL_Renderer* renderer) {
	char s[10086] = { 0 };
	sprintf(s, "LIFE : %d", life);
	SDL_Surface* text = TTF_RenderUTF8_Blended(font, s, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text);
	SDL_Rect srect = { 0,100,text->w ,text->h };
	SDL_RenderCopy(renderer, texture, NULL, &srect);
	SDL_FreeSurface(text);
	SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[])
{
	srand((int)time(NULL));
	//��ʼ��SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_Log("can not init SDL:%s", SDL_GetError());
		return 1;
	}
	TTF_Init();
	font = TTF_OpenFont("TTF/Minecraft.ttf", 35);
	if (!font) {
		SDL_Log("can not init SDL:%s", SDL_GetError());
		return 1;
	}
	// ��ʼ�� SDL, �������ں���Ⱦ������ʼ�� player �� platform
	Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
	Mix_Chunk* music_bg = Mix_LoadWAV("music/bgd.mp3");
	//Mix_Music* music_bg = Mix_LoadMUS("music/bgd.mp3");
	Mix_Chunk* music_fail = Mix_LoadWAV("music/fail.mp3");
	Mix_Chunk* music_chic = Mix_LoadWAV("music/chic.mp3");
	Mix_Chunk* music_odchic = Mix_LoadWAV("music/odchic.mp3");
	//Mix_PlayMusic(music_bg, -1);
	Mix_PlayChannel(-1, music_bg, -1);
	SDL_Window* window = SDL_CreateWindow(u8"��һ��С��Ϸ",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0);

	int running = 1;
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	int tem = rand() % 2;
	int cha = 0;
Again:;
	int ret = rand() % 2;
	int distance = 190 + rand() % 280 + 1;
ConTinue:;
	Player player;
	initPlayer(&player);
	Platform platform;
	Platform platform0;
	initPlatform(&platform, WINDOW_WIDTH / 2, WINDOW_HEIGHT - PLAYER_RADIUS - cha, PLATFORM_RADIUS);

	if (PLATFORM_RADIUS > 30) {
		initPlatform(&platform0, WINDOW_WIDTH / 2, distance, PLATFORM_RADIUS - 5);
	}
	else if (PLATFORM_RADIUS <= 30 && PLATFORM_RADIUS > 15) {
		initPlatform(&platform0, WINDOW_WIDTH / 2, distance, PLATFORM_RADIUS - 3);
	}
	else {
		initPlatform(&platform0, WINDOW_WIDTH / 2, distance, PLATFORM_RADIUS);
	}

	//SDL_Surface* screen = SDL_GetWindowSurface(window);
	SDL_Event event;
	while (running) {
		if (running == 1) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);
			PrintInfo_Start(font, renderer);
			PrintInfo_1(font, renderer);
			//SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer);
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					running = 0;
				}
				else if (event.type == SDL_KEYUP) {
					if (event.key.keysym.sym == SDLK_SPACE) {
						running = 2;
						break;
					}
				}
			}
		}
		else if (running == 3) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);
			print_scoreEnd(font, renderer);
			PrintInfo_End(font, renderer);
			SDL_RenderPresent(renderer);
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					running = 0;
				}
				else if (event.type == SDL_KEYUP) {
					if (event.key.keysym.sym == SDLK_SPACE) {
						life = 1;
						running = 2;
						score = 0;
						goto Again;
						break;
					}
				}
			}
		}
		else if (running == 2) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					running = 0;
				}
				else if (event.type == SDL_KEYUP) {
					if (event.key.keysym.sym == SDLK_SPACE) {
						jump(&player);
					}
					else if (event.key.keysym.sym == SDLK_1) {
						robot = 1 - robot;
						//jump(&player);
					}
				}
			}
			if (robot == 0) {
				//robotjump(&player);
				double dfen = WINDOW_HEIGHT - distance - PLAYER_RADIUS;
				fin = sqrt(dfen / 100.0);
				jump(&player);
			}
			if (player.jumping && player.velocity == 0) {
				player.jumping = false;
				if (!checkArrive(&player, &platform) && !checkArrive(&player, &platform0)) {
					life--;
					Mix_PlayChannel(-1, music_fail, 0);
					if (life == 0) {
						PLATFORM_RADIUS = 70;
						running = 3;
					}
					else {
						goto ConTinue;
					}
				}
				//����
				if (checkArrive(&player, &platform0)) {
					tem = ret;
					//��������
					if (checkArrivekiss(&player, &platform0)) {
						Mix_PlayChannel(-1, music_chic, 0);
						score += 20;
						life++;
					}
					else {
						Mix_PlayChannel(-1, music_odchic, 0);
						score += 10;
					}
					//����
					cha = player.y - platform0.y;
					PLATFORM_RADIUS = platform0.radius;
					SDL_Delay(20);
					goto Again;
				}
			}
			updatePlayer(&player);
			// ��Ⱦ��Ϸ����
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);
			//������ӡ
			print_score(font, renderer);
			if (score >= maxscore)
				maxscore = score;
			print_scoremax(font, renderer);
			print_Life(font, renderer);
			if (robot == 0) {
				PrintInfo_Robot(font, renderer);
			}
			// ����ƽ̨
			if (tem == 1) {
				Draw_Platform(&platform, renderer);
			}
			else {
				Draw_Rectangle(&platform, renderer);
			}
			if (ret == 1) {
				Draw_Platform(&platform0, renderer);
			}
			else {
				Draw_Rectangle(&platform0, renderer);
			}
			// �������
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			for (int w = 0; w < PLAYER_RADIUS * 2; w++) {
				for (int h = 0; h < PLAYER_RADIUS * 2; h++) {
					int dx = PLAYER_RADIUS - w;
					int dy = PLAYER_RADIUS - h;
					if (dx * dx + dy * dy <= PLAYER_RADIUS * PLAYER_RADIUS) {
						SDL_RenderDrawPoint(renderer, player.x + dx, player.y + dy);
					}
				}
			}
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
			SDL_RenderDrawPoint(renderer, player.x, player.y);
			// ��������������
			renderChargeBar(renderer, player.charge);

			SDL_RenderPresent(renderer);

			SDL_Delay(16);
		}
	}
	// ������Ⱦ���ʹ��ڣ��˳� SDL
	Mix_FreeChunk(music_bg);
	Mix_FreeChunk(music_fail);
	Mix_FreeChunk(music_chic);
	Mix_FreeChunk(music_odchic);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
