#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <list>
#include <set>
#include <cmath>
#include <string>

const int screenWidth = 100, screenHeight = 25;

enum Direction { up, down, left, right };

struct Vector2 {
	public:
	int x, y;
	Vector2 ();
	Vector2 (int, int);
	bool operator == (const Vector2 & vector2) const {
		return x == vector2.x && y == vector2.y;
	}
	bool operator != (const Vector2 & vector2) const {
		return !(*this == vector2);
	}
};

class Collider {
	private:
	int id = 0;
	static std::vector<Collider*> colliders;
	static int currentId;
	virtual void collided (Collider *) = 0;
	protected:
	Vector2 position, previousPosition, dimensions;
	void clear (Vector2);
	void setPosition (Vector2);
	void updateCollisionMatrix ();
	void checkCollisionMatrix ();
	public:
	Collider (Vector2, Vector2);
	void clear ();
	Vector2 getPosition ();
	static int collisionMatrix [screenWidth] [screenHeight];
};

class Graphics {
	private:
	Vector2 position, dimensions;
	public:
	static bool screenUpdated;
	static char screen [screenWidth] [screenHeight];
	Graphics (Vector2, Vector2);
	void clear ();
	void render (char);
	void render (char []);
	void setPosition (Vector2);
	void extrude (Direction, int);
};

class Wall : public Collider {
	private:
	Graphics graphics;
	void collided (Collider *);
	void updateWalkableAreas ();
	static bool initialized;
	public:
	Wall (Vector2, Vector2);
	static bool walkableAreas [screenWidth] [screenHeight];
};

class Node {
	private:
	public:
	Vector2 parentCoord;
	Vector2 coord;
	int gCost, hCost;
	bool walkable;
	int randomValue = rand ();
	Node ();
	Node (bool, Vector2);
	static int getDistance (Node, Node);
	static bool nodeInList (std::vector<Node>&a, Node);
	static void removeNodeFromVector (std::vector<Node> &vect, Node);
	int getFCost ();
	bool operator == (const Node &node) {
		return coord == node.coord;
	}
	bool operator != (const Node &node) {
		return !(* this == node);
	}
	bool operator < (const  Node &node) const {
		return randomValue > node.randomValue;
	}
};

class Grid {
	public:
	Node gridOfNode [screenWidth] [screenHeight];
	Grid ();
	std::vector<Node> getNeighbours (Node);
	void create ();
};

class PathFinder {
	private:
	public:
	static Grid grid;
	std::vector<Node> path;
	void findPath (Vector2, Vector2);
	void retracePath (Node, Node);
};

class Attack : public Collider {
	private:
	void collided (Collider *);
	bool blocked = false;
	public:
	Attack (Vector2, Vector2);
	bool getBlocked ();
};

class Player : public Collider {
	private:
	char defaultPose [9] = { ' ', 'O', ' ', '/', '|', 92, '/', '`', 92 };
	char attackUpPose [12] = { ' ', ' ', ' ', 92, 'O', '/', ' ', '|', ' ', '/', '`', 92 };
	char attackDownPose [12] = { ' ', 'O', ' ', '/', '|', 92, ' ', '|', ' ', '/', '`', 92 };
	char attackLeftPose [12] = { '_', '_', 'O', ' ', ' ', ' ', '|', 92, ' ', '/', '`', 92 };
	char attackRightPose [12] = { ' ', 'O', '_', '_', '/', '|', ' ', ' ', '/', '`', 92, ' ' };
	int attackTimer = 0;
	Direction attackDirection;
	int attackDuration = 5;
	char previousMovementInput;
	void endAttack ();
	Attack * attack = NULL;
	Graphics graphics;
	void collided (Collider *);
	int health = 10;
	void die ();
	bool dead = false;
	public:
	Player (Vector2);
	bool getDead ();
	void respondToInput (char);
	void beginAttack ();
	void move (char);
	void update ();
	void hurt ();
	int getHealth ();
};

class Zombie : public Collider {
	private:
	char defaultPose [9] = { ' ', 'C', ' ', '/', 'l', '`', '/', '`', '?' };
	int moveTimer;
	int moveInterval = 10;
	PathFinder pathFinder;
	int pathIndex = 0;
	std::vector <Node> path;
	Graphics graphics;
	bool dead = false;
	void collided (Collider *);
	void move ();
	bool spawnSuccessful = true;
	public:
	Zombie (Vector2, Vector2);
	void update ();
	void calculatePath (Vector2);
	void die ();
};

class ZombieManager {
	private:
	int zombieSpawnTimer = 0;
	int zombieSpawnInterval = 50;
	int waves = 0;
	void spawnZombie (Vector2);
	void updateZombies ();
	std::vector<Zombie *> zombies;
	public:
	ZombieManager ();
	int getWaves ();
	void calculateZombiesPaths (Vector2);
	int getZombieAmount ();
	void update (Vector2);
};

class Game {
	private:
	bool gameOver = false;
	int deltaTime;
	bool running;
	void initialize ();
	void update ();
	void draw ();
	char gameOverMessage [10] = "GAME OVER";
	Player player;
	ZombieManager zombieManager;
	Graphics graphics;
	Wall wall1, wall2, wall3, wall4;
	Wall wall5, wall6, wall7, wall8;
	bool firstTime = true;
	void over ();
	std::string input;
	std::string line1 = "You're on your way to work.....";
	std::string line2 = "\nSUDDENLY, ZOMBIES!";
	std::string line3 = "\nWill you fight? (Y/N)";
	std::string line4 = "Fight!";
	std::string line5 = "The zombies killed you.....\n";
	std::string line6 = "You stood there idly and died.....\n";
	std::string line7 = "Play again? (Y/N)";
	public:
	Game (int);
	static int score;
	void play ();
};

void toggleConsoleCursor (bool);

void printTextDelayed (std::string);

Vector2::Vector2 () : x (0), y (0) {}

Vector2::Vector2 (int x, int y) : x (x), y (y) {}

Node::Node () {

}

Node::Node (bool walkable, Vector2 coord) : walkable (walkable), coord (coord) {}

void Node::removeNodeFromVector (std::vector<Node> &vect, Node node) {
	for (int i = 0; i < vect.size (); i++)
		if (vect [i] == node)
			vect.erase (vect.begin () + i);
}

bool Node::nodeInList (std::vector<Node> &list, Node node) {
	for (Node nodeToCheck : list) {
		if (nodeToCheck == node)
			return true;
	}
	return false;
}

int Node::getDistance (Node nodeA, Node nodeB) {
	int distanceX = abs (nodeA.coord.x - nodeB.coord.x);
	int distanceY = abs (nodeA.coord.y - nodeB.coord.y);

	if (distanceX > distanceY)
		return 14 * distanceY + 10 * (distanceX - distanceY);
	return 14 * distanceX + 10 * (distanceY - distanceX);
}

int Node::getFCost () {
	return hCost + gCost;
}

Grid::Grid () {}

void Grid::create () {
	for (int x = 0; x < screenWidth; x++) {
		for (int y = 0; y < screenHeight; y++) {
			//bool walkable = true;
			bool walkable = Wall::walkableAreas [x] [y];
			gridOfNode [x] [y] = Node (walkable, Vector2 (x, y));
		}
	}
}

std::vector<Node> Grid::getNeighbours (Node nodeToFind) {
	std::vector<Node> neighbours;
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			if (x == 0 && y == 0)
				continue;
			int checkForX = nodeToFind.coord.x + x;
			int checkForY = nodeToFind.coord.y + y;
			if (checkForX > 0 && checkForX < screenWidth && checkForY > 0 && checkForY < screenHeight)
				neighbours.push_back (gridOfNode [checkForX] [checkForY]);
		}
	}
	return neighbours;
}

Grid PathFinder::grid;

void PathFinder::retracePath (Node startNode, Node endNode) {
	Node currentNode = endNode;
	while (currentNode != startNode) {
		path.push_back (currentNode);
		currentNode = grid.gridOfNode [currentNode.parentCoord.x] [currentNode.parentCoord.y];
	}
	std::reverse (path.begin (), path.end ());
}

void PathFinder::findPath (Vector2 originPos, Vector2 targetPos) {
	Node startNode = grid.gridOfNode [originPos.x] [originPos.y];
	Node endNode = grid.gridOfNode [targetPos.x] [targetPos.y];

	std::vector<Node> openList;
	std::set<Node> closedList;

	openList.push_back (startNode);

	while (openList.size () > 0) {
		Node currentNode = openList [0];

		for (int i = 1; i < openList.size (); i++) {
			if (currentNode.getFCost () > openList [i].getFCost () || currentNode.getFCost () == openList [i].getFCost ()) {
				if (openList [i].hCost < currentNode.hCost)
					currentNode = openList [i];
			}
		}

		Node::removeNodeFromVector (openList, currentNode);
		closedList.insert (currentNode);

		if (currentNode == endNode) {
			//std::cout << "FOUND" << std::endl;
			retracePath (grid.gridOfNode [startNode.coord.x] [startNode.coord.y], grid.gridOfNode [endNode.coord.x] [endNode.coord.y]);
			return;
		}

		std::vector<Node> neighbours = grid.getNeighbours (currentNode);
		for (int i = 0; i < neighbours.size (); i++) {
			if (!neighbours [i].walkable || closedList.count (neighbours [i]))
				continue;

			int newCostToNeighbour = currentNode.gCost + Node::getDistance (currentNode, neighbours [i]);
			if (newCostToNeighbour < neighbours [i].gCost || !Node::nodeInList (openList, neighbours [i])) {
				neighbours [i].gCost = newCostToNeighbour;
				neighbours [i].hCost = Node::getDistance (neighbours [i], endNode);
				neighbours [i].parentCoord = currentNode.coord;

				grid.gridOfNode [neighbours [i].coord.x] [neighbours [i].coord.y] = neighbours [i];

				if (!Node::nodeInList (openList, neighbours [i]))
					openList.push_back (neighbours [i]);
			}
		}
	}
}

bool Graphics::screenUpdated = false;

char Graphics::screen [screenWidth] [screenHeight];

Graphics::Graphics (Vector2 position, Vector2 dimensions) : position (position), dimensions (dimensions) {}

void Graphics::setPosition (Vector2 position) {
	clear ();
	this->position = position;
}

void Graphics::extrude (Direction direction, int amount) {
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
			break;
	}
}

void Graphics::clear () {
	for (int x = position.x; x < position.x + dimensions.x; x++) {
		for (int y = position.y; y < position.y + dimensions.y; y++) {
			screen [x] [y] = ' ';
		}
	}
	screenUpdated = true;
}

void Graphics::render (char graphics []) {
	int i = 0;
	for (int y = position.y; y < position.y + dimensions.y; y++) {
		for (int x = position.x; x < position.x + dimensions.x; x++) {
			if (screen [x] [y] = ' ') {
				screen [x] [y] = graphics [i];
				i ++;
			}
		}
	}
	screenUpdated = true;
}

void Graphics::render (char graphic) {
	for (int y = position.y; y < position.y + dimensions.y; y++)
		for (int x = position.x; x < position.x + dimensions.x; x++)
			screen [x] [y] = graphic;
	screenUpdated = true;
}

int Collider::currentId = 1;

std::vector<Collider *> Collider::colliders;

int Collider::collisionMatrix [screenWidth] [screenHeight];

Collider::Collider (Vector2 position, Vector2 dimensions) : position (position), dimensions (dimensions) {
	colliders.push_back (this);
	id = currentId;
	currentId ++;
	//updateCollisionMatrix ();
	//checkCollisionMatrix ();
	//setPosition (position);
}

Vector2 Collider::getPosition () {
	return position;
}

void Collider::setPosition (Vector2 position) {
	//clear (); //clear called twice once first time, second time is when it returns to previous pos
	previousPosition = this->position;
	this->position = position;
	checkCollisionMatrix ();
}

void Collider::checkCollisionMatrix () {
	for (int x = position.x; x < position.x + dimensions.x; x++) {
		for (int y = position.y; y < position.y + dimensions.y; y++) {
			int point = collisionMatrix [x] [y];
			if (point != 0 && point != id) {
				collided (colliders [point - 1]);
				//colliders [point - 1]->collided (this);
				return;
			}
		}
	}
	clear (previousPosition);
	updateCollisionMatrix ();
}

void Collider::updateCollisionMatrix () {
	for (int x = position.x; x < position.x + dimensions.x; x++) {
		for (int y = position.y; y < position.y + dimensions.y; y++) {
			collisionMatrix [x] [y] = id;
		}
	}
}

void Collider::clear (Vector2 position) {
	for (int x = position.x; x < position.x + dimensions.x; x++) {
		for (int y = position.y; y < position.y + dimensions.y; y++) {
			if (collisionMatrix [x] [y] == id)
				collisionMatrix [x] [y] = 0;
		}
	}
}

void Collider::clear () {
	for (int x = position.x; x < position.x + dimensions.x; x++) {
		for (int y = position.y; y < position.y + dimensions.y; y++) {
			if (collisionMatrix [x] [y] == id)
				collisionMatrix [x] [y] = 0;
		}
	}
}

Zombie::Zombie (Vector2 position, Vector2 playerPosition) : Collider (Vector2 (position), Vector2 (3, 3)), graphics (position, Vector2 (3, 3)) {
	moveTimer = 0;
	setPosition (position);
	if (spawnSuccessful) {
		graphics.setPosition (position);
		graphics.render (defaultPose);
		calculatePath (playerPosition);
	}
}

void Zombie::die () {
	clear (previousPosition);
	clear (position);
	graphics.clear ();
	dead = true;
	Game::score++;
}

void Zombie::calculatePath (Vector2 targetPosition) {
	if (dead)
		return;
	pathFinder.findPath (position, targetPosition);
	path = pathFinder.path;
	pathIndex = 0;
}

void Zombie::collided (Collider * collider) {
	if (dynamic_cast<Attack *> (collider))
		die ();
	else if (Player * player = dynamic_cast<Player *> (collider)) {
		player->hurt ();
		clear (previousPosition);
		updateCollisionMatrix ();
	} else if (dynamic_cast<Wall *> (collider)) {
		clear ();
		dead = true;
		spawnSuccessful = false;
	}
}

void Zombie::update () {
	if (dead)
		return;
	moveTimer++;
	if (moveTimer > moveInterval) {
		move ();
		moveTimer = 0;
	}
}

void Zombie::move () {
	pathIndex ++;
	if (pathIndex < path.size ()) {
		Vector2 newPosition (path [pathIndex].coord);
		setPosition (newPosition);
		if (dead)
			return;
		graphics.setPosition (newPosition);
		graphics.render (defaultPose);
	}
}

ZombieManager::ZombieManager () {};

int ZombieManager::getWaves () {
	return waves;
}

int ZombieManager::getZombieAmount () {
	return zombies.size ();
}

void ZombieManager::calculateZombiesPaths (Vector2 position) {
	for (int i = 0; i < zombies.size (); i++)
		zombies [i]->calculatePath (position);
}

void ZombieManager::updateZombies () {
	for (int i = 0; i < zombies.size (); i++)
		zombies [i]->update ();
}

void ZombieManager::update (Vector2 playerPosition) {
	zombieSpawnTimer++;
	if (zombieSpawnTimer > zombieSpawnInterval) {
		spawnZombie (playerPosition);
		zombieSpawnTimer = 0;
	}
	updateZombies ();
}

void ZombieManager::spawnZombie (Vector2 playerPosition) {
	bool spawnSuccessful = false;
	do {
		Vector2 spawnPosition (rand () % screenWidth, rand () % screenHeight);
		if (Wall::walkableAreas [spawnPosition.x] [spawnPosition.y] && spawnPosition != playerPosition) {
			spawnSuccessful = true;
			zombies.push_back (new Zombie (spawnPosition, playerPosition));
			if (getZombieAmount () > 10 * waves) {
				waves ++;
				zombieSpawnInterval * 0.8;
			}
		}
	} while (!spawnSuccessful);
}

Wall::Wall (Vector2 position, Vector2 dimensions) : Collider (position, dimensions), graphics (position, dimensions) {
	if (!initialized) {
		initialized = true;
		for (int x = 0; x < screenWidth; x++)
			for (int y = 0; y < screenHeight; y++)
				walkableAreas [x] [y] = true;
	}
	setPosition (position);
	updateWalkableAreas ();
	graphics.render (219);
}

void Wall::collided (Collider * collider) {
	if (dynamic_cast<Wall *> (collider)) {
		updateCollisionMatrix ();
	}
}

bool Wall::walkableAreas [screenWidth] [screenHeight];

void Wall::updateWalkableAreas () {
	for (int x = position.x; x < position.x + dimensions.x; x++) {
		for (int y = position.y; y < position.y + dimensions.y; y++) {
			walkableAreas [x] [y] = false;
		}
	}
}

bool Wall::initialized = false;

Attack::Attack (Vector2 position, Vector2 dimensions) : Collider (position, dimensions) {
	setPosition (position);
}

void Attack::collided (Collider * collider) {
	if (Zombie * zombie = dynamic_cast<Zombie *> (collider)) {
		zombie->die ();
		updateCollisionMatrix ();
	} else if (dynamic_cast<Wall *> (collider))
		blocked = true;
}

bool Attack::getBlocked () {
	return blocked;
}

Player::Player (Vector2 position) : Collider (Vector2 (position), Vector2 (3, 3)), graphics (Vector2 (position.x, position.y), Vector2 (3, 3)) {
	setPosition (position);
}

int Player::getHealth () {
	return health;
}

bool Player::getDead () {
	return dead;
}

void Player::hurt () {
	health--;
	if (health <= 0)
		die ();
}

void Player::collided (Collider * collider) {
	if (dynamic_cast<Zombie *> (collider)) {
		hurt ();
		clear (previousPosition);
		updateCollisionMatrix ();
	} else if (dynamic_cast<Wall *> (collider)) {
		Vector2 pos (previousPosition);
		setPosition (pos);
	}
}

void Player::move (char input) {
	if (dead || attackTimer < 5)
		return;
	switch (input) {
		case 'w':
			setPosition (Vector2 (position.x, position.y - 1));
			previousMovementInput = input;
			break;
		case 'a':
			setPosition (Vector2 (position.x - 2, position.y));
			previousMovementInput = input;
			break;
		case 's':
			setPosition (Vector2 (position.x, position.y + 1));
			previousMovementInput = input;
			break;
		case 'd':
			setPosition (Vector2 (position.x + 2, position.y));
			previousMovementInput = input;
			break;
	}
	graphics.setPosition (position);
	graphics.render (defaultPose);
}

void Player::die () {
	//clear (previousPosition);
	clear ();
	graphics.clear ();
	dead = true;
}

void Player::update () {
	if (dead)
		return;
	attackTimer++;
	if (attackTimer == attackDuration)
		endAttack ();
}

void Player::beginAttack () {
	if (dead || attackTimer < 5)
		return;
	attackTimer = 0;
	switch (previousMovementInput) {
		case 'w':
			attackDirection = up;
			attack = new Attack (Vector2 (position.x, position.y - 2), Vector2 (3, 2));
			if (!attack->getBlocked ()) {
				graphics.extrude (attackDirection, 1);
				graphics.render (attackUpPose);
			}
			break;
		case 'a':
			attackDirection = left;
			attack = new Attack (Vector2 (position.x - 2, position.y), Vector2 (2, 3));
			if (!attack->getBlocked ()) {
				graphics.extrude (attackDirection, 1);
				graphics.render (attackLeftPose);
			}
			break;
		case 's':
			attackDirection = down;
			attack = new Attack (Vector2 (position.x, position.y + 3), Vector2 (3, 2));
			if (!attack->getBlocked ()) {
				graphics.extrude (attackDirection, 1);
				graphics.render (attackDownPose);
			}
			break;
		case 'd':
			attackDirection = right;
			attack = new Attack (Vector2 (position.x + 3, position.y), Vector2 (2, 3));
			if (!attack->getBlocked ()) {
				graphics.extrude (attackDirection, 1);
				graphics.render (attackRightPose);
			}
			break;
	}
}

void Player::endAttack () {
	if (attack != NULL) {
		if (!attack->getBlocked ()) {
			graphics.clear ();
			graphics.extrude (attackDirection, -1);
		}
		attack->clear ();
		delete attack;
	}
	graphics.render (defaultPose);
}

void Player::respondToInput (char input) {
	if (dead || attackTimer < 5)
		return;
	if (input == ' ')
		beginAttack ();
	else
		move (input);
}

int Game::score = 0;

Game::Game (int refreshRate) :
	deltaTime (1000 / refreshRate), player (Vector2 (screenWidth / 2, screenHeight / 2)),
	wall1 (Vector2 (0, 0), Vector2 (screenWidth, 2)),
	wall2 (Vector2 (0, screenHeight - 2), Vector2 (screenWidth, 2)),
	wall3 (Vector2 (0, 2), Vector2 (3, screenHeight - 4)),
	wall4 (Vector2 (screenWidth - 3, 2), Vector2 (3, screenHeight - 4)),
	wall5 (Vector2 (screenWidth * 0.25, screenHeight * 0.3), Vector2 (2, screenHeight / 2)),
	wall6 (Vector2 (screenWidth * 0.75, screenHeight * 0.3), Vector2 (2, screenHeight / 2)),
	wall7 (Vector2 (screenWidth * 0.25, screenHeight * 0.3), Vector2 (screenWidth / 4, 1)),
	wall8 (Vector2 (screenWidth / 2, screenHeight * 4 / 5 - 2), Vector2 (screenWidth / 4, 1)),
	graphics (Vector2 (screenWidth / 2, screenHeight / 2), Vector2 (9, 1)) {};

void Game::over () {
	graphics.render (gameOverMessage);
	gameOver = true;
}

void Game::play () {
	printTextDelayed (line1);
	system ("pause>nul");
	printTextDelayed (line2);
	system ("pause>nul");
	printTextDelayed (line3);
	std::cin >> input;
	if (input == "y" || input == "Y" || input == "yes" || input == "Yes") {
		printTextDelayed (line4);
		running = true;
		initialize ();
		while (running) {
			update ();
			Sleep (deltaTime);
		}
	} else if (input == "n" || input == "N" || input == "no" || input == "No") {
		printTextDelayed (line5);
		printTextDelayed (line7);
		std::cin >> input;
		if (input == "y" || input == "Y" || input == "yes" || input == "Yes") {
			play ();
		} else
			return;
	} else {
		printTextDelayed (line6);
		printTextDelayed (line7);
		std::cin >> input;
		if (input == "y" || input == "Y" || input == "yes" || input == "Yes") {
			play ();
		} else
			return;
	}
}

void Game::initialize () {
	toggleConsoleCursor (false);
	srand (time (0));
	PathFinder::grid.create ();
}

void Game::update () {
	zombieManager.update (player.getPosition ());
	player.update ();
	if (kbhit ()) {
		char input = getch ();
		if (input == ' ')
			player.beginAttack ();
		else {
			player.move (input);
			zombieManager.calculateZombiesPaths (player.getPosition ());
		}
		if (gameOver && input == 'r') {
			running = false;

			return;
		}
	}
	if (player.getDead ())
		over ();
	if (Graphics::screenUpdated) {
		Graphics::screenUpdated = false;
		draw ();
	}
}

void Game::draw () {
	system ("cls");
	for (int height = 0; height < screenHeight; height++) {
		for (int width = 0; width < screenWidth; width++) {
			//std::cout << Collider::collisionMatrix [width] [height];
			//std::cout << Wall::walkableAreas [width] [height];
			std::cout << Graphics::screen [width] [height];
		}
		std::cout << std::endl;
	}
	std::cout << "Score : " << score
		<< "    Health : " << player.getHealth ()
		<< "    Zombies : " << zombieManager.getZombieAmount ()
		<< "    Wave : " << zombieManager.getWaves ()
		<< "    [W, A, S, D] to move, [Spacebar] to attack." << std::endl;
	if (gameOver) {
		std::cout << "tHe zOmBiEs AtE uR bRaiNs" << std::endl;
	}
}

int main () {
	Game game (10);
	game.play ();
}

void toggleConsoleCursor (bool isVisible) {
	HANDLE out = GetStdHandle (STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo (out, &cursorInfo);
	cursorInfo.bVisible = isVisible; // set the cursor visibility
	SetConsoleCursorInfo (out, &cursorInfo);
}

void printTextDelayed (std::string text) {
	for (int i = 0; i < text.length (); i++) {
		std::cout << text [i];
		Sleep (100);
	}
}