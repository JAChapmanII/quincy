#include <stdlib.h>
#include <time.h>
#include "ismap.h"

int main(int argc, char **argv) {
	srand(time(NULL));
	int nodeCount = 16;
	if(argc > 1) {
		nodeCount = atoi(argv[1]);
		if(nodeCount == 0)
			nodeCount = 16;
	}

	ISMap_Iterator *it = ismapi_create();
	if(!it) {
		fprintf(stderr, "test: could not create ISMap_Iterator\n");
		return 1;
	}

	ISMap *map = ismap_create();
	if(!map) {
		fprintf(stderr, "test: could not create ISMap\n");
		return 1;
	}

	uint32_t key = 0, keys[nodeCount];
	printf("Adding: ");
	for(int i = 0; i < nodeCount; ++i) {
		key += (rand() % 3) + 1;
		keys[i] = key;
		printf("%d", key);
		if(i != nodeCount - 1)
			printf(", ");
		ismap_add(map, key, "v");
	}
	printf("\n");

	ismapi_front(it, map);
	printf("Testing forward iterator (fit)\n");
	int seen = 0;
	while(it->type != IT_END) {
		seen++;
		if(it->current->key != keys[seen - 1])
			fprintf(stderr, "test: expected key does not match current [fit]\n");
		ismapi_next(it);
	}
	if(seen != nodeCount) {
		fprintf(stderr, "test: seen != nodeCount [fit]\n");
		return 3;
	}

	ismapi_back(it, map);
	printf("Testing reverse iterator (rit)\n");
	seen = 0;
	while(it->type != IT_BEGIN) {
		seen++;
		if(it->current->key != keys[nodeCount - seen])
			fprintf(stderr, "test: expected key does not match currrent [rit]\n");
		ismapi_prev(it);
	}
	if(seen != nodeCount) {
		fprintf(stderr, "test: seen != nodeCount [rit]\n");
		return 3;
	}

	printf("Everything good\n");
	return 0;
}

