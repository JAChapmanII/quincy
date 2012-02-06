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

// Various locations an iterator can be at currently
typedef enum { IT_FRONT, IT_BACK, IT_NODE, IT_INVALID } IteratorType;

// A ${NAME} bidirectional iterator
typedef struct {
	IteratorType type;
	${NAME}_Node *current;
	${NAME} *map;
} ${NAME}_Iterator;

/* Create an empty ${NAME} */
${NAME} *${VNAME}_create(void);
/* Free all memory associated with a ${NAME} */
void ${VNAME}_free(${NAME} *${VNAME});

/* Create a ${NAME}_Node object */
${NAME}_Node *${VNAME}n_create(${KEY_TYPE} key, ${VAL_TYPE} val);
/* Free space associated with a ${NAME}_Node */
void ${VNAME}n_free(${NAME}_Node *${VNAME}n);

// Create a ${NAME}_Iterator object
${NAME}_Iterator *${VNAME}i_create(void);
// Free space assoicated with a ${NAME}_Iterator
void ${VNAME}i_free(${NAME}_Iterator *${VNAME}i);

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

// Search a ${NAME} for the minimum element
${NAME}_Node *${VNAME}_min(${NAME} *${VNAME});
// Search a ${NAME} for the maximum element
${NAME}_Node *${VNAME}_max(${NAME} *${VNAME});

// Search a ${NAME} for the parent of the given element
${NAME}_Node *${VNAME}_parent(${NAME} *${VNAME}, ${NAME}_Node *${VNAME}n);
// Search a ${NAME} for the element occurring after a given element
${NAME}_Node *${VNAME}_next(${NAME} *${VNAME}, ${NAME}_Node *${VNAME}n);
// Search a ${NAME} for the element occurring before a given element
${NAME}_Node *${VNAME}_prev(${NAME} *${VNAME}, ${NAME}_Node *${VNAME}n);

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
