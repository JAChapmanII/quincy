#include <stdio.h>
#include "conf.h"
#include "vmap.h"

void vmap_print(VMap *map);
void vmapn_print(VMap_Node *mapn);

int main(int argc, char **argv) {
	conf_parseArguments(argv, argc);
	conf_read(NULL);
	char *server = conf_server(), *nick = conf_nick(), *chan = conf_chan(),
		*binary = conf_binary();
	int port = conf_port();

	printf("server: \"%s\"\n", server);
	printf("nick: \"%s\"\n", nick);
	printf("chan: \"%s\"\n", chan);
	printf("binary: \"%s\"\n", binary);
	printf("port: \"%d\"\n", port);

	VMap *confMap = conf_map();
	if(confMap == NULL)
		fprintf(stderr, "main: confMap null\n");
	else
		vmap_print(confMap);
}

void vmap_print(VMap *map) {
	if(map->root == NULL)
		printf("empty\n");
	else
		vmapn_print(map->root);
}
void vmapn_print(VMap_Node *mapn) {
	if(mapn == NULL)
		return;
	vmapn_print(mapn->left);
	printf("%s: %s\n", mapn->key, mapn->val);
	vmapn_print(mapn->right);
}

