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

	SIMap *simap = simap_create();
	simap_add(simap, "1", 1);
	simap_add(simap, "2", 2);
	simap_add(simap, "3", 3);
	simap_add(simap, "4", 4);
	simap_add(simap, "5", 5);

	SIMap_Node *simapn = simap_find(simap, "4");
	if(simapn != NULL)
		printf("found 4: %u\n", simapn->val);
	else
		printf("error!\n");

	return 0;
}

