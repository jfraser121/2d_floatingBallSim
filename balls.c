#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600
#define NUM_BALLS 50
#define DT 0.01  //timediff
#define STEPS 1000 //num simulation steps

typedef struct {
	double x, y;   // position
	double vx, vy; // velocity
	double r;      // radius
	Uint8 cr, cg, cb; // color (red, green, blue)
} Ball;


void drawBall(SDL_Renderer *renderer, Ball b);
int checkCollision(Ball a, Ball b);
void resolveCollision(Ball *a, Ball *b);
void generateBalls(Ball balls[], int numBalls);
void updatePositions(Ball balls[], int numBalls);
void handleWallCollisions(Ball balls[], int numBalls);
void handleBallCollisions(Ball balls[], int numBalls);


int main(void) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Ball Sim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	Ball balls[NUM_BALLS];
	generateBalls(balls, NUM_BALLS);
// Generate balls with random positions and velocities

	for (int step = 0; step < STEPS; step++) {
		printf("step %i:\n", step);

		updatePositions(balls, NUM_BALLS);
		handleWallCollisions(balls, NUM_BALLS);
		handleBallCollisions(balls, NUM_BALLS);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		for (int i = 0; i < NUM_BALLS; i++) {
			drawBall(renderer, balls[i]);
		}

		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void drawBall(SDL_Renderer *renderer, Ball b){
	SDL_SetRenderDrawColor(renderer, b.cr, b.cg, b.cb, 255);
	for (int w = -b.r; w < b.r; w++) {
		for (int h = -b.r; h < b.r; h++) {
			if (w*w + h*h <= b.r*b.r) {
				SDL_RenderDrawPoint(renderer, (int)(b.x + w), (int)(b.y + h));
			}
		}
	}
}

void generateBalls(Ball balls[], int numBalls) {
	srand((unsigned int)time(NULL));
	for (int i = 0; i < numBalls; i++) {
		balls[i].r = 10 + rand() % 21; // radius between 10 and 30
		balls[i].x = balls[i].r + rand() % (WIDTH - 2 * (int)balls[i].r);
		balls[i].y = balls[i].r + rand() % (HEIGHT - 2 * (int)balls[i].r);
		balls[i].vx = (rand() % 201 - 100); // velocity between -100 and 100
		balls[i].vy = (rand() % 201 - 100);
		balls[i].cr = rand() % 256; // random red
		balls[i].cg = rand() % 256; // random green
		balls[i].cb = rand() % 256; // random blue
	}
}

int checkCollision(Ball a, Ball b) {
	double dx = a.x - b.x;
	double dy = a.y - b.y;
	double distance = sqrt(dx * dx + dy * dy);
	return distance < (a.r + b.r);
}	

void resolveCollision(Ball *a, Ball *b) {
	double dx = b->x - a->x;
	double dy = b->y - a->y;
	double distance = sqrt(dx * dx + dy * dy);
	if (distance == 0) return; // Prevent division by zero

	double nx = dx / distance;
	double ny = dy / distance;

	double p = 2 * (a->vx * nx + a->vy * ny - b->vx * nx - b->vy * ny) / 2; // assumes equal mass

	a->vx -= p * nx;
	a->vy -= p * ny;
	b->vx += p * nx;
	b->vy += p * ny;
}

// Update positions of all balls
void updatePositions(Ball balls[], int numBalls) {
	for (int i = 0; i < numBalls; i++) {
		balls[i].x += balls[i].vx * DT;
		balls[i].y += balls[i].vy * DT;
	}
}

// Handle collisions with window boundaries
void handleWallCollisions(Ball balls[], int numBalls) {
	for (int i = 0; i < numBalls; i++) {
		if (balls[i].x - balls[i].r < 0 || balls[i].x + balls[i].r > WIDTH) {
			balls[i].vx = -balls[i].vx;
		}
		if (balls[i].y - balls[i].r < 0 || balls[i].y + balls[i].r > HEIGHT) {
		}	
	}
}

// Handle collisions between all pairs of balls
void handleBallCollisions(Ball balls[], int numBalls) {
	for (int i = 0; i < numBalls; i++) {
		for (int j = i + 1; j < numBalls; j++) {
			if (checkCollision(balls[i], balls[j])) {
				resolveCollision(&balls[i], &balls[j]);
			}
		}
	}
}

