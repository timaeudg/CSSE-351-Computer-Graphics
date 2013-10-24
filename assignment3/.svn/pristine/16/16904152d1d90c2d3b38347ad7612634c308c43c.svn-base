#include "Board.h"

unsigned int Board::nextObjectID = 1;

Board::Board() {

}

Board::Board(GLuint shaderProg)
{
	/*
		Initialize the board, using separate ids for each place on the board according to what is there
		Kings will be negative ids
	*/
	this->newGame();
	this->shaderProg = shaderProg;
	
	// Find out where the shader expects the data
	positionSlot = glGetAttribLocation(shaderProg, "pos");
	colorSlot = glGetUniformLocation(shaderProg, "color");
	transformSlot = glGetUniformLocation(shaderProg, "transform");

	//Generate mesh
	//Board square
	squareCheckerMesh.push_back(-1); squareCheckerMesh.push_back(-1);
	squareCheckerMesh.push_back(-1); squareCheckerMesh.push_back(-0.75);
	squareCheckerMesh.push_back(-0.75); squareCheckerMesh.push_back(-0.75);
	squareCheckerMesh.push_back(-0.75); squareCheckerMesh.push_back(-1);

	//Checker Circle
	int outerPoints = CIRCLE_RESOLUTION-2;

	double anglePerPoint = 2 * PI / outerPoints;
	float pointX = 0;
	float pointY = 0;
	for(int k = 0; k <= outerPoints ; k++) {
		pointX = CIRCLE_RADIUS * cos(anglePerPoint * k) - 0.875;
		pointY = CIRCLE_RADIUS * sin(anglePerPoint * k) - 0.875;
		squareCheckerMesh.push_back(pointX);
		squareCheckerMesh.push_back(pointY);
	}

	// Generate a GPU side buffer
	glGenBuffers(1, &positionBuffer);
	
	// Tell OpenGL we want to work with the buffer we just made
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	
	// Allocate and upload data to GPU
	glBufferData(GL_ARRAY_BUFFER, squareCheckerMesh.size() * sizeof(GLfloat), squareCheckerMesh.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


Board::~Board(void)
{
	glDeleteBuffers(1, &positionBuffer);
}


void Board::display(bool pickingEnabled){
	// Clear the color bits in the display buffer
	GLfloat transform[] = { 1,0,0, 0,1,0, 0,0,1};
	if(pickingEnabled){
		glClearColor(0,0,0,0);
	} else{
		glClearColor(1,1,1,1);
	}
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Use a simple shader to render the line
	glUseProgram(shaderProg);
	
	// Tell OpenGL we want to use a buffer on the GPU
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	
	// Tell OpenGL what shader data slot we want to use
	glEnableVertexAttribArray(positionSlot);
	glVertexAttribPointer(positionSlot, 2, GL_FLOAT, GL_FALSE, 0, 0);
	// Tell OpenGL how to interpret the data
	
	
	for(int i = 0; i < 8; i++){
		transform[7] = i*0.25;
		for(int k = 0; k < 4; k++){
			transform[6] = k*0.5 + 0.25*(i%2);
			glUniformMatrix3fv(transformSlot, 1, GL_FALSE, transform);
			//Draw background square
			if(pickingEnabled) {
				glUniform4f(colorSlot, 
					((float)(board[i][k] & 0xFF))/255.0f, 
					((float)((board[i][k] >> 8) & 0xFF))/255.0f, 
					((float)((board[i][k] >> 16) & 0xFF))/255.0f,
					((float)((board[i][k] >> 24) & 0xFF))/255.0f);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			} else { 
				glUniform4f(colorSlot, 0, 0, 1, 1);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

				// Do the same thing for colors
				if(board[i][k] & 0x0000ff00){
					glUniform4f(colorSlot, 1, 0, 0, 1);
					glDrawArrays(GL_TRIANGLE_FAN, 4, CIRCLE_RESOLUTION);
				} else if (board[i][k] & 0x00ff0000){
					glUniform4f(colorSlot, 0, 0, 0, 1);
					glDrawArrays(GL_TRIANGLE_FAN, 4, CIRCLE_RESOLUTION);
				}
			}
		}
	}
	// Tell OpenGL we are done with the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// Tell OpenGL we are done with the shader slot
	glDisableVertexAttribArray(positionSlot);
	// Tell OpenGL we are done with the shader
	glUseProgram(0);
}

bool Board::piecePickUp(unsigned int pickedID){
	if(pickedUpPieceID != 0){
		return false;
	}

	if(pickedID < 1 << 8) {
		//We didn't click a checker piece, so nothing to pickup
		return false;
	}

	for(int i = 0; i < 8; i++){
		for(int k = 0; k < 4; k++){
			if(pickedID == board[i][k]){
				pickedUpPieceID = pickedID;
				break;
			}
		}
	}

	if(pickedUpPieceID == 0){
		//bad data, piece not found (this shouldn't happen)
		return false;
	}	
	return true;
}

/*
	TODO:
		Movement rules
			limit movement on distance
			limit direction
			King rules
		Jumping rules
		Kinging rules (creation, not moving)
*/
bool Board::piecePlace(unsigned int destination){
	int prevI = -1;
	int prevK = -1;
	int destI = -1;
	int destK = -1;
	if(destination == 0) return false;
	if(destination >= 1 << 8){
		//destination isn't empty
		return false;
	} else {
		for(int i = 0; i < 8; i++){
			for(int k = 0; k < 4; k++){
				if(pickedUpPieceID == board[i][k]){
					prevI = i;
					prevK = k;
				}
				if(destination == board[i][k]){
					destI = i;
					destK = k;
				}
			}
		}
		if(prevI == -1 || prevK == -1 || destI == -1 || destK == -1){
			return false;
		} else {
			board[prevI][prevK] = destination;
			board[destI][destK] = pickedUpPieceID;
			pickedUpPieceID = 0;
			return true;
		}
	}

	return false;
}

void Board::newGame(bool flipped){
		/*
		Initialize the board, using separate ids for each place on the board according to what is there
		Kings will be negative ids
	*/
	nextObjectID = 1;
	for(int i = 0; i < 8; i++){
		for(int k = 0; k < 4; k++){
			if(i < 3){
				board[i][k] = nextObjectID << ((flipped) ? 16 : 8);
			} else if(i >= 5) {
				board[i][k] = nextObjectID << ((flipped) ? 8 : 16);
			} else {
				board[i][k] = nextObjectID;
			}
			nextObjectID++;
		}
	}
	pickedUpPieceID = 0;
}

bool Board::cancelPickup(){
	if(pickedUpPieceID >= 1 << 8) {
		pickedUpPieceID = 0;
		return true;
	}
	return false;
}
