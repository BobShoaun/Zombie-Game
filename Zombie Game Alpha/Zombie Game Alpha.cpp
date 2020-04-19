#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <vector>

const int screenWidth = 100, screenHeight = 20;
char screen [screenWidth] [screenHeight];

int score = 0;
bool screenUpdated = false;

void toggleConsoleCursor (bool);
void clearScreen ();

enum Direction { up, down, left, right };

struct Vector2 {
	public:
	int x, y;
	Vector2 ();
	Vector2 (int, int);
};

Vector2::Vector2 () {
	x = 0;
	y = 0;
};

Vector2::Vector2 (int x, int y) : x (x), y (y) {}

class Graphics {
	private:

	public:
	void clear ();
	//void render (char [][]);
};

class Collider {
	private:
	Vector2 position;
	Vector2 dimensions;
	static int collisionMatrix [screenWidth] [screenHeight];
	public:
	Collider (Vector2, Vector2);
	void clearGraphics ();
	void setPosition (Vector2);
	void extrude (Direction, int);
	bool checkCollision (Vector2&);
};

Collider::Collider (Vector2 position, Vector2 dimensions) : position (position), dimensions (dimensions) {}

void Collider::setPosition (Vector2 position) {
	this->position = position;
}

void Collider::clearGraphics () {
	for (int x = position.x; x < position.x + dimensions.x; x++) {
		for (int y = position.y; y < position.y + dimensions.y; y++) {
			screen [x] [y] = ' ';
		}
	}
}

void Collider::extrude (Direction direction, int amount) {
	switch (direction) {
		case up:
			position.y -= amount;
			dimensions.y += amount;
			break;
		case down:
			dimensions.y += amount;
			break;
		case left:
			position.x -= amount;
			dimensions.x += amount;
			break;
		case right:
			dimensions.x += amount;
	}
}

bool Collider::checkCollision (Vector2& collidedPosition) {
	for (int x = position.x; x < position.x + dimensions.x; x++) {
		for (int y = position.y; y < position.y + dimensions.y; y++) {
			if (screen [x] [y] != 0) {
				collidedPosition = Vector2 (x, y);
				return true;
			}
		}
	}
	return false;
}

class Zombie {
	private:
	Vector2 position;
	int moveTimer;
	int moveInterval = 10;
	public:
	Zombie (Vector2);
	Collider collider;
	void standPose ();
	void update ();
	void move ();
};

Zombie::Zombie (Vector2 position) : position (position.x, position.y), collider (Vector2 (position.x - 1, position.y - 1), Vector2 (3, 3)) {
	moveTimer = 0;
	standPose ();
};

void Zombie::standPose () {
	screen [position.x] [position.y] = 'l';
	screen [position.x] [position.y - 1] = 'c';
	screen [position.x] [position.y + 1] = '`';
	screen [position.x + 1] [position.y + 1] = 92; // '\'
	screen [position.x - 1] [position.y + 1] = '/';
	screen [position.x + 1] [position.y] = '`'; // '\'
	screen [position.x - 1] [position.y] = '/';
}

void Zombie::update () {
	moveTimer++;
	if (moveTimer > moveInterval) {
		move ();
		moveTimer = 0;
	}
}

void Zombie::move () {
	collider.clearGraphics ();
	position.x ++;
	collider.setPosition (Vector2 (position.x - 1, position.y - 1));
	standPose ();
	screenUpdated = true;
}

class ZombieManager {
	private:
	int waves = 0;
	int zombieSpawnTimer = 0;
	int zombieSpawnInterval = 50;
	void spawnZombie ();
	void updateZombies ();
	std::vector<Zombie> zombies;
	public:
	ZombieManager ();
	void recalculateZombiesPaths ();
	int getZombieAmount ();
	void update ();
};

ZombieManager::ZombieManager () {};

int ZombieManager::getZombieAmount () {
	return zombies.size ();
}

void ZombieManager::recalculateZombiesPaths () {

}

void ZombieManager::updateZombies () {
	for (int i = 0; i < zombies.size (); i++)
		zombies [i].update ();
}

void ZombieManager::update () {
	zombieSpawnTimer++;
	if (zombieSpawnTimer > zombieSpawnInterval) {
		spawnZombie ();
		zombieSpawnTimer = 0;
	}
	updateZombies ();
}

void ZombieManager::spawnZombie () {
	Zombie newZombie (Vector2 (rand () % screenWidth, rand () % screenHeight));
	zombies.push_back (newZombie);
	screenUpdated = true;
}

class Player {
	private:
	Vector2 position;
	Vector2 previousPosition;
	Collider collider;
	int attackTimer = 0;
	Direction attackDirection;
	int attackDuration = 5;
	char previousMovementInput;
	void standPose ();
	void attackPose (Direction);
	void move (char);
	void returnToPreviousPosition ();
	void checkWallCollision ();
	void checkFoodPickup ();
	void attack ();
	void endAttack ();
	public:
	Player (Vector2);
	void respondToInput (char);
	void update ();
};

Player::Player (Vector2 position) : position (position.x, position.y), collider (Vector2 (position.x - 1, position.y - 1), Vector2 (3, 3)) {}

void Player::returnToPreviousPosition () {
	position = previousPosition;
	collider.setPosition (Vector2 (position.x - 1, position.y - 1));
}

void Player::move (char input) {
	previousPosition = position;
	switch (input) {
		case 'w':
			position.y --;
			previousMovementInput = input;
			break;
		case 'a':
			position.x -= 2;
			previousMovementInput = input;
			break;
		case 's':
			position.y ++;
			previousMovementInput = input;
			break;
		case 'd':
			position.x += 2;
			previousMovementInput = input;
			break;
	}
	collider.setPosition (Vector2 (position.x - 1, position.y - 1));
}

void Player::update () {
	attackTimer++;
	if (attackTimer == attackDuration)
		endAttack ();
}

void Player::standPose () {
	screen [position.x] [position.y] = '|';
	screen [position.x] [position.y - 1] = 'O';
	screen [position.x] [position.y + 1] = '`';
	screen [position.x + 1] [position.y + 1] = 92; // '\'
	screen [position.x - 1] [position.y + 1] = '/';
	screen [position.x + 1] [position.y] = 92; // '\'
	screen [position.x - 1] [position.y] = '/';
}

void Player::attackPose (Direction direction) {
	switch (direction) {
		case up:
			screen [position.x] [position.y] = '|';
			screen [position.x] [position.y - 1] = 'O';
			screen [position.x] [position.y + 1] = '`';
			screen [position.x + 1] [position.y + 1] = 92; // '\'
			screen [position.x - 1] [position.y + 1] = '/';
			screen [position.x - 1] [position.y - 1] = 92;
			screen [position.x + 1] [position.y - 1] = '/';
			break;
		case down:
			screen [position.x] [position.y] = '|';
			screen [position.x] [position.y - 1] = 'O';
			screen [position.x] [position.y + 1] = '|';
			screen [position.x] [position.y + 2] = '`';
			screen [position.x + 1] [position.y + 2] = 92; // '\'
			screen [position.x - 1] [position.y + 2] = '/';
			screen [position.x + 1] [position.y] = 92; // '\'
			screen [position.x - 1] [position.y] = '/';
			break;
		case left:
			screen [position.x] [position.y] = '|';
			screen [position.x] [position.y - 1] = 'O';
			screen [position.x] [position.y + 1] = '`';
			screen [position.x + 1] [position.y + 1] = 92; // '\'
			screen [position.x - 1] [position.y + 1] = '/';
			screen [position.x + 1] [position.y] = 92; // '\'
			screen [position.x - 1] [position.y - 1] = '_';
			screen [position.x - 2] [position.y - 1] = '_';
			break;
		case right:
			screen [position.x] [position.y] = '|';
			screen [position.x] [position.y - 1] = 'O';
			screen [position.x + 1] [position.y - 1] = '_';
			screen [position.x + 2] [position.y - 1] = '_';
			screen [position.x] [position.y + 1] = '`';
			screen [position.x + 1] [position.y + 1] = 92; // '\'
			screen [position.x - 1] [position.y + 1] = '/';
			screen [position.x - 1] [position.y] = '/';
			break;
	}
}

void Player::attack () {
	attackTimer = 0;
	switch (previousMovementInput) {
		case 'w':
			attackDirection = up;
			collider.extrude (attackDirection, 2);
			checkFoodPickup ();
			break;
		case 'a':
			attackDirection = left;
			collider.extrude (attackDirection, 2);
			checkFoodPickup ();
			break;
		case 's':
			attackDirection = down;
			collider.extrude (attackDirection, 2);
			checkFoodPickup ();
			break;
		case 'd':
			attackDirection = right;
			collider.extrude (attackDirection, 2);
			checkFoodPickup ();
			break;
	}
	attackPose (attackDirection);
}

void Player::endAttack () {
	collider.clearGraphics ();
	collider.extrude (attackDirection, -2);
	standPose ();
	screenUpdated = true;
}

void Player::respondToInput (char input) {
	if (attackTimer < 5)
		return;
	collider.clearGraphics ();
	if (input == ' ') {
		attack ();
	} else {
		move (input);
		checkWallCollision ();
		checkFoodPickup ();
		standPose ();
	}
	screenUpdated = true;
}

void Player::checkWallCollision () {
	Vector2 pos;
	if (collider.checkCollision (pos))
		if (screen [pos.x] [pos.y] == '|' || screen [pos.x] [pos.y] == '_')
			returnToPreviousPosition ();
}

void Player::checkFoodPickup () {
	Vector2 pos;
	if (collider.checkCollision (pos)) {
		if (screen [pos.x] [pos.y] == '@') {
			score++;
			//if (score == 10)
			//over ();
			screen [pos.x] [pos.y] = ' ';
		}
	}
}

class Game {
	private:
	std::string name = "Food Run";
	char previousMovement;
	int framesPerSecond;
	//int score = 0;
	bool running;
	void initialize ();
	void update ();
	void resetScreen ();
	void draw ();
	void spawnFood (int);
	Player player;
	ZombieManager zombieManager;
	//bool screenUpdated = true;
	public:
	Game (int);
	void play ();
	void over ();
};

Game::Game (int framesPerSecond) : player (Vector2 (5, 5)), framesPerSecond (framesPerSecond) {}

void Game::over () {
	resetScreen ();
	screen [screenWidth / 2 - 4] [screenHeight / 2] = 'G';
	screen [screenWidth / 2 - 3] [screenHeight / 2] = 'A';
	screen [screenWidth / 2 - 2] [screenHeight / 2] = 'M';
	screen [screenWidth / 2 - 1] [screenHeight / 2] = 'E';
	screen [screenWidth / 2] [screenHeight / 2] = ' ';
	screen [screenWidth / 2 + 1] [screenHeight / 2] = 'O';
	screen [screenWidth / 2 + 2] [screenHeight / 2] = 'V';
	screen [screenWidth / 2 + 3] [screenHeight / 2] = 'E';
	screen [screenWidth / 2 + 4] [screenHeight / 2] = 'R';
}

void Game::play () {
	running = true;
	initialize ();
	while (running) {
		update ();
		Sleep (100);
	}
}

void Game::initialize () {
	toggleConsoleCursor (false);
	srand (time (0));
	resetScreen ();
	spawnFood (10);
}

void Game::update () {
	zombieManager.update ();
	player.update ();
	if (kbhit ())
		player.respondToInput (getch ());
	if (screenUpdated) {
		screenUpdated = false;
		draw ();
	}
}

void Game::draw () {
	system ("cls");
	for (int height = 0; height < screenHeight; height++) {
		for (int width = 0; width < screenWidth; width++)
			std::cout << screen [width] [height];
		std::cout << std::endl;
	}
	std::cout << "Score : " << score << std::endl;
	std::cout << "Zombies : " << zombieManager.getZombieAmount () << std::endl;
}

void Game::resetScreen () {
	for (int height = 0; height < screenHeight; height++) {
		for (int width = 0; width < screenWidth; width++) {
			screen [width] [height] = ' ';
			if (height == 0 || height == screenHeight - 1)
				screen [width] [height] = '_';
		}
		screen [0] [height] = screen [screenWidth - 1] [height] = '|';
	}
	screen [0] [0] = screen [screenWidth - 1] [0] = ' ';
}

void Game::spawnFood (int amount) {
	for (int i = 0; i < amount; i++)
		screen [rand () % screenWidth] [rand () % screenHeight] = '@';
}

int main () {
	Game game (60);
	game.play ();
}

void toggleConsoleCursor (bool isVisible) {
	HANDLE out = GetStdHandle (STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo (out, &cursorInfo);
	cursorInfo.bVisible = isVisible; // set the cursor visibility
	SetConsoleCursorInfo (out, &cursorInfo);
}

void clearScreen () {
	DWORD n;                         /* Number of characters written */
	DWORD size;                      /* number of visible characters */
	COORD coord = { 0 };               /* Top left screen position */
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	/* Get a handle to the console */
	HANDLE h = GetStdHandle (STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo (h, &csbi);
	/* Find the number of characters to overwrite */
	size = csbi.dwSize.X * csbi.dwSize.Y;
	/* Overwrite the screen buffer with whitespace */
	FillConsoleOutputCharacter (h, TEXT (' '), size, coord, &n);
	GetConsoleScreenBufferInfo (h, &csbi);
	FillConsoleOutputAttribute (h, csbi.wAttributes, size, coord, &n);
	/* Reset the cursor to the top left position */
	SetConsoleCursorPosition (h, coord);
}