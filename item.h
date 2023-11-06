#ifndef ITEM_H
#define ITEM_H
// Characters
#define MAX_CHARS 90

const char* getCharacter(unsigned int);

// Artifacts
const char* getArtifact(unsigned int);

// Weapons
enum {
	SWORD = 1,
	CLAYMORE = 2,
	POLEARM = 3,
	CATALYST = 4,
	BOW = 5,
};

const char* getWeapon(unsigned int);

// Other Items TODO
const char* getItem(unsigned int);
#endif
