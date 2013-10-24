#pragma once
#include <stdlib.h>

#define GLEW_STATIC
#include "../glew/glew.h"
#include "../ShaderManager.h"
#include <vector>

#define SQUARE_SIDE_LENGTH 0.25
#define CIRCLE_RESOLUTION 34
#define CIRCLE_TO_SQUARE_PROPORTION 0.9
#define PI 3.14159
const float CIRCLE_RADIUS = (SQUARE_SIDE_LENGTH/2)*CIRCLE_TO_SQUARE_PROPORTION;

class Board
{
private:
	static unsigned int nextObjectID;
	unsigned int pickedUpPieceID;
	unsigned int board[8][4];
	std::vector<GLfloat> squareCheckerMesh;
	GLuint shaderProg;
	GLuint positionBuffer;
	GLuint positionSlot;
	GLuint colorSlot;
	GLuint transformSlot;

public:

	Board(GLuint);
	Board();
	~Board(void);
	
	void display(bool=false);

	bool piecePickUp(unsigned int);

	bool piecePlace(unsigned int);

	bool cancelPickup();

	void newGame(bool=false);
};

