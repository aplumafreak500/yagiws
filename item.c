#include <stddef.h>
#include "item.h"

const char* getItem(unsigned int id) {
	const char* ret;
	// TODO There's gotta be a better way to do this, right?
	ret = getCharacter(id);
	if (ret != NULL) return ret;
	ret = getWeapon(id);
	if (ret != NULL) return ret;
	ret = getArtifact(id);
	if (ret != NULL) return ret;
	// TODO Other items
	return NULL;
}
