#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;
Color green = { 173,204,96,255 }; //to determine the color values 
Color darkGreen = { 43,51,24,255 };

int cellSize = 30;
int cellCount = 25; //30x25=750px
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque) {
	for (unsigned int i = 0; i < deque.size(); i++) {
		if (Vector2Equals(deque[i], element)) {
			return true;
		}
	}
	return false;
}

bool eventTriggered(double internval) {
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= internval) {
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

class Snake {
public:
	deque <Vector2> body = { Vector2{6,9}, Vector2{5,9},Vector2{4,9} };
	Vector2 direction = { 1,0 };
	bool addSegment = false;

	void Draw() {
		for (unsigned int i = 0; i < body.size(); i++) 
		{
			float x = body[i].x;
			float y = body[i].y;
			Rectangle segment = Rectangle{offset+ x * cellSize ,offset + y * cellSize , (float)cellSize , (float)cellSize};
			DrawRectangleRounded(segment,0.5,6,darkGreen);
		};
	}
	void Update() {
		body.push_front(Vector2Add(body[0], direction));
		if (addSegment == true) {
			addSegment = false;
		}
		else {
			body.pop_back();
		}
	}
	void Reset() {
		body = { Vector2{6,9},Vector2{5,9},Vector2{4,9} };
		direction = { 1,0 };
	}
};

class Food { //to define food

public:
	Vector2 position; // to define the postions
	Texture2D texture;

	//constructor
	Food( deque<Vector2> snakeBody) { 	
		Image image = LoadImage("F:/Games/RetroSnake/food.png");
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		position = GenrateRandomPos(snakeBody);
	}
	~Food() {
		UnloadTexture(texture);
	}
	void Draw() {
		DrawTexture(texture,offset + position.x * cellSize , offset + position.y * cellSize ,WHITE);
	}
	Vector2 GenrateRandomCell() {
		float x = GetRandomValue(0, cellCount - 1); //0 to 24
		float y = GetRandomValue(0, cellCount - 1); //0 to 24
		return Vector2{ x, y };
	}

	Vector2 GenrateRandomPos(deque<Vector2> snakebody ) // to genrate the random postion
	{
		float x = GetRandomValue(0, cellCount - 1); //0 to 24
		float y = GetRandomValue(0, cellCount - 1); //0 to 24
		Vector2 position = GenrateRandomCell();
		while (ElementInDeque(position, snakebody)) {
			position = GenrateRandomCell();
		}
		return position;
	}
};

class Game {
public:
	Snake snake = Snake();
	Food food = Food(snake.body);
	bool running = true;
	int score = 0;

	void Draw() {
		food.Draw();
		snake.Draw();
	}
	void Update() {
		if (running) {
			snake.Update();
			CheckCollisionWithFood();
			CheckCollisionWithEdge();
			CheckCollisionWithTail();
		}
	}
	void CheckCollisionWithFood() {
		if (Vector2Equals(snake.body[0], food.position)) {
			food.position = food.GenrateRandomPos(snake.body);
			snake.addSegment = true; 
			score++;
		}
	}
	void CheckCollisionWithEdge() {
    // Left or right edge
    if (snake.body[0].x < 0 || snake.body[0].x >= cellCount) {
        GameOver();
    }

    // Top or bottom edge
    if (snake.body[0].y < 0 || snake.body[0].y >= cellCount) {
        GameOver();
    }
}

	void CheckCollisionWithTail() {
		deque<Vector2> headlessBody = snake.body;
		headlessBody.pop_front();
		if (ElementInDeque(snake.body[0], headlessBody)) {
			GameOver();
		}
	}
	void GameOver() {
		snake.Reset();
		food.position = food.GenrateRandomPos(snake.body);
		running = false;
		score = 0;
	}
};

int main()
{
	cout << "starting a new game....." << endl;
	InitWindow(2*offset+cellSize * cellCount, 2*offset+cellSize * cellCount, "Retro Snake");// to make the clear window size of game here	are 3 args.(width,height, name)
	SetTargetFPS(60);  //to set the desired fps of the game

	Game game = Game();

	while (WindowShouldClose() == false ) //if user hits cross button the window is sexited and it is closed
	{
		BeginDrawing(); //the raylib librabry will start importing graphics

		if (eventTriggered(0.2))
		{
			game.Update();
		}

		if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
			game.snake.direction = { 0,-1 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
			game.snake.direction = { 0,1 };
			game.running = true;
		}
		if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
			game.snake.direction = {-1,0};
			game.running = true;
		}
		if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
			game.snake.direction = { 1,0 };
			game.running = true;
		}


		//drawing
		ClearBackground(green); //to display the window green 
		DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize*cellCount+10 , (float)cellSize*cellCount+10 }, 5, darkGreen);
		DrawText("RetroSnake", offset - 5, 20, 40, darkGreen);
		DrawText(TextFormat("%i",game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
		game.Draw();


		EndDrawing();// will stop importing graphics
	}

	CloseWindow();
	return 0;
}