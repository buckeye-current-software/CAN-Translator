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
* \file avl.c
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
#ifndef LOGLEVEL
# define LOGLEVEL 0
#endif

#include "all.h"

/** Library version
*/
#define LIBAVL_MAJOR_VERSION 1
#define LIBAVL_MINOR_VERSION 0
#define LIBAVL_REVISION 0
#define LIBAVL_VERSION "1.0.0"
#define LIBAVL_VERSION_CHECK(maj, min) ( ((maj) == LIBAVL_MAJOR_VERSION)\
&& ((min) == LIBAVL_MINOR_VERSION))


/** \fn int is_present_recur(node n, void *d, int (*data_cmp) (void *, void *));
* \brief Recursive function to check if a given data is present in tree.
*
* \return 1 if data is present, 0 if not.
* \param n Node of subtree to analyze.
* \param d Pointer to data.
* \param data_cmp Function to compare two nodes.
*
* \warning If you use this function you probably make a mistake.
*/
int is_present_recur(node n, void *d, int (*data_cmp) (void *, void *))
{
    int cmp = 0;

    // Prevent analyze of a Null node
    if (n == NULL)
        return 0;

    // Compare data
    cmp = data_cmp(n->data, d);

    if (cmp == 0)
        // Node found, return true
        return 1;
    else if (cmp > 0)
        // Current node is higher than data to look for,
        // need to go to left subtree.
        return is_present_recur(n->left, d, data_cmp);
    else
        // Current node is smaller than data to look for,
        // need to go to right subtree.
        return is_present_recur(n->right, d, data_cmp);
}

/** Use for debug only. Print recursive level of inserted element */
#if LOGLEVEL > 3
static int level_insert = 0;
#endif

/** \fn int height_tree(node tree);
* \brief Give the height of tree.
*
* \return Height of tree
* \param tree Root of tree to analyze
*
* If there is no son, height of node is 1. Else, the height is maximum
* height of subtrees plus 1.
*
* \warning If you use this function you probably make a mistake.
*/
unsigned int height_tree(node n)
{
    if (n == NULL)
        return 0;

    return n->height;
}

/** \fn void adjust_tree_height(node n);
* \brief Update height field of tree.
*
* \param n Calculate new height of tree pointed by \c tree.
*
* For the height calculation rules, see \c height_tree function.
*
* \warning If you use this function you probably make a mistake.
*/
void adjust_tree_height(node n)
{
    unsigned int h1;
    unsigned int h2;

    h1 = height_tree(n->left);
    h2 = height_tree(n->right);

    if (h1 > h2)
        n->height = h1 + 1;
    else
        n->height = h2 + 1;
}

/** \fn node rotate_tree_right(node n);
* \brief Proceed right rotation to tree pointed by \c n.
*
* \return New root of right rotated tree.
* \param n Pointer to root of tree.
*
* \warning If you use this function you probably make a mistake.
*/
node rotate_tree_right(node n)
{
    node temp = n->left;
    n->left = temp->right;
    adjust_tree_height(n);
    temp->right = n;
    adjust_tree_height(temp);
    return temp;
}

/** \fn node rotate_tree_left(node n);
* \brief Proceed left rotation to tree pointed by \c n.
*
* \return New root of left rotated tree.
* \param n Pointer to root of tree.
*
* \warning If you use this function you probably make a mistake.
*/
node rotate_tree_left(node n)
{
    node temp = n->right;
    n->right = temp->left;
    adjust_tree_height(n);
    temp->left = n;
    adjust_tree_height(temp);
    return temp;
}


/** \fn node equi_left(node n);
* \brief Balance left tree.
*
* \return New root of left-balanced tree.
* \param n Pointer to root of tree.
*
* This function make rotation and update height if necessary.
*
* \warning If you use this function you probably make a mistake.
*/
node equi_left(node n)
{

    node son = n->left;

    DLOG("height tree: tree(%d) | left (%d) | right (%d) | son (%d)",
            height_tree(n),
            height_tree(n->left),
            height_tree(n->right),
            height_tree(son));
    if (height_tree(son) > height_tree(n->right) + 1) {
        if (height_tree(son->right) > height_tree(son->left)) {
            DLOG("Need rotate left");
            n->left = rotate_tree_left(n->left);
        }
        DLOG("Need rotate right");
        n = rotate_tree_right(n);
    } else {
        DLOG("No rotate");
        adjust_tree_height(n);
    }
    return n;
}

/** \fn node equi_right(node n);
* \brief Balance right tree.
*
* \return New root of right-balanced tree.
* \param n Pointer to root of tree.
*
* This function make rotation and update height if necessary.
*
* \warning If you use this function you probably make a mistake.
*/
node equi_right(node n)
{
    node son = n->right;

    if (height_tree(son) > height_tree(n->left) + 1) {
        if (height_tree(son->left) > height_tree(son->right))
            n->right = rotate_tree_right(n->right);
        n = rotate_tree_left(n);
    } else {
        adjust_tree_height(n);
    }
    return n;
}


/** \fn int delete_node_min_recur(node *n, void (*data_delete) (void *));
* \brief Recursive deletion of minimum element.
*
* \return True if element is deleted, false if not.
* \param n Root of tree where minimum element must be deleted.
* \param data_delete Fonction to delete a node in tree.
*
* \warning If you use this function you probably make a mistake.
*/
int delete_node_min_recur(node *n, void (*data_delete) (void *))
{
    node aux = NULL;
    int result;

    if (*n == NULL)
        return 0;

    if ((*n)->left == NULL) {
        // No node in left subtree, this means that the current node
        // is the minimum node stored in tree.
        aux = *n;
        *n = aux->right;
        data_delete(aux->data);
        free(aux);
        return 1;
    } else {
        // not the minimum, go deep
        result = delete_node_min_recur(&((*n)->left), data_delete);
        // balance resulting tree
        *n = equi_right(*n);
    }

    return result;
}

/** \fn node delete_node_recur(node *root, void *data,
* int (*data_cmp) (void *, void *),
* void (*data_delete) (void *));
* \brief Recursive deletion of the a node.
*
* \param root Pointer of pointer to subtree.
* \param data Data to delete. Only field used in \c avl_data_cmp
* must be filled.
* \param data_cmp Function use to compare node.
* \param data_delete Function use to delete node.
* \return True if node is deleted, false else.
*
* \warning If you use this function you probably make a mistake.
*/
int delete_node_recur(node *root, void *data,
                      int (*data_cmp) (void *, void *),
                      void (*data_delete) (void *))
{
    int cmp = 0;
    int result = 0;
    node aux = NULL;

    if (*root == NULL) {
        WLOG("Node does not exist");
        return 0;
    }

    cmp = data_cmp(data, (*root)->data);
    if (cmp == 0) {
        // Current node is the node to delete.
        if ((*root)->right == NULL) {
            // simple deletion because there is no right subtree.
            // attach the left subtree instead of the deleted node
            aux = *root;
            *root = (*root)->left;

            // release memory used in node.
            data_delete(aux->data);
            free(aux);
        } else {
            // There is a right subtree.
            // swap minimun element of right subtree and
            // the deleted data, efectively delete data
            // and re balance right subtree.
            void *d;
            node temp = (*root)->right;

            // look for the minimum element of right subtree.
            while (temp->left != NULL)
                temp = temp->left;

            // swap data
            d = (*root)->data;
            (*root)->data = temp->data;
            temp->data = d;

            // delete minimum node.
            delete_node_min_recur(&((*root)->right), data_delete);
            // rebalance subtree.
            *root = equi_left(*root);
        }
        return 1;
    } else if (cmp > 0) {
        // current node is smaller than node to delete
        // go down into right subtree.
        result = delete_node_recur(&((*root)->right), data, data_cmp, data_delete);
        // rebalance subtree.
        *root = equi_left(*root);
    } else {
        // current node is higher than node to delete
        // go down into left subtree.
        result = delete_node_recur(&((*root)->left), data, data_cmp, data_delete);
        // rebalance subtree.
        *root = equi_right(*root);
    }

    return result;
}

/** \fn int insert_elmt_recur(node *n, node add_node,
* int (*data_cmp) (void *, void *));
* \brief Recursive function too add element in tree.
*
* \return Number of element inserted.
* \param n Root of tree where element must be inserted.
* \param add_node Element to be added in tree.
* \param data_cmp Function to compare nodes.
*
* \warning If you use this function you probably make a mistake.
*/
int insert_elmt_recur(node *n, node add_node, int (*data_cmp) (void *, void *))
{
    int present = 0; // 1 means that data already present
    int cmp;

    // Here is the end of a tree. It must create new node here
    DLOG("Insert %p at level %d", add_node, level_insert);
    if (*n == NULL) {
        (*n) = add_node;
        (*n)->height = 1;
        (*n)->left = NULL;
        (*n)->right = NULL;

        return 0;
    }

    cmp = data_cmp((*n)->data, add_node->data);

    // Check if current node is the node you want to add
    if (cmp == 0)
    {
        // node already exists
        return 1;
    }
    if (cmp > 0) {
        // Current node is higher that node you want to add
        // Insert it on left subtree.
        DLOG("Down into left level %d", ++level_insert);
        present = insert_elmt_recur(&(*n)->left, add_node, data_cmp);
        DLOG("Out of level %d", level_insert--);

        if (!present) {
            // node was really inserted, need to re-balance tree
            *n = equi_left(*n);
            return 0;
        } else
        {
            // node not inserted in subtree
        	//printf("Compared returned 0. Don't add node..? \n");
            return 1;
        }
    } else {
        // Current node is smaller that node you want to add
        // Insert it on right subtree.
        DLOG("Down into right level %d", ++level_insert);
        present = insert_elmt_recur(&(*n)->right, add_node, data_cmp);
        DLOG("Out of level %d", level_insert--);

        if (!present) {
            // node was really inserted, need to re-balance tree
            *n = equi_right(*n);
            return 0;
        } else
        {
            // node not inserted in subtree
        	//printf("Compare returned 0. Don't add node \n");
            return 1;
        }
    }

}

/** \fn void verif_avl(node n, int tree_min, int tree_max,
* void *data_min, void *data_max,
* int (*data_cmp) (void *, void *));
* \brief Recursive deffensive function to check if tree is an AVL tree.
*
* \param n Pointer to root of tree.
* \param tree_min Boolean must be true if \c tree is the minimum node.
* \param tree_max Boolean must be true if \c tree is the maximum node.
* \param data_min Pointer to the minimum element of sub-tree.
* \param data_max Pointer to the maximum element of sub-tree.
* \param data_cmp Fonction to compare nodes.
*
* \warning If you use this function you probably make a mistake.
*/
void verif_avl(node n,
        int tree_min,
        int tree_max,
        void *data_min,
        void *data_max,
        int (*data_cmp) (void *, void *))
{
    unsigned hg;
    unsigned hd;

    // Check order of data.
    if (tree_min && data_cmp(n->data, data_min) < 0) {
        DLOG("Tree->data < data_min");
        exit(-1);
    }
    if (tree_max && data_cmp(n->data, data_max) > 0) {
        DLOG("Tree->data > data_min");
        exit(-2);
    }

    // Check avl left subtree.
    if (n->left != NULL) {
        verif_avl(n->left,
                tree_min,
                1,
                data_min,
                n->data,
                data_cmp);
        hg = n->left->height;
    } else {
        hg = 0;
    }

    // Check avl right subtree.
    if (n->right != NULL) {
        verif_avl(n->right,
                1,
                tree_max,
                n->data,
                data_max,
                data_cmp);
        hd = n->right->height;
    } else {
        hd = 0;
    }


    // Check height consistency of each subtree
    if (hg <= hd) {
        if (!(hd + 1 == n->height && hg + 2 >= n->height)) {
            DLOG("(hg<hd) Error in tree height: hd %u | hg %u | tree->height %u",
                    hd, hg, n->height);
            exit(-3);
        }
    } else {
        if (!(hg + 1 == n->height && hd + 2 >= n->height)) {
            DLOG("(hg>hd) Error in tree height: hd %u | hg %u | tree->height %u",
                    hd, hg, n->height);
            exit(-4);
        }
    }
}

/** \fn void delete_tree_recur(node n, void (*data_delete) (void *));
* \brief Recursively delete all node in tree.
*
* \param n Root node of tree to delete.
* \param data_delete Function use to delete a node.
*
* \warning If you use this function you probably make a mistake.
*/
void delete_tree_recur(node n, void (*data_delete) (void *))
{
    if (n == NULL)
        return;

    if (n->left != NULL)
        delete_tree_recur(n->left, data_delete);
    if (n->right != NULL)
        delete_tree_recur(n->right, data_delete);

    data_delete(n->data);
    n->data = NULL;
    n->left = NULL;
    n->right = NULL;
    free(n);
    n = NULL;
}

/** \fn void print_tree_recur(node t, void (*data_print) (void *));
* \brief Recursive function to print tree. Use for debug.
*
* \param t Pointer to root of tree.
* \param data_print Function to display a node.
*
* \warning If you use this function you probably make a mistake.
*/
void print_tree_recur(node t, void (*data_print) (void *))
{
    if (t == NULL)
        return;

    // recursively print left subtree.
    print_tree_recur(t->left, data_print);
    {
        // print current node with debug information.
        unsigned i = 0;
        for (i = 0; i < t->height; i++)
            printf(" ");
        struct message_node* node = (struct message_node*)(t->data);
        printf("[%d|%p]\n", node->key, t);
        struct list_node * l_node = node->list->head;
        while (l_node != NULL)
        {
        	printf("Signal: %s, OkStart: %d, OkEnd: %d, WarnStart: %d, WarnEnd: %d, Unit: %s \n", l_node->signal->id, l_node->signal->okStart,
        			l_node->signal->okEnd, l_node->signal->warnStart, l_node->signal->warnEnd, l_node->signal->unit);
        	l_node = l_node->next;
        }
        fflush(stdout);
        data_print(t->data);
        printf("\n");
    }
    // recursively print right subtree.
    print_tree_recur(t->right, data_print);
}

/** \fn void explore_tree_recur(node t, void (*treatement)(void *, void *),
* void *param);
* \brief Recursive exploration of tree.
*
* \param t Pointer to subtree.
* \param treatement Function apply to each node of tree.
* \param param Pointer to data to pass to \c treatement function.
*
* \warning If you use this function you probably make a mistake.
*/
void explore_tree_recur(node t, void (*treatement)(void *))
{
    if (t == NULL)
        return;

    // recursively treat left subtree.
    explore_tree_recur(t->left, treatement);
    // treat current node.
    (*treatement)(t->data);
    // recursively treat right subtree.
    explore_tree_recur(t->right, treatement);
}



/** \fn int explore_restrain_tree_recur(node t, int (*check)(void *, void *),
* void *param, void *data_min,
* void *data_max,
* int (*data_cmp) (void *, void *));
* \brief Recursive and restrain exploration of tree.
*
* \return Accumulation of return value of \c check function.
* \param t Pointer to root of tree.
* \param check Function apply to each node of tree between \c data_min and
* \c data_max.
* \param param Pointer to data to pass to \c check function
* \param data_min All treated node are greater than \c data_min
* \param data_max All treated node are smaller than \c data_max
* \param data_cmp Function to compare nodes.
*
* \warning If you use this function you probably make a mistake.
*/
int explore_restrain_tree_recur(node t, int (*check)(void *, void *),
        void *param,
        void *data_min, void *data_max,
        int (*data_cmp) (void *, void *))
{
    if (t == NULL)
        return 0;

    if (data_cmp(t->data, data_max) > 0)
        // current data is not in the asked range.
        return explore_restrain_tree_recur(t->left, check, param,
                                            data_min, data_max,
                                            data_cmp);
    else if (data_cmp(t->data, data_min) < 0)
        // current data is not in the asked range.
        return explore_restrain_tree_recur(t->right, check, param,
                                            data_min, data_max,
                                            data_cmp);
    else {
        // current data is in the range.
        int accu = 0;
        // treat recursively left subtree.
        accu += explore_restrain_tree_recur(t->left, check, param,
                                            data_min, data_max,
                                            data_cmp);
        // treat current node.
        accu += check(t->data, param);
        // treat recursively right subtree.
        accu += explore_restrain_tree_recur(t->right, check, param,
                                            data_min, data_max,
                                            data_cmp);
        return accu;
    }
}


/** \fn int get_data_recur(node n, void *data, size_t data_size,
* int (*data_cmp) (void *, void *))
* \brief Recursively get of a single data.
*
* \param n Root of tree to analyze.
* \param data Pointer to the asked data. At the begining of the function,
* only field used in \c avl_data_cmp must be filled, at the end (and if
* data exist in tree), all filled will be filled.
* \param data_size Size of the data structure (need to copy data).
* \param data_cmp Function to compare nodes.
* \return 1 if data was found, 0 if not.
*
* \warning If you use this function, you probably make a mistake.
*/
int get_data_recur(node n, void *data, size_t data_size, int (*data_cmp) (void *, void *))
{
    int cmp = 0;

    if (n == NULL)
        return 0;

    cmp = data_cmp(n->data, data);
    if (cmp == 0) {
        // Current node is the good node, copy it.
        memcpy(data, n->data, data_size); // The problem line...
        return 1;
    } else if (cmp > 0) {
        // Need to go deep in the left subtree.
        return get_data_recur(n->left, data, data_size, data_cmp);
    } else {
        // Need to go deep in the right subtree.
        return get_data_recur(n->right, data, data_size, data_cmp);
    }

}

void * get_signal_recur(node n, void *data, size_t data_size, int (*data_cmp) (void *, void *)) // Doesn't actually use data_size. Remove eventually
{
	int cmp = 0;

	if (n == NULL)
	{
		return NULL;
	}

	cmp = data_cmp(n->data, data);
	if (cmp == 0) {
		return n->data;
	} else if (cmp > 0) {
	// Need to go deep in the left subtree.
		return get_signal_recur(n->left, data, data_size, data_cmp);
	} else {
	// Need to go deep in the right subtree.
		return get_signal_recur(n->right, data, data_size, data_cmp);
	}
}

/* ************************************************************************* *\
|* EXTERNAL FUNCTION *|
\* ************************************************************************* */

/** \fn tree *init_dictionnary(int (*data_cmp)(void *, void *),
* void (*data_print)(void *),
* void (*data_delete)(void *),
* void *(*data_copy)(void *));
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
                       void (*data_copy)(void *, void *))
{
    // New tree allocation
    tree *t = malloc(sizeof(tree));

    // Initialized field
    t->count = 0;
    t->root = NULL;
    t->data_cmp = data_cmp;
    t->data_print = data_print;
    t->data_delete = data_delete;
    t->data_copy = data_copy;

    return t;
}

/* \fn int insert_elmt(tree *t, void *data, size_t datasize);
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
unsigned int insert_elmt(tree *t, void *data, size_t datasize)
{

    node to_add = NULL;
    int present = 0;

    // check if data is already present
    if (is_present(t, data))
    {
    	//printf("Data is already present");
        return t->count;
    }
    // Allocate memory for the new data and copy data.
    to_add = calloc(sizeof(struct _node),1);
    if(to_add == NULL)
    {
    	printf("TO ADD IS NULL\n");
    }
    to_add->data = calloc(datasize,1);
    //memcpy(to_add->data, data, datasize);
    if(to_add->data == NULL)
    {
    	printf("To_add->data is NULL\n");
    }
    //to_add->data = data;
    t->data_copy(data, to_add->data);
    to_add->height = 0;
    to_add->left = to_add->right = NULL;

    // recursively insert data in tree.
    present = insert_elmt_recur(&(t->root), to_add, t->data_cmp);

    // increment counter of element if so.
    if (!present) {
        DLOG("New data was added.\n");
        //printf("New data was added.\n");
        return ++t->count;
    } else {
        DLOG("Data was updated.\n");
        //printf("Data was updated.\n");
        return t->count;
    }
}


/* \fn void verif_tree(tree *t);
* \brief Deffensive check if tree is a real AVL tree.
*
* \param t Pointer to tree.
*
* If tree is not an AVL tree, this function end on an assert.
*/
void verif_tree(tree *t)
{

    if (t == NULL)
        return;
    if (t->root == NULL)
        return;

    // recursively check of avl tree.
    verif_avl(t->root, 0, 0, t->root->data, t->root->data, t->data_cmp);
}

/* \fn void delete_tree(tree *t);
* \brief Deallocate all memory used by tree.
*
* \param t Pointer to tree to delete.
*/
void delete_tree(tree *t)
{
    if (t == NULL)
        return;

    delete_tree_recur(t->root, t->data_delete);
    free(t);
    t = NULL;
}

/* \fn void print_tree(tree *t);
* \brief Use for debug only. Print all element in tree with function
* \c data_print.
*
* \param t Pointer to tree.
*/
void print_tree(tree *t)
{
    if (t == NULL)
        return;
    if (t->root == NULL)
        return;

    // recursively print the tree.
    print_tree_recur(t->root, t->data_print);
}

/* \fn void explore_tree(tree *t, void (*treatement)(void *, void *),
* void *param);
* \brief Execute function \c treatement on every node in tree.
*
* \param t Pointer to tree.
* \param treatement Function to apply to each node.
* \param param Pointer to extra data to pass to \c treatement function.
*
* This function goes thought the entire tree and, if \c n is the pointer
* to the current node, call the function:
* treatement(n, param);
*
*/
void explore_tree(tree *t, void (*treatement)(void *))
{
    if (t == NULL)
        return;
    if (t->root == NULL)
        return;

    // recursively explore the whole tree.
    explore_tree_recur(t->root, treatement);
}

/* \fn explore_restrain_tree(tree *t, int (*check)(void *, void *),
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
        void *data_min, void *data_max)
{
    if (t == NULL)
        return 0;
    if (t->root == NULL)
        return 0;

    // recursively explore part of tree.
    return explore_restrain_tree_recur(t->root, check, param,
                                       data_min, data_max,
                                       t->data_cmp);
}

/* \fn int is_present(tree *t, void *d);
* \brief Function to check if a given data is present in tree.
*
* \return 1 if data is present, 0 if not.
* \param t Pointer to tree.
* \param d Pointer to data. Only field used in \c data_cmp need
* to be filled in \c d.
*/
int is_present(tree *t, void *d)
{
    if (t == NULL)
        return 0;

    // Return result of a recursive exploration
    return is_present_recur(t->root, d, t->data_cmp);
}

/* \fn void delete_node_min(tree *t);
* \brief Delete minimum element of a tree.
*
* \param t Tree where minimum element will be deleted.
*/
void delete_node_min(tree *t)
{
    if (t == NULL || t->root == NULL)
        return;

    // go recursively in tree to delete minimum node
    if (delete_node_min_recur(&(t->root), t->data_delete))
        t->count--;
}

/* \fn void delete_node(tree *t, void *data);
* \brief Delete node n of tree.
*
* \param t Pointer to tree.
* \param data Data to delete.
*/
void delete_node(tree *t, void *data)
{
    if (t == NULL)
        return;
    if (t->root == NULL)
        return;
    // explore tree recursively to delete node
    if (delete_node_recur(&(t->root), data, t->data_cmp, t->data_delete))
        t->count--;
}

/* \fn int get_data(tree *t, void *data, size_t data_size);
* \brief Fill information pointed by data with the data stored in the tree.
*
* \return True if value pointed by data are relevant, false if not.
*
* \param t Pointer to tree.
* \param data Data to retrieve. At the begining of the function, only
* field used in \c avl_data_cmp must be filled.
* \param data_size Size of data structure pointed by data.
*/
int get_data(tree *t, void *data, size_t data_size)
{
    if (t == NULL)
        return 0;
    if (t->root == NULL)
        return 0;

    return get_data_recur(t->root, data, data_size, t->data_cmp);
}

/*
 * Provides the same functionality as get_data but returns the actual
 * pointer to the signal in the tree. Use this over get_data when
 * accessing data (memory leak in get_data?)
 */
struct signal_node * get_signal(tree *t, void *data, size_t data_size)
{
	if (t == NULL)
	{
		return NULL;
	}
	if (t->root == NULL)
	{
	    return NULL;
	}
	return get_signal_recur(t->root, data, data_size, t->data_cmp);
}

/*
 * Provides the same functionality as get_signal but returns a message
 * node instead of a signal_node
 */
struct message_node * get_message(tree *t, void *data, size_t data_size)
{
	if (t == NULL)
	{
		return NULL;
	}
	if (t->root == NULL)
	{
	    return NULL;
	}
	return get_signal_recur(t->root, data, data_size, t->data_cmp);
}
