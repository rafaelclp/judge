#include <bits/stdc++.h>
#include <inttypes.h>

// CONSTANTS
const int cubeSize = 4; // 4x4x4

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
	Position blocks[8];

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
		// First part
		blocks[0] = Position(0,0,0),
		blocks[1] = Position(0,1,0),
		blocks[2] = Position(1,0,0),
		blocks[3] = Position(1,1,0);

		// Second part
		blocks[4] = Position(x,y,z);
		blocks[5] = Position(x+1,y,z);
		if (isVertical) {
			blocks[6] = Position(x,y,z+1);
			blocks[7] = Position(x+1,y,z+1);
		} else {
			blocks[6] = Position(x,y+1,z);
			blocks[7] = Position(x+1,y+1,z);
		}
	}

	// Fills the cube with this piece, r-rotated, using p as origin.
	// Returns true if it was successful; false otherwise.
	inline bool fill(bool cube[][cubeSize][cubeSize], Rotation& r, Position& p) {
		static Position rotatedBlocks[8];

		// Rotates the blocks and checks whether it is possible to fill or not
		for (int i = 0; i < 8; i++) {
			rotatedBlocks[i] = rotate(blocks[i], r);

			Position& pos = rotatedBlocks[i];
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
			Position& pos = rotatedBlocks[i];
			int x = p.x + pos.x,
				y = p.y + pos.y,
				z = p.z + pos.z;
			cube[x][y][z] = true;
		}

		return true;
	}
};

// ALGORITHM
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

bool cube[cubeSize][cubeSize][cubeSize];
bool bruteforce(int pieceIndex=0) {
	const int numRotations = 64;

	// Checks if we found a valid combination
	if (pieceIndex == numPieces)
		return true;

	Piece piece = pieces[pieceIndex];

	bool originalCube[cubeSize][cubeSize][cubeSize];
	memcpy(originalCube, cube, sizeof(cube));

	for (int Ox = 0; Ox < cubeSize; Ox++)
		for (int Oy = 0; Oy < cubeSize; Oy++)
			for (int Oz = 0; Oz < cubeSize; Oz++)
				for (Rotation r = 0; r < numRotations; r++) {
					Position p = Position(Ox, Oy, Oz);

					if (!piece.fill(cube, r, p))
						continue;

					if (bruteforce(pieceIndex + 1)) {
						printf("Piece %d at <%d,%d,%d> with rotation <%d,%d,%d>\n",
							pieceIndex, Ox,Oy,Oz, getXRotation(r), getYRotation(r),
							getZRotation(r));
						return true;
					}

					memcpy(cube, originalCube, sizeof(cube));
				}

	return false;
}

int main() {
	time_t t = clock();
	bruteforce();
	printf("Finished in %d ms.\n", clock() - t);
	return 0;
}


