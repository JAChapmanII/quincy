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

	printf("Testing forward iterator (fit)\n");
	ISMap_Node *cur = ismap_min(map);
	if(!cur) {
		fprintf(stderr, "test: min returned NULL\n");
		return 2;
	}

	int seen = 0;
	do {
		seen++;
		if(cur->key != keys[seen - 1])
			fprintf(stderr, "test: expected key does not match cur->key [fit]\n");
		cur = ismap_next(map, cur);
	} while(cur != NULL);

	if(seen != nodeCount) {
		fprintf(stderr, "test: seen != nodeCount [fit]\n");
		return 3;
	}

	printf("Testing reverse iterator (rit)\n");
	cur = ismap_max(map);
	if(!cur) {
		fprintf(stderr, "test: max returned NULL\n");
		return 2;
	}

	seen = 0;
	do {
		seen++;
		if(cur->key != keys[nodeCount - seen])
			fprintf(stderr, "test: expected key does not match cur->key [rit]\n");
		cur = ismap_prev(map, cur);
	} while(cur != NULL);

	if(seen != nodeCount) {
		fprintf(stderr, "test: seen != nodeCount [rit]\n");
		return 3;
	}

	printf("Everything good\n");

	return 0;
}

