#include <stdio.h>

#include "vmap.h"
#include "simap.h"

int main(int argc, char **argv) {
	printf("quincy: 0.0a\n");

	VMap *vmap = vmap_create();
	vmap_add(vmap, "1", "one");
	vmap_add(vmap, "2", "two");
	vmap_add(vmap, "3", "three");
	vmap_add(vmap, "4", "four");
	vmap_add(vmap, "5", "five");

	VMap_Node *vmapn = vmap_find(vmap, "3");
	if(vmapn != NULL)
		printf("found 3: %s\n", vmapn->val);
	else
		printf("error!\n");

	return 0;
}

