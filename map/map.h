#ifndef ${GUARD}_H
#define ${GUARD}_H

#include <stdint.h>
#include <stdio.h>
${H_INCLUDE}

/* ${NAME}_Node, a node in an AVL tree */
typedef struct ${NAME}_Node {
	${KEY_TYPE} key;
	${VAL_TYPE} val;

	struct ${NAME}_Node *left;
	struct ${NAME}_Node *right;
	uint8_t height;
} ${NAME}_Node;

/* A ${KEY_TYPE} -> ${VAL_TYPE} map backed by an AVL tree */
typedef struct {
	${NAME}_Node *root;
} ${NAME};

/* Create an empty ${NAME} */
${NAME} *${VNAME}_create(void);
/* Free all memory associated with a ${NAME} */
void ${VNAME}_free(${NAME} *${VNAME});

/* Create a ${NAME}_Node object */
${NAME}_Node *${VNAME}n_create(${KEY_TYPE} key, ${VAL_TYPE} val);
/* Free space associated with a ${NAME}_Node */
void ${VNAME}n_free(${NAME}_Node *${VNAME}n);

/* Add a new key/value pair to the tree
 * 	returns 1 on failure, 0 otherwise
 */
int ${VNAME}_add(${NAME} *${VNAME}, ${KEY_TYPE} key, ${VAL_TYPE} val);
/* Update the value in an already existing node */
int ${VNAME}_set(${NAME} *${VNAME}, ${KEY_TYPE} key, ${VAL_TYPE} val);
/* Remove a node from the tree */
int ${VNAME}_erase(${NAME} *${VNAME}, ${KEY_TYPE} key);

/* Search ${NAME} for a specific node */
${NAME}_Node *${VNAME}_find(${NAME} *${VNAME}, ${KEY_TYPE} key);

/* Search a ${NAME} for the minimum element */
${NAME}_Node *${VNAME}_min(${NAME} *${VNAME});

/* Recursively compute the total nodes in a ${NAME} */
size_t ${VNAME}_size(${NAME} *${VNAME});

/* Write a ${NAME} to a file */
int ${VNAME}_write(${NAME} *${VNAME}, FILE *outFile);
/* Try to open a file and write the ${NAME} to it */
int ${VNAME}_dump(${NAME} *${VNAME}, char *fileName);

/* Read a ${NAME} from a file */
int ${VNAME}_load(${NAME} *${VNAME}, FILE *inFile);
/* Try to open a file and load the ${NAME} from it */
int ${VNAME}_read(${NAME} *${VNAME}, char *fileName);

/* Write this ${NAME} out as a graphviz .dot file */
int ${VNAME}_writeDot(${NAME} *${VNAME}, char *outputName);

#endif /* ${GUARD}_H */