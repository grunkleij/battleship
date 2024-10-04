#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 5
#define SHIP_COUNT 3

// Ship node structure to represent a single part of a ship
struct ShipNode {
    int x, y;
    struct ShipNode *next;
};

// Ship structure to represent the entire ship
struct Ship {
    struct ShipNode *parts;
    struct Ship *next;
};

// Function to create a new ship node
struct ShipNode *createShipNode(int x, int y) {
    struct ShipNode *newNode = (struct ShipNode *)malloc(sizeof(struct ShipNode));
    newNode->x = x;
    newNode->y = y;
    newNode->next = NULL;
    return newNode;
}

// Function to add a part to a ship
void addShipPart(struct Ship **ship, int x, int y) {
    struct ShipNode *newNode = createShipNode(x, y);
    newNode->next = (*ship)->parts;
    (*ship)->parts = newNode;
}

// Function to create a new ship
struct Ship *createShip() {
    struct Ship *newShip = (struct Ship *)malloc(sizeof(struct Ship));
    newShip->parts = NULL;
    newShip->next = NULL;
    return newShip;
}

// Function to add a new ship to the fleet
void addShipToFleet(struct Ship **fleet, struct Ship *newShip) {
    newShip->next = *fleet;
    *fleet = newShip;
}

// Initialize grid
void initializeGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = '~';  // '~' represents water
        }
    }
}

// Display grid
void displayFullGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    printf("  ");
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%d ", i);
    }
    printf("\n");

    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%d ", i);
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

// Function to display opponent grid with hidden ships
void displayOpponentGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    printf("  ");
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%d ", i);
    }
    printf("\n");

    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%d ", i);
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 'S') {
                printf("~ ");
            } else {
                printf("%c ", grid[i][j]);
            }
        }
        printf("\n");
    }
}

// Function to place a ship
void placeShip(char grid[GRID_SIZE][GRID_SIZE], struct Ship **fleet, int shipSize, int player, int isAI) {
    int x, y, orientation;
    struct Ship *newShip = createShip();

    while (1) {
        if (!isAI) {
            // Player's turn to place ships
            printf("Player %d, place ship of size %d.\n", player, shipSize);
            printf("Enter starting coordinates (row and column): ");
            scanf("%d%d", &x, &y);
            printf("Enter orientation (1=horizontal, 2=vertical, 3=diagonal): ");
            scanf("%d", &orientation);
        } else {
            // AI's turn to place ships
            orientation = rand() % 3 + 1; // Random orientation (1=horizontal, 2=vertical, 3=diagonal)

            do {
                // Generate random valid starting coordinates
                if (orientation == 1) {
                    // Horizontal placement (ensure room for the entire ship horizontally)
                    x = rand() % GRID_SIZE;
                    y = rand() % (GRID_SIZE - shipSize + 1);
                } else if (orientation == 2) {
                    // Vertical placement (ensure room for the entire ship vertically)
                    x = rand() % (GRID_SIZE - shipSize + 1);
                    y = rand() % GRID_SIZE;
                } else {
                    // Diagonal placement (ensure room for the entire ship diagonally)
                    x = rand() % (GRID_SIZE - shipSize + 1);
                    y = rand() % (GRID_SIZE - shipSize + 1);
                }
            } while (0);  // Continue generating random coordinates until a valid position is found
        }

        int valid = 1;  // Assume placement is valid initially
        for (int i = 0; i < shipSize; i++) {
            int nx = x, ny = y;
            if (orientation == 1) ny += i;           // Horizontal placement
            else if (orientation == 2) nx += i;      // Vertical placement
            else if (orientation == 3) { nx += i; ny += i; }  // Diagonal placement

            // Check if the ship part is out of bounds or overlaps with another ship
            if (nx >= GRID_SIZE || ny >= GRID_SIZE || grid[nx][ny] == 'S') {
                valid = 0;
                if (!isAI) printf("Invalid placement! Try again.\n");
                break;
            }
        }

        // If valid, place the ship
        if (valid) {
            for (int i = 0; i < shipSize; i++) {
                int nx = x, ny = y;
                if (orientation == 1) ny += i;       // Horizontal placement
                else if (orientation == 2) nx += i;  // Vertical placement
                else if (orientation == 3) { nx += i; ny += i; }  // Diagonal placement

                addShipPart(&newShip, nx, ny);  // Add the ship part to the linked list
                grid[nx][ny] = 'S';  // Mark the grid with the ship part
            }
            addShipToFleet(fleet, newShip);  // Add the new ship to the fleet
            break;  // Ship placed successfully, exit the loop
        }

        // If AI, retry placement until it's valid
        if (isAI && !valid) {
            continue;
        }
    }
}


// Function to make a guess
int makeGuess(char opponentGrid[GRID_SIZE][GRID_SIZE], int player) {
    int x, y;
    printf("Player %d, enter coordinates to attack: ", player);
    scanf("%d%d", &x, &y);

    if (opponentGrid[x][y] == 'S') {
        printf("Hit!\n");
        opponentGrid[x][y] = 'X';  // 'X' represents a hit
        return 1;
    } else if (opponentGrid[x][y] == '~') {
        printf("Miss.\n");
        opponentGrid[x][y] = 'O';  // 'O' represents a miss
        return 0;
    } else {
        printf("Already attacked here. Try again.\n");
        return 0;
    }
}

// Check if all ships are sunk
int checkWin(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 'S') return 0;  // Ship still present
        }
    }
    return 1;  // All ships sunk
}

// Function to handle AI attack
void aiAttack(char grid[GRID_SIZE][GRID_SIZE]) {
    int x = rand() % GRID_SIZE;
    int y = rand() % GRID_SIZE;
    printf("Computer attacks at (%d, %d):\n", x, y);

    if (grid[x][y] == 'S') {
        printf("Hit!\n");
        grid[x][y] = 'X';
    } else if (grid[x][y] == '~') {
        printf("Miss.\n");
        grid[x][y] = 'O';
    }
}

int main() {
    srand(time(0));

    struct Ship *player1Fleet = NULL;
    struct Ship *player2Fleet = NULL;

    char player1Grid[GRID_SIZE][GRID_SIZE], player2Grid[GRID_SIZE][GRID_SIZE];
    initializeGrid(player1Grid);
    initializeGrid(player2Grid);

    int gameMode;
    printf("Choose game mode:\n1. Player vs Player\n2. Player vs Computer\n");
    scanf("%d", &gameMode);

    // Player 1 places ships
    for (int i = 1; i <= SHIP_COUNT; i++) {
        placeShip(player1Grid, &player1Fleet, i, 1, 0);
        displayFullGrid(player1Grid);
    }

    // Player 2 or AI places ships
    if (gameMode == 1) {
        for (int i = 1; i <= SHIP_COUNT; i++) {
            placeShip(player2Grid, &player2Fleet, i, 2, 0);
            displayFullGrid(player2Grid);
        }
    } else {
        for (int i = 1; i <= SHIP_COUNT; i++) {
            placeShip(player2Grid, &player2Fleet, i, 2, 1);  // AI places ships
        }
    }

    int currentPlayer = 1;
    while (1) {
        printf("\nPlayer %d's turn.\n", currentPlayer);

        if (currentPlayer == 1) {
            displayOpponentGrid(player2Grid);
            makeGuess(player2Grid, 1);
            if (checkWin(player2Grid)) {
                printf("Player 1 wins!\n");
                break;
            }
            currentPlayer = 2;
        } else {
            if (gameMode == 1) {
                displayOpponentGrid(player1Grid);
                makeGuess(player1Grid, 2);
            } else {
                aiAttack(player1Grid);
            }
            if (checkWin(player1Grid)) {
                printf("Player 2 wins!\n");
                break;
            }
            currentPlayer = 1;
        }
    }

    return 0;
}
