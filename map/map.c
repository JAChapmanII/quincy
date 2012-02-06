#include "${FILE}.h"

#include <stdlib.h>
${C_INCLUDE}

#include "util.h"

#define SUCCESS        0
#define FAILURE        1
#define INVALID_SOURCE 8
#define ALLOC_FAIL     9

<<<--- FUNCTION_DECLARATIONS

void ${VNAME}n_fixHeight(${NAME}_Node *${VNAME}n);

${NAME}_Node *${VNAME}n_add(${NAME}_Node *${VNAME}n,
		${KEY_TYPE} key, ${VAL_TYPE} val);
${NAME}_Node *${VNAME}n_find(${NAME}_Node *${VNAME}n, ${KEY_TYPE} key);
${NAME}_Node *${VNAME}n_min(${NAME}_Node *${VNAME}n);
${NAME}_Node *${VNAME}n_max(${NAME}_Node *${VNAME}n);
int ${VNAME}n_kequals(${NAME}_Node *left, ${NAME}_Node *right);

${NAME}_Node *${VNAME}n_erase(${NAME}_Node *${VNAME}n, ${KEY_TYPE} key);

${NAME}_Node *${VNAME}n_balance(${NAME}_Node *${VNAME}n);
${NAME}_Node *${VNAME}n_rightRotation(${NAME}_Node *n);
${NAME}_Node *${VNAME}n_leftRotation(${NAME}_Node *n);

size_t ${VNAME}n_size(${NAME}_Node *${VNAME}n);

${NAME} *${VNAME}_create(void) { // {{{
	${NAME} *${VNAME} = malloc(sizeof(${NAME}));
	if(!${VNAME})
		return NULL;
	${VNAME}->root = NULL;
	return ${VNAME};
} // }}}
void ${VNAME}_free(${NAME} *${VNAME}) { // {{{
	if(!${VNAME})
		return;
	if(${VNAME}->root)
		${VNAME}n_free(${VNAME}->root);
	free(${VNAME});
} // }}}

${NAME}_Node *${VNAME}n_create(${KEY_TYPE} key, ${VAL_TYPE} val) { // {{{
	if(!key || !val)
		return NULL;
	${NAME}_Node *${VNAME}n = malloc(sizeof(${NAME}_Node));
	if(!${VNAME}n)
		return NULL;
	${VNAME}n->left = ${VNAME}n->right = NULL;
	${VNAME}n->height = 1;

	${INIT_KEY}(&${VNAME}n->key);
	${INIT_VAL}(&${VNAME}n->val);

	${COPY_KEY}(&${VNAME}n->key, &key);
	${COPY_VAL}(&${VNAME}n->val, &val);

	if(!${VNAME}n->key || !${VNAME}n->val) {
		${VNAME}n_free(${VNAME}n);
		return NULL;
	}
	return ${VNAME}n;
} // }}}
void ${VNAME}n_free(${NAME}_Node *${VNAME}n) { // {{{
	if(!${VNAME}n)
		return;
	if(${VNAME}n->left)
		${VNAME}n_free(${VNAME}n->left);
	if(${VNAME}n->right)
		${VNAME}n_free(${VNAME}n->right);
	${KEY_TYPE} key = ${VNAME}n->key;
	if(${KEY_VALID})
		${FREE_KEY}(${VNAME}n->key);
	${VAL_TYPE} val = ${VNAME}n->val;
	if(${VAL_VALID})
		${FREE_VAL}(${VNAME}n->val);
	free(${VNAME}n);
} // }}}

${NAME}_Iterator *${VNAME}i_create(${NAME} *${VNAME}) { // {{{
	${NAME}_Iterator *${VNAME}i = malloc(sizeof(${NAME}_Iterator));
	if(!{VNAME}i)
		return NULL;
	${VNAME}i->type = IT_INVALID;
	${VNAME}i->map = ${VNAME};
	${VNAME}i->current = NULL;
	return ${VNAME}i;
}// }}}
void ${VNAME}i_free(${NAME}_Iterator *${VNAME}i) { // {{{
	if(!{VNAME}i)
		return;
	free(${VNAME}i);
} // }}}

/* TODO: error handling */
int ${VNAME}_add(${NAME} *${VNAME}, ${KEY_TYPE} key, ${VAL_TYPE} val) {
	if(!${KEY_VALID} || !${VAL_VALID})
		return 1;
	if(!${VNAME}->root) {
		${VNAME}->root = ${VNAME}n_create(key, val);
		if(!${VNAME}->root)
			return 1;
		return 0;
	}
	${VNAME}->root = ${VNAME}n_add(${VNAME}->root, key, val);
	return 0;
}
${NAME}_Node *${VNAME}n_add(${NAME}_Node *${VNAME}n, // {{{
		${KEY_TYPE} key, ${VAL_TYPE} val) {
	// TODO: this was just return; ....
	if(!${VNAME}n || !${KEY_VALID} || !${VAL_VALID}) return ${VNAME}n;
	int cmp = ${KEY_COMP}(${VNAME}n->key, key);
	// TODO: error on this condition? This makes _set obsolete
	if(!cmp) {
		if(${COPY_VAL}(&${VNAME}n->val, &val) != SUCCESS) {
			// TODO: handle errors
		}
		return ${VNAME}n;
	}

	${NAME}_Node **child;
	if(cmp > 0)
		child = &${VNAME}n->left;
	else
		child = &${VNAME}n->right;

	if(!*child)
		*child = ${VNAME}n_create(key, val);
	else
		*child = ${VNAME}n_add(*child, key, val);

	${VNAME}n_fixHeight(${VNAME}n);
	return ${VNAME}n_balance(${VNAME}n);
} // }}}

uint8_t ${VNAME}n_height(${NAME}_Node *${VNAME}n) { // {{{
	if(!${VNAME}n)
		return 0;
	return ${VNAME}n->height;
} // }}}

// TODO: implement
int ${VNAME}_set(${NAME} *${VNAME}, ${KEY_TYPE} key, ${VAL_TYPE} val);

${NAME}_Node *${VNAME}_min(${NAME} *${VNAME}) { // {{{
	if(!${VNAME} || !${VNAME}->root)
		return NULL;
	return ${VNAME}n_min(${VNAME}->root);
} // }}}
${NAME}_Node *${VNAME}n_min(${NAME}_Node *${VNAME}n) { // {{{
	if(!${VNAME}n)
		return NULL;
	if(${VNAME}n->left)
		return ${VNAME}n_min(${VNAME}n->left);
	return ${VNAME}n;
} // }}}

${NAME}_Node *${VNAME}_max(${NAME} *${VNAME}) { // {{{
	if(!${VNAME} || !${VNAME}->root)
		return NULL;
	return ${VNAME}n_max(${VNAME}->root);
} // }}}
${NAME}_Node *${VNAME}n_max(${NAME}_Node *${VNAME}n) { // {{{
	if(!${VNAME}n)
		return NULL;
	if(${VNAME}n->right)
		return ${VNAME}n_max(${VNAME}n->right);
	return ${VNAME}n;
} // }}}

int ${VNAME}n_kequals(${NAME}_Node *left, ${NAME}_Node *right) { // {{{
	if((left == NULL) || (right == NULL))
		return 0;
	return (${KEY_COMP}(left->key, right->key) == 0);
} // }}}

// TODO: the find method can probably use this, then simply look at the left
// TODO: or right child for the final result.
${NAME}_Node *${VNAME}_parent(${NAME} *${VNAME}, ${NAME}_Node *${VNAME}n) { // {{{
	if(!${VNAME} || !${VNAME}->root || !${VNAME}n)
		return NULL;
	${NAME}_Node *cur = ${VNAME}->root;
	// TODO: currently if the the target is the root, we skip over it and
	// TODO: return a NULL. We may want to keep these semantics, but bail early?
	do {
		if(${VNAME}n_kequals(cur->left, ${VNAME}n) ||
				${VNAME}n_kequals(cur->right, ${VNAME}n))
			return cur;
		int cmp = ${KEY_COMP}(cur->key, ${VNAME}n->key);
		if(cmp > 0)
			cur = cur->left;
		else
			cur = cur->right;
	} while(cur != NULL);
	return NULL;
} // }}}

int ${VNAME}_erase(${NAME} *${VNAME}, ${KEY_TYPE} key) { // {{{
	if(!${VNAME} || !${KEY_VALID})
		return 1;
	${VNAME}->root = ${VNAME}n_erase(${VNAME}->root, key);
	return 0;
} // }}}
${NAME}_Node *${VNAME}n_erase(${NAME}_Node *${VNAME}n, ${KEY_TYPE} key) { // {{{
	if(!${VNAME}n || !${KEY_VALID})
		return NULL;
	int cmp = ${KEY_COMP}(${VNAME}n->key, key);
	if(!cmp) {
		if(!${VNAME}n->left && !${VNAME}n->right) {
			${VNAME}n_free(${VNAME}n);
			return NULL;
		}
		if(${VNAME}n->left && !${VNAME}n->right) {
			${NAME}_Node *l = ${VNAME}n->left;
			${VNAME}n->left = NULL;
			${VNAME}n_free(${VNAME}n);
			return l;
		}
		if(!${VNAME}n->left && ${VNAME}n->right) {
			${NAME}_Node *r = ${VNAME}n->right;
			${VNAME}n->right = NULL;
			${VNAME}n_free(${VNAME}n);
			return r;
		}
		// replace with min from right tree
		${NAME}_Node *min = ${VNAME}n_min(${VNAME}n->right);
		if(!min) {
			; // TODO: panic
		}
		${NAME}_Node *m = ${VNAME}n_create(min->key, min->val);
		if(!m) {
			; // TODO: panic
		}
		// this should be one of the simple cases above
		${VNAME}n->right = ${VNAME}n_erase(${VNAME}n->right, min->key);

		m->left = ${VNAME}n->left;
		m->right = ${VNAME}n->right;
		m->height = ${VNAME}n->height;

		${VNAME}n->left = ${VNAME}n->right = NULL;
		${VNAME}n_free(${VNAME}n);

		${VNAME}n_fixHeight(m);
		return ${VNAME}n_balance(m);
	}
	if(cmp > 0) {
		if(!${VNAME}n->left)
			return ${VNAME}n;
		${VNAME}n->left = ${VNAME}n_erase(${VNAME}n->left, key);
	} else {
		if(!${VNAME}n->right)
			return ${VNAME}n;
		${VNAME}n->right = ${VNAME}n_erase(${VNAME}n->right, key);
	}
	${VNAME}n_fixHeight(${VNAME}n);
	return ${VNAME}n_balance(${VNAME}n);
} // }}}

void ${VNAME}n_fixHeight(${NAME}_Node *${VNAME}n) { // {{{
	if(!${VNAME}n)
		return;
	int lh = ${VNAME}n_height(${VNAME}n->left),
		rh = ${VNAME}n_height(${VNAME}n->right);
	${VNAME}n->height = ((lh > rh) ? lh : rh) + 1;
} // }}}

${NAME}_Node *${VNAME}n_balance(${NAME}_Node *${VNAME}n) { // {{{
	int lh = ${VNAME}n_height(${VNAME}n->left), rh = ${VNAME}n_height(${VNAME}n->right);
	if(rh - lh > 1) {
		lh = ${VNAME}n_height(${VNAME}n->right->left);
		rh = ${VNAME}n_height(${VNAME}n->right->right);
		if(lh > rh)
			${VNAME}n->right = ${VNAME}n_rightRotation(${VNAME}n->right);
		${VNAME}n = ${VNAME}n_leftRotation(${VNAME}n);
	} else if(lh - rh > 1) {
		lh = ${VNAME}n_height(${VNAME}n->left->left);
		rh = ${VNAME}n_height(${VNAME}n->left->right);
		if(rh > lh)
			${VNAME}n->left = ${VNAME}n_leftRotation(${VNAME}n->left);
		${VNAME}n = ${VNAME}n_rightRotation(${VNAME}n);
	}
	return ${VNAME}n;
} // }}}

${NAME}_Node *${VNAME}n_rightRotation(${NAME}_Node *n) { // {{{
	${NAME}_Node *nr = n->left, *c = n->left->right;
	nr->right = n;
	n->left = c;

	${VNAME}n_fixHeight(n);
	${VNAME}n_fixHeight(nr);
	return nr;
} // }}}
${NAME}_Node *${VNAME}n_leftRotation(${NAME}_Node *n) { // {{{
	${NAME}_Node *nr = n->right, *b = n->right->left;
	nr->left = n;
	n->right = b;

	${VNAME}n_fixHeight(n);
	${VNAME}n_fixHeight(nr);
	return nr;
} // }}}

${NAME}_Node *${VNAME}_find(${NAME} *${VNAME}, ${KEY_TYPE} key) { // {{{
	if(!${VNAME}->root || !${KEY_VALID})
		return NULL;
	return ${VNAME}n_find(${VNAME}->root, key);
} // }}}
${NAME}_Node *${VNAME}n_find(${NAME}_Node *${VNAME}n, ${KEY_TYPE} key) { // {{{
	if(!${VNAME}n || !${KEY_VALID})
		return NULL;
	int cmp = ${KEY_COMP}(${VNAME}n->key, key);
	if(!cmp)
		return ${VNAME}n;
	if(cmp > 0)
		return ${VNAME}n_find(${VNAME}n->left, key);
	return ${VNAME}n_find(${VNAME}n->right, key);
} // }}}

size_t ${VNAME}_size(${NAME} *${VNAME}) { // {{{
	if(!${VNAME}->root)
		return 0;
	return ${VNAME}n_size(${VNAME}->root);
} // }}}
size_t ${VNAME}n_size(${NAME}_Node *${VNAME}n) { // {{{
	if(!${VNAME}n)
		return 0;
	/* NULL children are handled above */
	return ${VNAME}n_size(${VNAME}n->left) + ${VNAME}n_size(${VNAME}n->right) + 1;
} // }}}

int ${VNAME}_load(${NAME} *${VNAME}, FILE *inFile) { // {{{
	if(!${VNAME} || !inFile)
		return -1;

	size_t size = util_readSizeT(inFile);
	if(size == 0)
		return -2;

	size_t rnodes = 0;
	for(; rnodes < size; ++rnodes) {
		${KEY_TYPE} key;
		${VAL_TYPE} val;

		if(${READ_KEY}(&key, inFile) != SUCCESS) {
			// TODO: handle errors?
			fprintf(stderr, "${VNAME}_load: error on READ_KEY\n");
		}
		if(${READ_VAL}(&val, inFile) != SUCCESS) {
			// TODO: handle errors?
			fprintf(stderr, "${VNAME}_load: error on READ_VAL\n");
		}

		// add the node to the ${NAME}
		${VNAME}_add(${VNAME}, key, val);
	}

	if(rnodes != size) {
		fprintf(stderr, "rn: %lu, s: %lu\n", rnodes, size);
		return -3;
	}
	return rnodes;
} // }}}
int ${VNAME}_read(${NAME} *${VNAME}, char *fileName) { // {{{
	if(!${VNAME} || !fileName)
		return 0;

	FILE *dumpFile = fopen(fileName, "r");
	if(!dumpFile)
		return 0;

	int res = ${VNAME}_load(${VNAME}, dumpFile);
	fclose(dumpFile);

	return res;
} // }}}

int ${VNAME}n_dump(${NAME}_Node *${VNAME}n, FILE *dumpFile) { // {{{
	if(!${VNAME}n || !dumpFile)
		return 0;

	int count = 1;

	if(${WRITE_KEY}(${VNAME}n->key, dumpFile) != SUCCESS) {
		// TODO: handle errors?
		fprintf(stderr, "${VNAME}n_dump: error during WRITE_KEY\n");
		count = 0;
	} else {
		if(${WRITE_VAL}(${VNAME}n->val, dumpFile) != SUCCESS) {
			// TODO: handle errors?
			fprintf(stderr, "${VNAME}n_dump: error during WRITE_VAL\n");
			count = 0;
		}
	}

	count += ${VNAME}n_dump(${VNAME}n->left, dumpFile);
	count += ${VNAME}n_dump(${VNAME}n->right, dumpFile);
	return count;
} // }}}
int ${VNAME}_write(${NAME} *${VNAME}, FILE *outFile) { // {{{
	if(!${VNAME} || !outFile)
		return -1;

	size_t size = ${VNAME}_size(${VNAME});
	if(size < 1)
		return 0;

	if(util_writeSizeT(outFile, size) != sizeof(size_t))
		return -128;

	int count = ${VNAME}n_dump(${VNAME}->root, outFile);
	return count;
} // }}}
int ${VNAME}_dump(${NAME} *${VNAME}, char *fileName) { // {{{
	if(!${VNAME} || !fileName)
		return 0;

	FILE *dumpFile = fopen(fileName, "wb");
	if(!dumpFile)
		return 0;

	int count = ${VNAME}_write(${VNAME}, dumpFile);
	fclose(dumpFile);
	return count;
} // }}}

int ${VNAME}n_writeDot(${NAME}_Node *${VNAME}n, FILE *of, int nullCount) { // {{{
	if(!of || !${VNAME}n)
		return 0;
	char *keyString = ${FORMAT_KEY}(${VNAME}n->key);
	char *valString = ${FORMAT_VAL}(${VNAME}n->val);
	fprintf(of, "\t\"%s = %s\";\n", keyString, valString);
	if(${VNAME}n->left) {
		char *lKeyString = ${FORMAT_KEY}(${VNAME}n->left->key);
		char *lValString = ${FORMAT_VAL}(${VNAME}n->left->val);
		fprintf(of, "\t\"%s = %s\" -> \"%s = %s\";\n", keyString, valString,
				lKeyString, lValString);
		free(lKeyString);
		free(lValString);
		nullCount += ${VNAME}n_writeDot(${VNAME}n->left, of, nullCount);
	} else {
		fprintf(of, "null%d [shape=point]\n", nullCount);
		fprintf(of, "\t\"%s = %s\" -> null%d\n", keyString, valString, nullCount);
		nullCount++;
	}
	if(${VNAME}n->right) {
		char *rKeyString = ${FORMAT_KEY}(${VNAME}n->right->key);
		char *rValString = ${FORMAT_VAL}(${VNAME}n->right->val);
		fprintf(of, "\t\"%s = %s\" -> \"%s = %s\";\n", keyString, valString,
				rKeyString, rValString);
		free(rKeyString);
		free(rValString);
		nullCount += ${VNAME}n_writeDot(${VNAME}n->right, of, nullCount);
	} else {
		fprintf(of, "null%d [shape=point]\n", nullCount);
		fprintf(of, "\t\"%s = %s\" -> null%d\n", keyString, valString, nullCount);
		nullCount++;
	}
	free(keyString);
	free(valString);
	return nullCount;
} // }}}
int ${VNAME}_writeDot(${NAME} *${VNAME}, char *outputName) { // {{{
	if(!${VNAME} || !outputName)
		return 1;

	FILE *of = fopen(outputName, "w");
	if(!of)
		return 2;

	fprintf(of, "digraph BST {\n");
	fprintf(of, "\tnode [fontname=\"Arial\"];\n");
	${VNAME}n_writeDot(${VNAME}->root, of, 0);
	fprintf(of, "}\n");

	fclose(of);
	return 0;
} // }}}

<<<--- FUNCTION_IMPLEMENTATIONS

