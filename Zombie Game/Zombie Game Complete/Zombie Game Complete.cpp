/**
**	Description: A Zombie survival game to entertain bored people. The game based it's mechanic around multiple 2D arrays, along with Collision Detection, Fighting, Scoring System and A* PathFinding
**				Zombies will spawn in waves, with each waves increasing in difficulty (by lessening spawning interval).
**				The player can attack in 4 directions, and the score is how many zombies killed by the player.
**				Health will decrease each time the player is touched by a zombie
**				Try to survive and kill as much zombies as you can, and beat your friend's highscore!
**
**	Author: Ng Bob Shoaun 0331088
**			Tan Jia Hui 0330960
*			Ngeh Chee Sen 0331576
**	 Last date of modification: 21 / 07 / 2017
**/

#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <vector>
#include <set>
#include <cmath>
#include <string>

const int screenWidth = 100, screenHeight = 25; // the amount of rows and columns in the 2D array. All 2D arrays(Pathfinding's Grid, Collision, Graphic) will have their number of elements based in these const values

enum Direction { up, down, left, right };

/**
** A User defined struct type to contain the X,Y value of a certain object.
** A struct was used because Vector2 is only going to contain pure data members(POD, plain old data) like integers and such
** And also because variables declared is default as public, exactly what we need.
**/
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
	virtual void collided (Collider*) = 0;
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

/**
** The node class is the mose important part of the A* pathfinding algorithm, a pathFinding algorithm to find the shortest path from a starting point to an ending point
**	Each coord(In this case, represented by Vector2) will be represented by a Node
**/
class Node {
	private:
	public:
	Vector2 parentPosition; //used to trace back to it's parent node, important when retracing the path after the shortest path has been found
	Vector2 position; //it's coord in the array
					  /**
					  ** The G cost is the movement cost from itself to the starting node, used to determined whether path going through this node is the shortest
					  ** The H cost is the movement cost from itself to the end node, ignoring obstacles, to determine whether the path is going towards the endNode, not the other way round
					  **/
	int gCost, hCost;
	bool walkable; //if this Node is a wall, then ignore this Node when calculating the shortest path
	int randomValue = rand (); //used to in the bool > operator for the std::set container
	Node ();
	Node (bool, Vector2);
	static int getDistance (Node, Node);
	static bool nodeInList (std::vector<Node> &, Node);
	static void removeNodeFromVector (std::vector<Node> &, Node);
	int getFCost (); //FCost is the sum of G and H cost

					 //following operator overloadings are required to compare custom class types
	bool operator == (const Node &node) {
		return position == node.position;
	}
	bool operator != (const Node &node) {
		return !(*this == node);
	}
	bool operator < (const  Node &node) const {
		return randomValue > node.randomValue;
	}
};

// A grid is a class type that contains a grid of Node
class Grid {
	public:
	Node gridOfNode [screenWidth] [screenHeight]; //declaring the a 2d array of Node
	Grid ();
	std::vector<Node> getNeighbours (Node);
	void create ();
};

//The brain of the A* path finding
class PathFinder {
	private:
	public:
	static Grid grid; //since there is only 1 grid at a time, it would be declared as a static
	std::vector<Node> path; // a path of nodes that contain the shortest path from the targetNode to endNode
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
	Wall wall1, wall2, wall3, wall4, wall5, wall6, wall7, wall8;
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

Node::Node () {}

Node::Node (bool walkable, Vector2 position) : walkable (walkable), position (position) {}

/**
** Remove node in a Vector of type Node, using an for-loop
**/
void Node::removeNodeFromVector (std::vector<Node> & vector, Node node) {
	for (int i = 0; i < vector.size (); i++)
		if (vector [i] == node)
			vector.erase (vector.begin () + i);
}

/**
** A bool function that returns true if Node is in a vector, and false otherwise
**/
bool Node::nodeInList (std::vector<Node> & list, Node node) {
	for (Node nodeToCheck : list)
		if (nodeToCheck == node)
			return true;
	return false;
}

/**
** A function with a formula to get the movementCost between one Node to the other
** 14 will be the 1 diagonal move
** and 10 will the movement cost when moving horzontally and vertically
** Reason: if moving up and down is 1, using pythagoras, the diagonal value will be 1.41
** for eficiency reason, multiplying these values by 10 to get a whole number will reduce workload for the computer
**/
int Node::getDistance (Node nodeA, Node nodeB) {
	int distanceX = abs (nodeA.position.x - nodeB.position.x);
	int distanceY = abs (nodeA.position.y - nodeB.position.y);
	if (distanceX > distanceY)
		return 14 * distanceY + 10 * (distanceX - distanceY);
	return 14 * distanceX + 10 * (distanceY - distanceX);
}

int Node::getFCost () {
	return hCost + gCost;
}

Grid::Grid () {}

//initializing the gridOfNode with Nodes
void Grid::create () {
	for (int x = 0; x < screenWidth; x++) {
		for (int y = 0; y < screenHeight; y++) {
			bool walkable = Wall::walkableAreas [x] [y];
			gridOfNode [x] [y] = Node (walkable, Vector2 (x, y));
		}
	}
}

/**
** A function that returns a vector of Nodes
** Used to get the neighbour nodes of a given node(passed in as argument)
**/
std::vector<Node> Grid::getNeighbours (Node nodeToFind) {
	std::vector<Node> neighbours;
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			if (x == 0 && y == 0)
				continue;
			int checkForX = nodeToFind.position.x + x;
			int checkForY = nodeToFind.position.y + y;
			if (checkForX > 0 && checkForX < screenWidth && checkForY > 0 && checkForY < screenHeight) // check if node is within the Grid(array of Nodes)
				neighbours.push_back (gridOfNode [checkForX] [checkForY]); // if so, add it into the neighbours vector
		}
	}
	return neighbours; // return neighbours if any
}

Grid PathFinder::grid; //initialzing the static grid

					   // function to retrace path, so other classes can use the information
void PathFinder::retracePath (Node startNode, Node endNode) {
	Node currentNode = endNode;
	while (currentNode != startNode) {
		path.push_back (currentNode);
		currentNode = grid.gridOfNode [currentNode.parentPosition.x] [currentNode.parentPosition.y];
	}
	std::reverse (path.begin (), path.end ()); //reverse the path 
}

// Most important function, to find the shortest path from an currentPos to TargetPos, if there's one, call retracePath function to start retracing path
void PathFinder::findPath (Vector2 currentPosition, Vector2 targetPosition) {
	Node startNode = grid.gridOfNode [currentPosition.x] [currentPosition.y]; //find the node on the originPos
	Node endNode = grid.gridOfNode [targetPosition.x] [targetPosition.y]; // find the node on targetPos
	std::vector<Node> openList; // openList is a vector that contains Node that should be evaluated(to check if the path through it will the be shortest)
								// closedList is a set because the ordering of the closed list does not matter match, as we only want to check whether a Node is contained in this set. 
								// The set.count function has a low complexity(log(n)), therefore it's preferred
	std::set<Node> closedList;
	openList.push_back (startNode); //first add currentnode to openList, as it's the first node to be evaluated
									//keep looping until there is no more Node in openList, which means all nodes has been evaluated
	while (openList.size () > 0) {
		Node currentNode = openList [0];
		for (int i = 1; i < openList.size (); i++)
			if (currentNode.getFCost () > openList [i].getFCost () || currentNode.getFCost () == openList [i].getFCost ()) // make the Node in openList with the lowest fCost to be the currentNode, if they are the same, determine them by comparing their hCost(lesser hCost means closer to targetNode)
				if (openList [i].hCost < currentNode.hCost)
					currentNode = openList [i];
		Node::removeNodeFromVector (openList, currentNode); //remove current node from openList
		closedList.insert (currentNode); //add currentNode to closedList
										 //if currentNode is the same as endNode, it means that the path has been found, and it's time to retrace it
		if (currentNode == endNode) {
			retracePath (grid.gridOfNode [startNode.position.x] [startNode.position.y], grid.gridOfNode [endNode.position.x] [endNode.position.y]); //retrace pathFunction
			return;
		}
		std::vector<Node> neighbours = grid.getNeighbours (currentNode); //get neighbour of currentNode and loop through the vector of Nodes
		for (int i = 0; i < neighbours.size (); i++) {
			if (!neighbours [i].walkable || closedList.count (neighbours [i])) //if node is not walkable or already being evaluated, skip it
				continue;
			//it's movement cost from itself to startNode
			int newCostToNeighbour = currentNode.gCost + Node::getDistance (currentNode, neighbours [i]);
			if (newCostToNeighbour < neighbours [i].gCost || !Node::nodeInList (openList, neighbours [i])) { //check if the path through neighbour[i] is the shortest, and whether it's in the openList
				neighbours [i].gCost = newCostToNeighbour;
				neighbours [i].hCost = Node::getDistance (neighbours [i], endNode);
				neighbours [i].parentPosition = currentNode.position; //set currentNode as the neighbour's parent
				grid.gridOfNode [neighbours [i].position.x] [neighbours [i].position.y] = neighbours [i]; //applying changes the Nodes in the grid
				if (!Node::nodeInList (openList, neighbours [i]))
					openList.push_back (neighbours [i]); // if Node is not in openList, add it in so it can evaluated
			}
		}
	}
}

bool Graphics::screenUpdated = false;

char Graphics::screen [screenWidth] [screenHeight];

Graphics::Graphics (Vector2 position, Vector2 dimensions) : position (position), dimensions (dimensions) {}

// set position for the graphics
void Graphics::setPosition (Vector2 position) {
	clear ();
	this->position = position;
}

// extrude the graphics by the amount
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

// clear the graphics
void Graphics::clear () {
	for (int x = position.x; x < position.x + dimensions.x; x++)
		for (int y = position.y; y < position.y + dimensions.y; y++)
			screen [x] [y] = ' ';
	screenUpdated = true;
}

// load the graphics to be rendered and adds it to the screen array, then sets screenUpdated flag to true.
void Graphics::render (char graphics []) {
	int i = 0;
	for (int y = position.y; y < position.y + dimensions.y; y++) {
		for (int x = position.x; x < position.x + dimensions.x; x++) {
			if (screen [x] [y] = ' ') {
				screen [x] [y] = graphics [i];
				i++;
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

// the static id to keep track of the next id for the collider.
int Collider::currentId = 1;

// static array of colliders, stores all the colliders that is created
std::vector<Collider *> Collider::colliders;

// stores collision information in this 2d array
int Collider::collisionMatrix [screenWidth] [screenHeight];

// constructor for Collider.
Collider::Collider (Vector2 position, Vector2 dimensions) : position (position), dimensions (dimensions) {
	colliders.push_back (this);
	id = currentId;
	currentId++;
}

// getter for the colliders position
Vector2 Collider::getPosition () {
	return position;
}

// sets the position
void Collider::setPosition (Vector2 position) {
	previousPosition = this->position;
	this->position = position;
	checkCollisionMatrix ();
}

// checks for collisions, then calls the collided fucntion is collided with something
void Collider::checkCollisionMatrix () {
	for (int x = position.x; x < position.x + dimensions.x; x++) {
		for (int y = position.y; y < position.y + dimensions.y; y++) {
			int point = collisionMatrix [x] [y];
			if (point != 0 && point != id) {
				collided (colliders [point - 1]);
				return;
			}
		}
	}
	clear (previousPosition);
	updateCollisionMatrix ();
}

// updates the collision matrix
void Collider::updateCollisionMatrix () {
	for (int x = position.x; x < position.x + dimensions.x; x++)
		for (int y = position.y; y < position.y + dimensions.y; y++)
			collisionMatrix [x] [y] = id;
}

// clears the collider
void Collider::clear (Vector2 position) {
	for (int x = position.x; x < position.x + dimensions.x; x++)
		for (int y = position.y; y < position.y + dimensions.y; y++)
			if (collisionMatrix [x] [y] == id)
				collisionMatrix [x] [y] = 0;
}

/*clears the collider*/
void Collider::clear () {
	for (int x = position.x; x < position.x + dimensions.x; x++)
		for (int y = position.y; y < position.y + dimensions.y; y++)
			if (collisionMatrix [x] [y] == id)
				collisionMatrix [x] [y] = 0;
}

/* constructor, creates a zombie and spawns it.*/
Zombie::Zombie (Vector2 position, Vector2 playerPosition) : Collider (Vector2 (position), Vector2 (3, 3)), graphics (position, Vector2 (3, 3)) {
	moveTimer = 0;
	setPosition (position);
	if (spawnSuccessful) {
		graphics.setPosition (position);
		graphics.render (defaultPose);
		calculatePath (playerPosition);
	}
}

/* called when the zombie dies, it clears the colliders and graphics of the zombie,
and then sets a flag for dead to true*/
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

// 
void Zombie::update () {
	if (dead)
		return;
	moveTimer++;
	if (moveTimer > moveInterval) {
		move ();
		moveTimer = 0;
	}
}

// moves the zombies, makes the pathIndex to increase so the next position is used in the path found. The position is then set and the graphics rendered
void Zombie::move () {
	pathIndex++;
	if (pathIndex < path.size ()) {
		Vector2 newPosition (path [pathIndex].position);
		setPosition (newPosition);
		if (dead)
			return;
		graphics.setPosition (newPosition);
		graphics.render (defaultPose);
	}
}

// constructor
ZombieManager::ZombieManager () {};

// getter for waves
int ZombieManager::getWaves () {
	return waves;
}

//getter for zombies vector size
int ZombieManager::getZombieAmount () {
	return zombies.size ();
}

// calculates the zombies path for all zombies
void ZombieManager::calculateZombiesPaths (Vector2 position) {
	for (int i = 0; i < zombies.size (); i++)
		zombies [i]->calculatePath (position);
}

// function that updates all the zombies in the zombies vector
void ZombieManager::updateZombies () {
	for (int i = 0; i < zombies.size (); i++)
		zombies [i]->update ();
}

// called every update, it checks if zombietimer has exceeded the spawn interval, is so then spawn a zombie. it also increases the zombieSpawnTimer
void ZombieManager::update (Vector2 playerPosition) {
	zombieSpawnTimer++;
	if (zombieSpawnTimer > zombieSpawnInterval) {
		spawnZombie (playerPosition);
		zombieSpawnTimer = 0;
	}
	updateZombies ();
}

/*used to spawn a zombie, if the spawn was successful,
meaning it doesnt collide spawn within non walkable areas
or on the player then the zombie is created and added to the vector of zombies,
the zombie amount is also checked to increase the wave and decrease spawn interval if neccesssary */
void ZombieManager::spawnZombie (Vector2 playerPosition) {
	bool spawnSuccessful = false;
	do {
		Vector2 spawnPosition (rand () % screenWidth, rand () % screenHeight);
		if (Wall::walkableAreas [spawnPosition.x] [spawnPosition.y] && spawnPosition != playerPosition) {
			spawnSuccessful = true;
			zombies.push_back (new Zombie (spawnPosition, playerPosition));
			if (getZombieAmount () > 10 * waves) {
				waves++;
				zombieSpawnInterval *= 0.8;
			}
		}
	} while (!spawnSuccessful);
}

/*Wall constructor, the position and dimensions of the wall is passed into the constructor,
the for loops that initializes the static array walkable areas are only called once when the first wall object is created
the initial collision detection is done, then the walkableAreas array updated to account for the new wall, after that the graphics are rendered*/
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

/* function override of Collider base class's pure virtual collided function,
called when the Wall collides with another collider, the collider that it collides with is passed into the function
if the collider is another wall, the wall will overlap the collided wall by updating the collision matrix */
void Wall::collided (Collider * collider) {
	if (dynamic_cast<Wall *> (collider))
		updateCollisionMatrix ();
}

/*a 2d bool array that stores data to all the transversible and non transversible positions,
it is used by the pathfinding system*/
bool Wall::walkableAreas [screenWidth] [screenHeight];

/*updates the walkable areas bool array*/
void Wall::updateWalkableAreas () {
	for (int x = position.x; x < position.x + dimensions.x; x++)
		for (int y = position.y; y < position.y + dimensions.y; y++)
			walkableAreas [x] [y] = false;
}

/* static flag for checking if a wall object has been created,
this is so that the walkable areas array will only be initialized once, and not everytime a new wall object is created*/
bool Wall::initialized = false;

/* attack constructor, position and dimensions passed in, base class constructor called,
then the initial collision detection done by calling setPosition method*/
Attack::Attack (Vector2 position, Vector2 dimensions) : Collider (position, dimensions) {
	setPosition (position);
}

/* function override of Collider base class's pure virtual collided function,
called when the attack collides with another collider, if the collider is a zombie, then the zombie dies,
and the collisions updated, if it collides with a wall, blocked is set to true, blocked is a flag for if the
attack is allowed, so if it collides with a wall the attack is not allowed */
void Attack::collided (Collider * collider) {
	if (Zombie * zombie = dynamic_cast<Zombie *> (collider)) {
		zombie->die ();
		updateCollisionMatrix ();
	} else if (dynamic_cast<Wall *> (collider))
		blocked = true;
}

// getter for blocked variable
bool Attack::getBlocked () {
	return blocked;
}

/* constructor for player, the position is passed into the constructor and used to set the initial position
of the graphics and collider hitbox, the dimensions of the player (3 by 3) is also passed into the constructors of graphics and collider
finally, the setPosition method is called to do the first collision check when it is created.*/
Player::Player (Vector2 position) : Collider (Vector2 (position), Vector2 (3, 3)), graphics (position, Vector2 (3, 3)) {
	setPosition (position);
}

// getter for the health variable in player
int Player::getHealth () {
	return health;
}

// getter for the dead variable in player
bool Player::getDead () {
	return dead;
}

// hurts the player by decreasing its health, if the health is zero or below, the player dies
void Player::hurt () {
	health--;
	if (health <= 0)
		die ();
}

/* function override of Collider base class's pure virtual collided fucntion,
called when the player collides with another collider, the collider that it collides with is passed into the function
if the collider is a zombie, the player gets hurt and updates its colliders, else if the collider is a wall then set
position to the previous position, so the player wont go inside the wall. */
void Player::collided (Collider * collider) {
	if (dynamic_cast<Zombie *> (collider)) {
		hurt ();
		clear (previousPosition);
		updateCollisionMatrix ();
	} else if (dynamic_cast<Wall *> (collider))
		setPosition (previousPosition);
}

/* moves the player according to the input passed, and then stores the previousInput to be used by the attack system
after the movement the graphics of the player is updated. No movement will occur if the player is dead or if its attacking */
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
	clear ();
	graphics.clear ();
	dead = true;
}

// called every update, increases attack timer, checks if attacktimer is equals to attack duration, if so then end attack
void Player::update () {
	if (dead)
		return;
	attackTimer++;
	if (attackTimer == attackDuration)
		endAttack ();
}

// called when player attack, this initiates an attack, the graphics has to be extruded for the attack
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

// called when the attack is finished, it destroys the attack instance, and clears the attack graphics then show defualt pose
void Player::endAttack () {
	if (attack) {
		if (!attack->getBlocked ()) {
			graphics.clear ();
			graphics.extrude (attackDirection, -1);
		}
		attack->clear ();
		delete attack;
	}
	graphics.render (defaultPose);
}

// initialize static variable score of Game class to 0
int Game::score = 0;

/* constructor for Game class, the refresh rate is passed into the constructor,
and then 1000 is divided by the refresh rate to get the delta time (time between each update)
the player, and 8 walls for the playing field is created along with the graphics for rendering the Game Over mesage. */
Game::Game (int refreshRate) :
	deltaTime (1000 / refreshRate),
	player (Vector2 (screenWidth / 2, screenHeight / 2)),
	wall1 (Vector2 (0, 0), Vector2 (screenWidth, 2)),
	wall2 (Vector2 (0, screenHeight - 2), Vector2 (screenWidth, 2)),
	wall3 (Vector2 (0, 2), Vector2 (3, screenHeight - 4)),
	wall4 (Vector2 (screenWidth - 3, 2), Vector2 (3, screenHeight - 4)),
	wall5 (Vector2 (screenWidth * 0.25, screenHeight * 0.3), Vector2 (2, screenHeight / 2)),
	wall6 (Vector2 (screenWidth * 0.75, screenHeight * 0.3), Vector2 (2, screenHeight / 2)),
	wall7 (Vector2 (screenWidth * 0.25, screenHeight * 0.3), Vector2 (screenWidth / 4, 1)),
	wall8 (Vector2 (screenWidth / 2, screenHeight * 4 / 5 - 2), Vector2 (screenWidth / 4, 1)),
	graphics (Vector2 (screenWidth / 2, screenHeight / 2), Vector2 (9, 1)) {}

// called when the game is over (when the player dies), it renders the game Over message in the middle of the screen and set gameOver to true
void Game::over () {
	graphics.render (gameOverMessage);
	gameOver = true;
}

// starts the game
void Game::play () {
	printTextDelayed (line1);
	system ("pause>nul"); // check any input from keyboard
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
		if (input == "y" || input == "Y" || input == "yes" || input == "Yes")
			play ();
		else
			return;
	} else {
		printTextDelayed (line6);
		printTextDelayed (line7);
		std::cin >> input;
		if (input == "y" || input == "Y" || input == "yes" || input == "Yes")
			play ();
		else
			return;
	}
}

/* initializes the game, removes the blinking cursor, set random number generator to generate new random number based on the time,
creates the grid for pathfinding*/
void Game::initialize () {
	toggleConsoleCursor (false);
	srand (time (0));
	PathFinder::grid.create ();
}

/*the update mathod, called buy the game loop every deltaTime, updates the zombieManager, player, checks for keyboard input and processes them accordingly,
check if player is dead, and check if the screen was updated, to which the draw method will be called to reprint the updated screen array*/
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

/* this is where all the cout happens in the game loop, before drawing anything, the screen is cleared.
then every character in the screen array is printed, followed by the score, health, zombie amount, wave and instructions display.
Also checks if the game is over, which if is true, print out the death message and prompt to terminate game */
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
		<< "\tHealth : " << player.getHealth ()
		<< "\tZombies : " << zombieManager.getZombieAmount ()
		<< "\tWave : " << zombieManager.getWaves ()
		<< "\t[W, A, S, D] to move, [Spacebar] to attack." << std::endl;
	if (gameOver)
		std::cout << "tHe zOmBiEs AtE uR bRaiNs, press 'r' to terminate game" << std::endl;
}

// main method, creates a Game object and calls the play method
int main () {
	Game game (10);
	game.play ();
}

/* toggles the underscore in the console to be visible or invisible depending on the boolean passed in
function is not original and copy pasted from the internet, as it is not part of the logic of the program */
void toggleConsoleCursor (bool isVisible) {
	HANDLE out = GetStdHandle (STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo (out, &cursorInfo);
	cursorInfo.bVisible = isVisible; // set the cursor visibility
	SetConsoleCursorInfo (out, &cursorInfo);
}

// a function that recreates the typing effet by iterating through a string and print each character of the string with a slight delay in between
void printTextDelayed (std::string text) {
	for (int i = 0; i < text.length (); i++) {
		std::cout << text [i];
		Sleep (100);
	}
}