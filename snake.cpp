#include <stdio.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <stdlib.h> 
#include <time.h>

#define SNAKE_MAXX 640
#define SNAKE_MAXY 480
#define SNAKE_STARTBLOCKS 3
#define SNAKE_MAXBLOCKS 100
#define SNAKE_DELAY 20

const int MAXXD10=SNAKE_MAXX/10, MAXYD10=SNAKE_MAXY/10;

int GenerateNumber(Uint32 modulo);
void AManIsCoughingInBrazil(int code);
void InitializeSDL(Uint32 flags);
SDL_Surface* SetVideoModeAndVerify(int width, int height, int bitsperpixel, Uint32 flags);
void SetTitleAndIcon(char *title, char *titlemin, char *iconpath);
void InputHandling(SDL_Event event, int snake[SNAKE_MAXBLOCKS][2], int state[MAXXD10][MAXYD10], int *running, int *dead, int *currentDirection, int *formerDirection, int *currentSize, int * amountOfApples, int *maxApples, int *score, long int *frameCounter, int *divider);
void DrawEntireBitmap(SDL_Surface *screen, char *path, int x, int y);
void Drawing(SDL_Surface *screen, int state[MAXXD10][MAXYD10], int *dead, int *score);
void InitialSnakeSetup(int snake[SNAKE_MAXBLOCKS][2], int state[MAXXD10][MAXYD10]);
void GenerateRandomlyPlacedApple(int *amountOfApples, int maxApples, int state[MAXXD10][MAXYD10]);
void AppleCollisionHandling(int state[MAXXD10][MAXYD10], int snake[SNAKE_MAXBLOCKS][2], int *currentSize, int *amountOfApples, int *score);
void WallCollisionHandling(int snake[SNAKE_MAXBLOCKS][2], int *dead);
void SelfCollisionHandling(int snake[SNAKE_MAXBLOCKS][2], int *dead);
void MoveSnake(int snake[SNAKE_MAXBLOCKS][2], int state[MAXXD10][MAXYD10], int *currentDirection, int *currentSize, int *amountOfApples, int *score, int *dead);
void ResetVariables(int *running, int *dead, int *currentDirection, int *formerDirection, int *currentSize, int * amountOfApples, int *maxApples, int *score);
void PlayAgain(int snake[SNAKE_MAXBLOCKS][2], int state[MAXXD10][MAXYD10], int *running, int *dead, int *currentDirection, int *formerDirection, int *currentSize, int *amountOfApples, int *maxApples, int *score);
int IsActionFrame(long int *frameCounter, int *divider);

int main(int argc, char *argv[]) {

	srand(time(NULL));
	
	InitializeSDL(SDL_INIT_EVERYTHING);
	
	SDL_Surface *screen;
	screen = SetVideoModeAndVerify(SNAKE_MAXX, SNAKE_MAXY, 0, SDL_ANYFORMAT | SDL_DOUBLEBUF);
	
	SetTitleAndIcon("AD's Snake", "AD's Snake", "icon.bmp");
	
	SDL_Event event;
	int running = 1, dead = 0;
	/*   7 8 9
	      \|/
	     4-5-6
	      /|\
	     1 2 3    */
	int currentDirection=4, formerDirection=4;
	int state [MAXXD10][MAXYD10];
	int snake [SNAKE_MAXBLOCKS][2]; // first row is X, second row is Y
	int currentSize = SNAKE_STARTBLOCKS;
	int amountOfApples = 0, maxApples = 1;
	int score = 0; // gotta keep track of how many apples have been eaten
	
	ResetVariables(&running, &dead, &currentDirection, &formerDirection, &currentSize, &amountOfApples, &maxApples, &score);
	InitialSnakeSetup(snake, state);
	
	long int frameCounter = 0;
	long int newticks, oldticks;
	int divider = 5;
	while(running) {
		oldticks = SDL_GetTicks();
		
		InputHandling(event, snake, state, &running, &dead, &currentDirection, &formerDirection, &currentSize, & amountOfApples, &maxApples, &score, &frameCounter, &divider);
	  
	  if (IsActionFrame(&frameCounter, &divider)) { 
			GenerateRandomlyPlacedApple(&amountOfApples, maxApples, state);
			if (!dead) { MoveSnake(snake, state, &currentDirection, &currentSize, &amountOfApples, &score, &dead); }
			Drawing(screen, state, &dead, &score);
		}
				
		newticks = SNAKE_DELAY - (SDL_GetTicks() - oldticks);
		if (newticks>0) { SDL_Delay(newticks); }
		frameCounter++;
	}

	printf("Your score was: %d\n", score);
	
	SDL_Quit();
  return 0;
}

int GenerateNumber(Uint32 divider) {
	return rand() % divider;
}
void AManIsCoughingInBrazil(int code) {
	SDL_Quit();
	exit(code);
}
void InitializeSDL(Uint32 flags) {
	if (SDL_Init(flags) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		AManIsCoughingInBrazil(1);
	}
} 
SDL_Surface* SetVideoModeAndVerify(int width, int height, int bitsperpixel, Uint32 flags) {
	SDL_Surface *screen;
	screen = SDL_SetVideoMode(width, height, bitsperpixel, flags);
	if (screen == NULL) {
		fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
		AManIsCoughingInBrazil(1);
	}
	return screen;
}
void SetTitleAndIcon(char *title, char *titlemin, char *iconpath) {
	SDL_WM_SetCaption(title, titlemin);
	SDL_WM_SetIcon(SDL_LoadBMP(iconpath), NULL);
}
void InputHandling(SDL_Event event, int snake[SNAKE_MAXBLOCKS][2], int state[MAXXD10][MAXYD10], int *running, int *dead, int *currentDirection, int *formerDirection, int *currentSize, int *amountOfApples, int *maxApples, int *score, long int *frameCounter, int *divider) {
	while(SDL_PollEvent(&event)) {
		switch(event.type){
			case SDL_KEYDOWN:
				if (event.key.keysym.sym==SDLK_LEFT) {
					*currentDirection=4;
				}	else if (event.key.keysym.sym==SDLK_RIGHT) {
					*currentDirection=6;
				} else if (event.key.keysym.sym==SDLK_UP) {
					*currentDirection=8;
				} else if (event.key.keysym.sym==SDLK_DOWN) {
					*currentDirection=2;
				} 
				if (event.key.keysym.sym==SDLK_RETURN) {
					PlayAgain(snake, state, running, dead, currentDirection, formerDirection, currentSize, amountOfApples, maxApples, score);				
				}
				if (event.key.keysym.sym==SDLK_SPACE) { *divider = 2;	} else { *divider = 5; }
				if (event.key.keysym.sym==SDLK_ESCAPE) { AManIsCoughingInBrazil(0); }
				break;
			case SDL_QUIT:
				AManIsCoughingInBrazil(0);
				break;
		}
	}
	if (*currentDirection==4 & *formerDirection==6) { 
		*currentDirection=*formerDirection;
	} else if (*currentDirection==6 & *formerDirection==4) { 
		*currentDirection=*formerDirection;
	} else if (*currentDirection==8 & *formerDirection==2) { 
		*currentDirection=*formerDirection;
	} else if (*currentDirection==2 & *formerDirection==8) { 
		*currentDirection=*formerDirection;
	} else {
		if (IsActionFrame(frameCounter, divider)) { *formerDirection=*currentDirection; }
	}
}
void Drawing(SDL_Surface *screen, int state[MAXXD10][MAXYD10], int *dead, int *score) {
	boxColor(screen, 0, 0, SNAKE_MAXX, SNAKE_MAXY, 0xF5F5DCFF); // clear screen with BEIGE
	boxColor(screen, 10, 10, SNAKE_MAXX-11, SNAKE_MAXY-11, 0x000000FF); // interior is black
	for (int i=1;i<(MAXXD10-1);i++) {
		for (int j=1;j<(MAXYD10-1);j++) {
			int it10 = i*10, jt10 = j*10;
			if (state[i][j]==1) { boxColor(screen, it10, jt10, it10+9, jt10+9, 0xFF1111FF); }
			if (state[i][j]==2) { DrawEntireBitmap(screen, "apple.bmp", it10, jt10); }
		}
	}
	if (*dead) {
		DrawEntireBitmap(screen, "gameover.bmp", (SNAKE_MAXX/2)-80, (SNAKE_MAXY/2)-60);
		char buf[81]; buf[80] = 0;
		snprintf(buf, 80, "Score: %d", *score);
		stringColor(screen, (SNAKE_MAXX/2)-30, (SNAKE_MAXX/2), buf, 0x999999FF);
		stringColor(screen, (SNAKE_MAXX/2)-100, (SNAKE_MAXX/2)+20, "Press ENTER to play again.", 0x999999FF);
		stringColor(screen, (SNAKE_MAXX/2)-80, (SNAKE_MAXX/2)+30, "Press ESCAPE to quit.", 0x999999FF);
		SDL_Flip(screen);
	} else {
		SDL_Flip(screen);
	}
}
void DrawEntireBitmap(SDL_Surface *screen, char *path, int x, int y) {
	SDL_Surface *image;
	SDL_Surface *temp;
	
	temp = SDL_LoadBMP(path);
	if (temp == NULL) {
		fprintf(stderr, "Unable to load bitmap: %s\n", SDL_GetError());
		AManIsCoughingInBrazil(1);
	}
		
	image = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	SDL_Rect src, dest;
 
	src.x = 0;
	src.y = 0;
	src.w = image->w;
	src.h = image->h;
	
	dest.x = x;
	dest.y = y;
	dest.w = image->w;
	dest.h = image->h;
	
	SDL_BlitSurface(image, &src, screen, &dest);
}
void InitialSnakeSetup(int snake[SNAKE_MAXBLOCKS][2], int state[MAXXD10][MAXYD10]) {
	for (int i=0;i<SNAKE_MAXBLOCKS;i++) {
		snake[i][0] = 0;
		snake[i][1] = 0;
	}
	snake[0][0] = MAXXD10/2; // in this case, 32
	snake[0][1] = MAXYD10/2; // in this case, 24
	for (int i=1;i<SNAKE_STARTBLOCKS;i++) {
		snake[i][0] = snake[i-1][0]+1;
		snake[i][1] = snake[i-1][1];
	}
	
	for (int i=0;i<MAXXD10;i++) {
		for (int j=0;j<MAXYD10;j++) {
			state[i][j] = 0;
		}
	}
	for (int i=0;i<SNAKE_STARTBLOCKS;i++) {
		int tempx = snake[i][0];
		int tempy = snake[i][1];
		state[tempx][tempy] = 1;
	}
}
void GenerateRandomlyPlacedApple(int *amountOfApples, int maxApples, int state[MAXXD10][MAXYD10]) {
  if (*amountOfApples<maxApples) {
		int applex = GenerateNumber(MAXXD10-2)+1, appley = GenerateNumber(MAXYD10-2)+1;
		while (state[applex][appley]==1) {
			applex = GenerateNumber(MAXXD10-2)+1; appley = GenerateNumber(MAXYD10-2)+1;
		}
		state[applex][appley] = 2;
		*amountOfApples = *amountOfApples + 1;
  }
}
void AppleCollisionHandling(int state[MAXXD10][MAXYD10], int snake[SNAKE_MAXBLOCKS][2], int *currentSize, int *amountOfApples, int *score) {
	if (state[snake[0][0]][snake[0][1]] == 2) {
		if (*currentSize<SNAKE_MAXBLOCKS) { (*currentSize)++; }
		state[snake[0][0]][snake[0][1]] = 0;
		(*amountOfApples)--;
		(*score)++;
	}
}
void WallCollisionHandling(int snake[SNAKE_MAXBLOCKS][2], int *dead) {
	if (snake[0][0]<1 | snake[0][1]<1 | snake[0][0]>=(MAXXD10-1) | snake[0][1]>=(MAXYD10-1)) { 
		printf("Collision with wall, game over!\n");
		*dead = 1;
	}
}
void SelfCollisionHandling(int snake[SNAKE_MAXBLOCKS][2], int *dead) {
	for (int i=4;i<SNAKE_MAXBLOCKS;i++) {
		if (snake[0][0] == snake[i][0] & snake[0][1] == snake[i][1]) {
			printf("Collision with self, game over!\n");
			*dead = 1;
		}
	}
}
void MoveSnake(int snake[SNAKE_MAXBLOCKS][2], int state[MAXXD10][MAXYD10], int *currentDirection, int *currentSize, int *amountOfApples, int *score, int *dead) {
	int copysnake [SNAKE_MAXBLOCKS][2];
	for (int i=0;i<SNAKE_MAXBLOCKS;i++) {
		copysnake[i][0] = snake[i][0];
		copysnake[i][1] = snake[i][1];
		state[copysnake[i][0]][copysnake[i][1]] = 0;
	}
	if (*currentDirection==4) {
		snake[0][0] = snake[0][0] - 1;
	} else if (*currentDirection==6) {
		snake[0][0] = snake[0][0] + 1;
	} else if (*currentDirection==8) {
		snake[0][1] = snake[0][1] - 1;
	} else if (*currentDirection==2) {
		snake[0][1] = snake[0][1] + 1;
	}
	
	AppleCollisionHandling(state, snake, currentSize, amountOfApples, score);
	
	WallCollisionHandling(snake, dead);
	
	// follow the leader
	state[snake[0][0]][snake[0][1]] = 1;
	for (int i=1;i<*currentSize;i++) {
		snake[i][0] = copysnake[i-1][0];
		snake[i][1] = copysnake[i-1][1];
		state[snake[i][0]][snake[i][1]] = 1;
	}
	
	SelfCollisionHandling(snake, dead);
}
void ResetVariables(int *running, int *dead, int *currentDirection, int *formerDirection, int *currentSize, int * amountOfApples, int *maxApples, int *score) {
	*running = 1; *dead = 0;
	*currentDirection=4; *formerDirection=4;
	*currentSize = SNAKE_STARTBLOCKS;
	*amountOfApples = 0; *maxApples = 1;
	*score = 0;
}
void PlayAgain(int snake[SNAKE_MAXBLOCKS][2], int state[MAXXD10][MAXYD10], int *running, int *dead, int *currentDirection, int *formerDirection, int *currentSize, int * amountOfApples, int *maxApples, int *score) {
	ResetVariables(running, dead, currentDirection, formerDirection, currentSize, amountOfApples, maxApples, score);
	InitialSnakeSetup(snake, state);
}
int IsActionFrame(long int *frameCounter, int *divider) {
	if (*frameCounter % *divider == 0) { return 1; } else { return 0; }
}
