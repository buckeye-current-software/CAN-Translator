/*
* Libavl is a library to manage AVL structure to store and organize
* everykind of data. You just need to implement function to compare,
* to desallocate and to print your structure.
*
* DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
* Version 2, December 2004
*
* Copyright (C) 2013 Adrien Oliva <adrien.oliva@yapbreak.fr>
*
* Everyone is permitted to copy and distribute verbatim or modified
* copies of this license document, and changing it is allowed as long
* as the name is changed.
*
* DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
* TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
*
* 0. You just DO WHAT THE FUCK YOU WANT TO.
*/

/**
* \file avl.h
* \author Adrien Oliva
* \date May, 24th, 2011
* \brief Generic AVL-tree library.
* \version 1.0.0
*
* Implementation of an AVL tree to store generic data.
* In computer science, an AVL tree is a self-balancing binary search tree,
* and it was the first such data structure to be invented. In an AVL tree,
* the heights of the two child subtrees of any node differ by at most one.
* Lookup, insertion, and deletion all take \f$\mathcal{O}(\log n)\f$
* time in both the average and worst cases, where \e n is the number of
* nodes in the tree prior to the operation. Insertions and deletions may
* require the tree to be rebalanced by one or more tree rotations.
*
* The AVL tree is named after its two Soviet inventors, G.M. Adelson-Velskii
* and E.M. Landis, who published it in their 1962 paper "An algorithm for the
* organization of information."
*
* The balance factor of a node is the height of its left subtree minus the
* height of its right subtree (sometimes opposite) and a node with balance
* factor 1, 0, or −1 is considered balanced. A node with any other balance
* factor is considered unbalanced and requires rebalancing the tree. The
* balance factor is either stored directly at each node or computed from the
* heights of the subtrees.
*
* \section Usage Use of library
*
* Here is an example of code that store structure in AVL tree:
*
* <pre>
* #include <stdlib.h>
* #include <stdio.h>
* #include "avl.h"
*
* // Structure we want to store
* // key is used to order data
* struct data {
* int key;
* int value;
* };
*
* // Function that compares two struct data
* int data_cmp(void *a, void *b)
* {
* struct data *aa = (struct data *) a;
* struct data *bb = (struct data *) b;
*
* // Protect against NULL pointer
* // It could generally never happened
* if (!aa || !bb)
* return 0;
*
* return aa->key - bb->key;
* }
*
* // Function that dumps data structure
* void data_print(void *d)
* {
* struct data *dd = (struct data *) d;
*
* if (dd)
* printf("{ %d => %d }\n", dd->key, dd->value);
* }
*
* // Function that delete a data structure
* void data_delete(void *d)
* {
* struct data *dd = (struct data *) d;
*
* if (dd) {
* // You can put here all additional needed
* // memory deallocation
* free(dd);
* }
* }
*
* int main(int argc, char *argv)
* {
* tree *avl_tree = NULL;
* struct data tmp;
* int result;
*
* // Initialize a new tree with our three previously defined
* // functions to store data structure.
* avl_tree = init_dictionnary(data_cmp, data_print, data_delete);
*
* tmp.key = 42;
* tmp.value = 4242;
*
* // Add element {42, 4242} in our tree.
* result = insert_elmt(avl_tree, &tmp, sizeof(struct data));
* // Here result is equal to 1 since there is only 1 element in tree.
*
* // Dump tree to stdout with data_print function
* print_tree(avl_tree);
*
* // For all search function, the only value needed in tmp structure
* // is key field.
* tmp.key = 20;
* tmp.value = 0;
*
* if (!is_present(avl_tree, &tmp))
* printf("Key 20 is not found.\n");
*
* tmp.key = 42;
* if (is_present(avl_tree, &tmp))
* printf("Key 42 exist in tree.\n");
*
* if (get_data(avl_tree, &tmp, sizeof(struct data)))
* printf("Now, tmp.key is equal to 4242\n");
*
* delete_node(avl_tree, &tmp);
* if (!is_present(avl_tree, &tmp))
* printf("Key 42 does not exist anymore.\n");
*
* // Free all memory
* delete_tree(avl_tree);
*
* return 0;
* }
* </pre>
*
* You can find this example in folder \b example.
*
* \subsection Tree initialisation
*
* To start a new tree, you need to init a new one with function
* \b init_tree.
*
* \subsection Manage data
*
* The libavl provide all necessary function to store, retrieve and
* browse your data. The following set gives basic operation:
* * \b insert_elmt
* * \b is_present
* * \b get_data
* * \b delete_node
* * \b delete_node_min
*
* Moreover, libavl gives the availability to browse your entire data
* or a subset of your data with:
* * \b explore_tree
* * \b explore_restrain_tree
* * \b print_tree
*
* Finally, libavl take care of your memory and deallocate all memory
* used in a tree when you want to destroy it with \b delete_tree.
*
*/
#ifndef __AVL_H__
#define __AVL_H__

#include <stddef.h>



/** \struct _node
* \brief Node of a tree
*
* Structure that contain all data usefull to organize the tree
*/
struct _node {
        /** Size of subtree */
        unsigned height;
        /** Left son */
        struct _node *left;
        /** Right son */
        struct _node *right;
        /** Pointer to data stored in this node */
        void *data;
};

/**
* \brief Pointer to the \c _node structure.
*/
typedef struct _node *node;

/**
* \brief Tree structure wich contains all necessary element.
*/
typedef struct _tree {
        /** Number of element in tree */
        unsigned count;
        /** Pointer to the first node of tree */
        node root;
        /** \brief External function to compare data
*
* \param a Pointer to first element to compare
* \param b Pointer to second element to compare
*
* \return 0 if a = b, positive if a > b and negative if a < b.
*
* \note \e You must implement this function. It is necessary for the library
* to work and depends on your data you want to store.
*/
        int (* data_cmp) (void *, void *);
        /** \brief External function to print data.
*
* \param d Pointer to data to print.
*
* This function is usefull for debuging program.
*
* \note \e You must implement this function. It is necessary for the library
* to work and depends on your data you want to store.
*/
        void (* data_print) (void *d);

        /** \brief External function to delete data.
*
* \param d Pointer to data to delete.
*
* This function is usefull for when you want to delete data from tree to prevent
* memory leak.
*
* \note \e You must implement this function. It is necessary for the library
* to work and depends on your data you want to store.
*/
        void (* data_delete) (void *d);

        /** \brief External function to copy data.
*
* \param d Pointer to data to copy.
* \return New data allocation with all its fields filled.
*
* \note \e You must implement this function. It is necessary for the library
* to work and depends on your data you want to store.
*/
        void (* data_copy) (void *, void *);
} tree;



/* ************************************************************************* *\
|* EXTERNAL FUNCTION *|
\* ************************************************************************* */

/** \fn tree *init_dictionnary(int (*data_cmp)(void *, void *),
* void (*data_print)(void *),
* void (*data_delete)(void *),
* void (*data_copy)(void *, void *));
* \brief Initialize dictionnary.
*
* \return Pointer to new tree.
*
* \param data_cmp Function to compare data.
* \param data_print Function to print data.
* \param data_delete Function to delete data.
* \param data_copy Function to copy data.
*
* This function return an initilized tree.
*
* \note By default, if you do not provide function usefull to the tree,
* the LibAVL library uses its own function. As LibAVL does not know exactly
* the structure and the definition of your data, result can not be as good
* as expected. Details on these functions are given in the following.
*
* \section LibAVL internal data function
*
* - Compare function: by default, the internal \c data_cmp function will
* return the difference between the two pointer given in arguments
*
* return (int) ((ptrdiff_t) a - (ptrdiff_t) b);
*
* The \c data_cmp function is really critical and guarantee efficiency of
* LibAVL.
* - Print function: by default, the internal \c data_print function will
* print the addresse contained in pointer in argument, in hexadecimal
*
* printf("0x%08x\n", d);
*
* Since \c data_print is mainly used as debug function to dump a complete
* tree, it is not the most important data function, but with the correct
* implementation, it could save lots of time and money when something
* went wrong.
* - Delete function: by default, the internal \c data_delete function will
* call \c free on the argument pointer. If your data is only a static
* structure or a simple type, this function is enough. But for bigger
* object like a string array, it is necessary to provide a new
* \c data_delete function to avoid memory leak.
* - Copy function: by default, the internal \c data_copy function will
* memcpy data.
*/
tree *init_dictionnary(int (*data_cmp)(void *, void *),
                       void (*data_print)(void *),
                       void (*data_delete)(void *),
                       void (*data_copy)(void *, void *));

/** \fn int insert_elmt(tree *t, void *data, size_t datasize);
* \brief Insert new element in tree.
*
* \return Number of element inserted in tree.
* \param t Pointer to tree.
* \param data Pointer to data to add.
* \param datasize Size of data to add.
*
* This function allocate a new memory space with the given size
* and copy object pointed by \c data to the newly created space.
*/
unsigned int insert_elmt(tree *t, void *data, size_t datasize);

/** \fn void verif_tree(tree *t);
* \brief Deffensive check if tree is a real AVL tree.
*
* \param t Pointer to tree.
*
* If tree is not an AVL tree, this function end on an assert.
*/
void verif_tree(tree *t);

/** \fn void delete_tree(tree *t);
* \brief Deallocate all memory used by tree.
*
* \param t Pointer to tree to delete.
*/
void delete_tree(tree *t);

/** \fn void print_tree(tree *t);
* \brief Use for debug only. Print all element in tree with function
* \c data_print.
*
* \param t Pointer to tree.
*/
void print_tree(tree *t);

/** \fn void explore_tree(tree *t, void (*treatement)(void *, void *),
* void *param);
* \brief Execute function \c treatement on every node in tree.
*
* \param t Pointer to tree.
* \param treatement Function to apply to each node.
* \param param Pointer to extra data to pass to \c treatement function.
*
* This function goes thought the entire tree and, if \c n is the pointer
* to the current node, call the function:
*
* treatement(n, param);
*
*/
void explore_tree(tree *t, void (*treatement)(void *));

void datalog_tree(tree *t, size_t data_size, FILE *f);

/** \fn explore_restrain_tree(tree *t, int (*check)(void *, void *),
* void *param,
* void *data_min, void *data_max);
* \brief Execute function \c check on every node between \c data_min and
* \c data_max.
*
* \return Accumulation of all return value of \c check function.
* \param t Pointer to tree.
* \param check Function apply on every node between \c data_min and
* \c data_max
* \param param Pointer to extra data to pass to \c check function.
* \param data_min Pointer to the minimum element.
* \param data_max Pointer to the maximum element.
*
* This function goes thought a part of tree bounded with \c data_min and
* \c data_max, and if \c n is the pointer to the current node, it calls
* the function:
*
* accu += check(n, param);
*
* The value of \c accu is returned by \c explore_restrain_tree.
*/
int explore_restrain_tree(tree *t, int (*check)(void *, void *), void *param,
                                void *data_min, void *data_max);

/** \fn int is_present(tree *t, void *d);
* \brief Function to check if a given data is present in tree.
*
* \return 1 if data is present, 0 if not.
* \param t Pointer to tree.
* \param d Pointer to data. Only field used in \c avl_data_cmp need
* to be filled in \c d.
*/
int is_present(tree *t, void *d);

/** \fn void delete_node_min(tree *t);
* \brief Delete minimum element of a tree.
*
* \param t Tree where minimum element will be deleted.
*/
void delete_node_min(tree *t);

/** \fn void delete_node(tree *t, void *data);
* \brief Delete node n of tree.
*
* \param t Pointer to tree.
* \param data Data to delete.
*/
void delete_node(tree *t, void *data);

/** \fn int get_data(tree *t, void *data, size_t data_size);
* \brief Fill information pointed by data with the data stored in the tree.
*
* \return True if value pointed by data are relevant, false if not.
*
* \param t Pointer to tree.
* \param data Data to retrieve. At the begining of the function, only
* field used in \c avl_data_cmp must be filled.
* \param data_size Size of data structure pointed by data.
*/
int get_data(tree *t, void *data, size_t data_size);

/*
 * Provides the same functionality as get_data but returns the actual
 * pointer to the signal in the tree. Use this over get_data when
 * accessing data (memory leak in get_data?)
 */
struct signal_node * get_signal(tree *t, void *data, size_t data_size);

/*
 * Provides the same functionality as get_signal but returns a message
 * node instead of a signal_node
 */
struct message_node * get_message(tree *t, void *data, size_t data_size);

#endif
