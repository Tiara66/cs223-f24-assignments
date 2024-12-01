#include "tree.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>

/**
 * A binary search tree implementation with functions for insertion, 
 * searching, and printing.
 *
 * This program provides utility methods to insert nodes, find nodes, 
 * and print the tree in both an indented format and a sorted format.
 *
 * @author: Tianyun Song
 * @version: December 1, 2024
 */

/**
 * Finds a node with the specified name in the binary search tree.
 * 
 * @param name The name to search for
 * @param root The root of the binary search tree
 * @return The node containing the name, or NULL if not found
 */
struct tree_node* find(const char* name, struct tree_node* root)
{
  if (root == NULL || strcmp(root->data.name, name) == 0)
        return root; // Return the node if found or NULL if not found

    if (strcmp(name, root->data.name) < 0)
        return find(name, root->left); // Search in the left subtree

    return find(name, root->right); // Search in the right subtree
}

/**
 * Inserts a new node with the specified name into the binary search tree.
 * 
 * @param name The name to insert
 * @param root The root of the binary search tree
 * @return The root of the tree after insertion
 */
struct tree_node* insert(const char* name, struct tree_node* root)
{
  if (root == NULL) {
        // Allocate memory for a new node
        struct tree_node* new_node = malloc(sizeof(struct tree_node));
        if (new_node == NULL) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        strcpy(new_node->data.name, name); // Set the name
        new_node->left = new_node->right = NULL; // Initialize children
        return new_node;
    }

    if (strcmp(name, root->data.name) < 0)
        root->left = insert(name, root->left); // Insert in the left subtree
    else if (strcmp(name, root->data.name) > 0)
        root->right = insert(name, root->right); // Insert in the right subtree

    return root; // Return the root
}

/**
 * Clears the entire binary search tree, freeing all allocated memory.
 * 
 * @param root The root of the binary search tree
 */
void clear(struct tree_node* root)
{
  if (root == NULL)
        return;

    clear(root->left); // Clear left subtree
    clear(root->right); // Clear right subtree
    free(root); // Free current node
}

/**
 * Helper function to print the tree with indentation.
 * 
 * @param root The root of the binary search tree
 * @param depth The depth of the current node
 * @param prefix The prefix to print before the node name
 */
void printIndented(struct tree_node* root, int depth, char *prefix) {
    if (root == NULL)
        return;

    // print current node
    for (int i = 0; i < depth; i++) {
        printf(" "); // indentation
    }
    printf("%s%s\n", prefix, root->data.name);

    // print left tree
    if (root->left) {
        printIndented(root->left, depth + 1, "l:");
    }

    // print right tree
    if (root->right) {
        printIndented(root->right, depth + 1, "r:");
    }
}

/**
 * Wrapper function to print the tree in an indented format.
 * 
 * @param root The root of the binary search tree
 */
void print(struct tree_node* root) {
    printIndented(root, 0, ""); // Start at depth 0
}

/**
 * Prints the binary search tree in sorted order.
 * 
 * @param root The root of the binary search tree
 */
void printSorted(struct tree_node* root)
{
  if (root == NULL)
        return;

    printSorted(root->left); // Traverse left subtree
    printf("%s\n", root->data.name); // Print the current node
    printSorted(root->right); // Traverse right subtree
}