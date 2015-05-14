#include <bits/stdc++.h>
#include <inttypes.h>

// CONSTANTS
const int cubeSize = 4; // 4x4x4
const int numRotations = 64;

#define VERTICAL true
#define HORIZONTAL false

// TYPE DEFINITIONS
typedef int8_t Rotation;

// OTHER MACROS
#define getXRotation(rotation) ((rotation)&3) // xy plane
#define getYRotation(rotation) (((rotation)>>2)&3) // yz plane
#define getZRotation(rotation) (((rotation)>>4)&3) // xz plane

// STRUCTS/CLASSES
struct Position {
	int x, y, z;
	inline Position(): x(0), y(0), z(0) {}
	inline Position(int x_, int y_, int z_): x(x_), y(y_), z(z_) {}
};

class Piece {
	Position blocks[numRotations][8];

	// Applies a rotation to a point in the space.
	// Returns the position of the rotated point.
	inline Position rotate(Position& p, Rotation& r) {
		int xRotation = getXRotation(r); // xy plane
		int yRotation = getYRotation(r); // yz plane
		int zRotation = getZRotation(r); // xz plane

		int x = p.x,
			y = p.y,
			z = p.z;

		// Applies xRotation (90deg rotation)
		while (xRotation-- > 0) {
			int y_ = y;
			y = -x;
			x = y_;
		}

		// Applies yRotation (-90deg rotation)
		while (yRotation-- > 0) {
			int y_ = y;
			y = -z;
			z = y_;
		}

		// Applies zRotation (-90deg rotation)
		while (zRotation-- > 0) {
			int x_ = x;
			x = -z;
			z = x_;
		}

		return Position(x,y,z);
	}

public:
	// Assumes the first part of each piece is (0,0,0,HORIZONTAL)
	Piece(int x, int y, int z, bool isVertical) {
		// Generates the blocks for non-rotated piece
		Position* nrBlocks = blocks[0];

		// First part
		nrBlocks[0] = Position(0,0,0),
		nrBlocks[1] = Position(0,1,0),
		nrBlocks[2] = Position(1,0,0),
		nrBlocks[3] = Position(1,1,0);

		// Second part
		nrBlocks[4] = Position(x,y,z);
		nrBlocks[5] = Position(x+1,y,z);
		if (isVertical) {
			nrBlocks[6] = Position(x,y,z+1);
			nrBlocks[7] = Position(x+1,y,z+1);
		} else {
			nrBlocks[6] = Position(x,y+1,z);
			nrBlocks[7] = Position(x+1,y+1,z);
		}

		// Generates the blocks for all rotations
		for (Rotation i = 1; i < numRotations; i++)
			for (int j = 0; j < 8; j++)
				blocks[i][j] = rotate(nrBlocks[j], i);
	}

	// Fills the cube with this piece, r-rotated, using p as origin.
	// Returns true if it was successful; false otherwise.
	inline bool fill(bool cube[][cubeSize][cubeSize], Rotation& r, Position& p) {
		Position* rBlocks = blocks[r];

		// Rotates the blocks and checks whether it is possible to fill or not
		for (int i = 0; i < 8; i++) {
			Position& pos = rBlocks[i];
			int x = p.x + pos.x,
				y = p.y + pos.y,
				z = p.z + pos.z;

			// Out of bounds
			if (x < 0 || y < 0 || z < 0 || x >= cubeSize || y >= cubeSize || z >= cubeSize)
				return false;
			// This position is already filled in the cube
			if (cube[x][y][z])
				return false;
		}

		// Fills the cube
		for (int i = 0; i < 8; i++) {
			Position& pos = rBlocks[i];
			int x = p.x + pos.x,
				y = p.y + pos.y,
				z = p.z + pos.z;
			cube[x][y][z] = true;
		}

		return true;
	}

	// Gets a position, translated in relation to a block of this piece, when
	// r-rotated.
	inline Position getTranslatedPosition(Position& p, int blockIndex, Rotation r=0) {
		Position bPos = blocks[r][blockIndex];
		return Position(p.x - bPos.x, p.y - bPos.y, p.z - bPos.z);
	}
};

// PIECES
const Piece pieces[] = {
	Piece(1,0,1, VERTICAL),
	Piece(1,1,1, HORIZONTAL),
	Piece(2,0,0, VERTICAL),
	Piece(0,1,1, HORIZONTAL),
	Piece(2,1,0, HORIZONTAL),
	Piece(2,1,0, HORIZONTAL),
	Piece(2,1,0, VERTICAL),
	Piece(0,1,1, VERTICAL)
};
const int numPieces = sizeof(pieces) / sizeof(pieces[0]);

// GLOBAL VARIABLES
bool cube[cubeSize][cubeSize][cubeSize];

// ALGORITHM
template<int MAXB>
bool quick_bruteforce(Position p=Position(), int piecesMask=0) {
	// Checks if we found a valid combination
	if (piecesMask == 0xff)
		return true;

	// Finds next empty position p in our cube
	while (cube[p.x][p.y][p.z]) {
		if (++p.z >= cubeSize)
			p.z = 0, p.y++;
		if (p.y >= cubeSize)
			p.y = 0, p.x++;
	}

	bool originalCube[cubeSize][cubeSize][cubeSize];
	memcpy(originalCube, cube, sizeof(cube));

	// Tests all non-used pieces into this empty position
	for (int i = 0; i < 8; i++) {
		if ((piecesMask >> i) & 1)
			continue;

		// Includes piece i in the mask
		int piecesMask_ = piecesMask | (1 << i);
		Piece piece = pieces[i];

		// Tries all possible rotations
		for (Rotation r = 0; r < numRotations; r++) {
			// Tries blocks 0..MAXB-1 from this rotated piece as origin
			for (int b = 0; b < MAXB; b++) {
				// Translates our point according to the new origin (chosen block b)
				Position translatedPos = piece.getTranslatedPosition(p, b, r);

				if (!piece.fill(cube, r, translatedPos))
					continue;
	
				if (quick_bruteforce<MAXB>(p, piecesMask_)) {
					printf("Piece %d at <%d,%d,%d> with rotation <%d,%d,%d>\n",
						i, p.x,p.y,p.z, getXRotation(r), getYRotation(r),
						getZRotation(r));
					return true;
				}

				memcpy(cube, originalCube, sizeof(cube));
			}
		}
	}

	return false;
}

void quick_bruteforce() {
	if (quick_bruteforce<2>()) return;
	if (quick_bruteforce<4>()) return;
	if (quick_bruteforce<6>()) return;
	if (quick_bruteforce<8>()) return;
}

int main() {
	time_t t = clock();
	quick_bruteforce();
	printf("Finished in %d ms.\n", clock() - t);
	return 0;
}

