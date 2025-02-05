#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

const int OBSTACLE_WIDTH = 50;
const int OBSTACLE_HEIGHT = 50;
const int OBSTACLE_SPEED = 5;
const int OBSTACLE_SPAWN_INTERVAL = 150; 

struct Obstacle {
    SDL_Rect rect;  
    SDL_Color color; 
};

std::vector<Obstacle> obstacles; 
int frameCounter = 0;            
bool gameOver = false;           

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int RECT_WIDTH = 50;
const int RECT_HEIGHT = 50;
const int RECT_SPEED = 10;

int roadOffset = 0;         
const int SCROLL_SPEED = 5;

SDL_Window *window = NULL;
SDL_Renderer *windowRenderer = NULL;
SDL_Event currentEvent;

bool quit = false;

int mouseX, mouseY;

const int timeStep = 10; 
Uint32 lastUpdateTime = 0; 

bool initWindow() {
  bool success = true;

  // Try to initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "SDL initialization failed" << std::endl;
    success = false;
  } else {
    // Try to create the window
    window = SDL_CreateWindow("Hello SDL2 Window!", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                              WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL) {
      std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
      success = false;
    } else {
      // Create a renderer for the current window
      windowRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

      if (windowRenderer == NULL) {
        std::cout << "Failed to create the renderer: " << SDL_GetError()
                  << std::endl;
        success = false;
      } else {
        // Set background color
        SDL_SetRenderDrawColor(windowRenderer, 255, 255, 255, 255);
        // Apply background color
        SDL_RenderClear(windowRenderer);
      }
    }
  }

  return success;
}
void spawnObstacle() {
  int leftLaneX = 250;  
  int rightLaneX = 450;
  int spawnX = (rand() % 2 == 0) ? leftLaneX : rightLaneX; // Random lane

  // Generate a random color for the obstacle
  SDL_Color obstacleColor = {
    (Uint8)(rand() % 256),  // Random red value
    (Uint8)(rand() % 256),  // Random green value
    (Uint8)(rand() % 256),  // Random blue value
    255                    // Fully opaque
  };

  // Create a new obstacle with a random color
  SDL_Rect newObstacle = {spawnX, -OBSTACLE_HEIGHT, OBSTACLE_WIDTH, OBSTACLE_HEIGHT};
  obstacles.push_back({newObstacle, obstacleColor}); // Add both rect and color
}

void updateObstacles() {
  for (auto &obstacle : obstacles) {
    obstacle.rect.y += OBSTACLE_SPEED;
  }

  // Remove obstacles that move off-screen
  obstacles.erase(
      std::remove_if(obstacles.begin(), obstacles.end(),
                     [](Obstacle &o) { return o.rect.y > WINDOW_HEIGHT; }),
      obstacles.end());
}

void drawObstacles() {
  // Loop through each obstacle and draw it with its stored color
  for (const auto &obstacle : obstacles) {
    SDL_SetRenderDrawColor(windowRenderer, obstacle.color.r, obstacle.color.g, obstacle.color.b, obstacle.color.a); // Set the color

    // Draw the obstacle
    SDL_RenderFillRect(windowRenderer, &obstacle.rect);
  }
}

bool leftPressed = false;
bool rightPressed = false;

void updateMovement(SDL_Rect &rect) {
  int leftBorderX = 200;
  int rightBorderX = 600;

  if (leftPressed && rect.x > leftBorderX) {
    rect.x -= RECT_SPEED; // Move left smoothly
  }

  if (rightPressed && rect.x + RECT_WIDTH < rightBorderX) {
    rect.x += RECT_SPEED; // Move right smoothly
  }
}

void processEvents() {
  while (SDL_PollEvent(&currentEvent)) {
    if (currentEvent.type == SDL_QUIT) {
      quit = true;
    }

    if (currentEvent.type == SDL_KEYDOWN) {
      if (currentEvent.key.keysym.sym == SDLK_LEFT) {
        leftPressed = true;
      }
      if (currentEvent.key.keysym.sym == SDLK_RIGHT) {
        rightPressed = true;
      }
      if (currentEvent.key.keysym.sym == SDLK_ESCAPE) {
        quit = true;
      }
    }

    if (currentEvent.type == SDL_KEYUP) {
      if (currentEvent.key.keysym.sym == SDLK_LEFT) {
        leftPressed = false;
      }
      if (currentEvent.key.keysym.sym == SDLK_RIGHT) {
        rightPressed = false;
      }
    }
  }
}
void drawFrame(SDL_Rect &rect) {
  // Clear the background
  SDL_SetRenderDrawColor(windowRenderer, 255, 255, 255,
                         255); // White background
  SDL_RenderClear(windowRenderer);

  // Draw the scrolling road
  int roadHeight = WINDOW_HEIGHT; // The height of each road segment
  SDL_Rect road1 = {WINDOW_WIDTH / 4, roadOffset, WINDOW_WIDTH / 2, roadHeight};
  SDL_Rect road2 = {WINDOW_WIDTH / 4, roadOffset - roadHeight, WINDOW_WIDTH / 2,
                    roadHeight};

  SDL_SetRenderDrawColor(windowRenderer, 128, 128, 128, 255); // Grey road color
  SDL_RenderFillRect(windowRenderer, &road1);
  SDL_RenderFillRect(windowRenderer, &road2);

  // Draw the left blue border
  SDL_Rect leftBorder1 = {WINDOW_WIDTH / 4 - 10, roadOffset, 10, roadHeight};
  SDL_Rect leftBorder2 = {WINDOW_WIDTH / 4 - 10, roadOffset - roadHeight, 10,
                          roadHeight};
  SDL_SetRenderDrawColor(windowRenderer, 0, 0, 255, 255); // Blue color
  SDL_RenderFillRect(windowRenderer, &leftBorder1);
  SDL_RenderFillRect(windowRenderer, &leftBorder2);

  // Draw the right blue border
  SDL_Rect rightBorder1 = {WINDOW_WIDTH / 4 + WINDOW_WIDTH / 2, roadOffset, 10,
                           roadHeight};
  SDL_Rect rightBorder2 = {WINDOW_WIDTH / 4 + WINDOW_WIDTH / 2,
                           roadOffset - roadHeight, 10, roadHeight};
  SDL_RenderFillRect(windowRenderer, &rightBorder1);
  SDL_RenderFillRect(windowRenderer, &rightBorder2);

  // Draw the yellow stripes down the middle of the road
  int stripeWidth = 20;
  int stripeSpacing = 120;
  int stripeYPos = roadOffset % stripeSpacing;

  SDL_SetRenderDrawColor(windowRenderer, 255, 255, 0, 255); // Yellow color

  for (int i = 0; i < roadHeight + stripeSpacing; i += stripeSpacing) {
    SDL_Rect stripe = {WINDOW_WIDTH / 2 - stripeWidth / 2, -60 + stripeYPos + i,
                       stripeWidth, 60};
    SDL_RenderFillRect(windowRenderer, &stripe);
  }

  // Scroll the road
  roadOffset += SCROLL_SPEED;
  if (roadOffset >= roadHeight) {
    roadOffset = 0; // Reset offset when a segment scrolls off-screen
  }

  // Draw the rectangle
  SDL_SetRenderDrawColor(windowRenderer, 255, 0, 0, 255); // Red rectangle
  SDL_RenderFillRect(windowRenderer, &rect);
  drawObstacles();

  // Update the window
  SDL_RenderPresent(windowRenderer);
}

// Check if two rectangles collide
bool checkCollision(SDL_Rect a, SDL_Rect b) {
  return (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h &&
          a.y + a.h > b.y);
}

// Check if the game is over
void checkGameOver(SDL_Rect &rect) {
  for (const auto &obstacle : obstacles) {
    if (checkCollision(rect, obstacle.rect)) {
      gameOver = true;
    }
  }
}

// Cleanup SDL
void cleanup() {
  // Destroy renderer
  if (windowRenderer) {
    SDL_DestroyRenderer(windowRenderer);
    windowRenderer = NULL;
  }

  // Destroy window
  if (window) {
    SDL_DestroyWindow(window);
    window = NULL;
  }

  // Quit SDL
  SDL_Quit();
}

int main(int argc, char *argv[]) {
  // Initialize window
  if (!initWindow()) {
    std::cout << "Failed to initialize" << std::endl;
    return -1;
  }

  SDL_Rect rect = {WINDOW_WIDTH / 2 - RECT_WIDTH / 2, 450, RECT_WIDTH,
                   RECT_HEIGHT};

  Uint32 lastTime = SDL_GetTicks();

  // Game loop
  while (!quit && !gameOver) { // Exit when gameOver is true
    Uint32 currentTime = SDL_GetTicks();

    // Process events (only once per frame)
    processEvents();
    updateMovement(rect);
    updateObstacles();
    checkGameOver(rect);
    
    // Update game logic based on timeStep (e.g., every 100 ms)
    if (currentTime - lastTime >= timeStep) {
      lastTime = currentTime;

      // Slow the movement of the game (by limiting logic updates)
      drawFrame(rect); // Render frame continuously
    }

    if (frameCounter % OBSTACLE_SPAWN_INTERVAL == 0) {
      spawnObstacle();
    }
    frameCounter++;

    // Delay to cap frame rate around 60 FPS (~16ms delay)
    SDL_Delay(16); // ~60 FPS
  }

  // Display game over message
  if (gameOver) {
    SDL_SetRenderDrawColor(windowRenderer, 255, 0, 0, 255); // Red game-over color
    SDL_RenderClear(windowRenderer);
    SDL_RenderPresent(windowRenderer);
    SDL_Delay(1000); // Pause for 1 second before quitting
  }

  cleanup();
  return 0;
}
