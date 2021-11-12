/*
Name: BDZ3 Trees, variant 29
Author: Vadim Olennikov
*/
#define COUNT ( 2 ) /*maximum number of children per node */
#define LENGTH ( 100 )/* max name's length */
#define TEMPL_SIZE ( 20 )/* characters to read string */
#define KEY ( 8 ) /* max key length */
#define NOT_LEAF_CODE ( -4 )/* if node is not leaf */
#define KEY_ERR ( -2 )/* if key is wrong */
#define MEMORY_ERR ( -1 )
#define FILE_OPEN_ERR ( -5 )
#define MAX_LEVEL ( 100 )/*max tree level*/
#define OPERATIONS ( 100 )/* The number of operations selected by the user */
#define NEG_DIGIT ( -168565 )

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node node;
struct _node
{
    char name[ LENGTH ];
    node *child[ COUNT ];
};

int add( node **root, char *key, char *name);/* add new element to tree */
int load(node ** root, char * filename);/* load tree from file */
node *create_leaf( char *name );/* create new leaf of tree */
void show( node *root );/* show tree */
void show_subtree( node *root, int level );/* help function for show */
void drop( node **cur );/*delete tree */
int load_exept_root( FILE *file, node **root );
int isleaf( node *leaf );
int delete_leaf( node **root, char *key );
int highlight_subtree( node **root, char *key );
int check_key( node *cur, char *key );
int branch_val( node *root );
int find_min_level( node *root );
void show_all_branches_in_level( node *root, int level_leaf );
void show_branches_in_level( node *root, node *cur, int level, int leaf_level, int *key );
void show_branch( node *root, int leaf_level, int *key );
void show_all_branches_min_nodes( node *root );
void menu_main( node **root, int *operations, int position );
void show_menu_main();
void menu_operations( node **root, int *operations, int position );
void show_selected_operations( int *operations );
void show_menu_operation();
void putNULL( int *operations );

int main()
{
    int res;
    int operations[ OPERATIONS ];
    node *root = 0;
    char filename[] = "data.txt";

    putNULL( operations );

    res = load( &root, filename );
    if( res == 0 )
        menu_main( &root, operations, 0 );//start position in arr is 0
    else
    {
        printf( "File open error");
        exit FILE_OPEN_ERR;
    }
    return 0;
}

void putNULL( int *operations )
{
    int i;

    for( i = 0; i < OPERATIONS; i++ )
        operations[ i ] = 0;
}

int add( node **cur, char *key, char *name)/* add new element to tree */
{
    int i, number;
    node **next;/* next level pointer */

    /* create new leaf */
    node * element;
    element = create_leaf( name );
    if( element == NULL )/* if a memory allocation error occurred */
        return MEMORY_ERR;

    for( i = 0; key[ i ]; i++ )
    {
        number = key[ i ] - '0';/* number of child; convert char to int */
        next = &( ( *cur )->child[ number ] );/* next level pointer */

        if( number >= COUNT || number < 0 )//if key is wrong
            return KEY_ERR;;
        if( *next == NULL && key[ i + 1 ] != '\0' )/* if parent of our element doesn't exist */
            return KEY_ERR;

        cur = next;/* go to child number key[ i ] */
    }
    ( *cur ) = element;
    return 0;/* successful execution */
}

int load (node ** root, char * filename)/* load tree from file */
{
    int res_load;/* result of scanning subtree*/
    FILE * file;
    char name[ LENGTH ];
    char templateSTR[ TEMPL_SIZE ];/* to read a given number of characters */
    sprintf( templateSTR, "%%%is", LENGTH );

    file = fopen( filename, "r" );
    if( file == NULL )
        return -3;/* file open error */

    if( fscanf( file, templateSTR, name ) > 0 )/* put data in root */
        *root = create_leaf( name );

    res_load = load_exept_root( file, root );

    fclose( file );
    return res_load;/* result of scanning */
}

int load_exept_root( FILE *file, node **root )
{
    int res_add, res_scan = 1;/* result of scanning */
    char key[ KEY ], name[ LENGTH ];
    char templateSTR[ TEMPL_SIZE ];/* to read a given number of characters */
    sprintf( templateSTR, "%%%is", LENGTH );

    for( ; res_scan > 0; )
    {
        res_scan = fscanf( file, templateSTR, key );
        res_scan = fscanf( file, templateSTR, name );

        if( res_scan > 0 )/* if name is scanned successful */
            res_add = add( root, key, name );//root is pointer to pointer and it doesn't change here
        if( res_add != 0  )//not success add
            return res_add;
    }
    return 0;
}

node *create_leaf ( char *name )/* create new leaf of tree */
{
    int i;

    node *element;
    element =( node* ) malloc( sizeof( node ) );
    if( element == NULL )/* if a memory allocation error occurred */
        return NULL;

    strcpy( element->name, name );
    for( i = 0; i < COUNT; i++ )
        element->child[ i ] = NULL;  /* because element doesn't have children */

    return element;
}

void show( node *root )
{
    int i;

    if( root == NULL )
        return;/* tree is empty  */

    printf( "-> %s\n", root->name );

    for( i = 0; i < COUNT; i++ )
        show_subtree( root->child[ i ], 1 );/* next level is 1 */
}

//-------------------------------------------------------------------
/* Example of result:
-> root
   |-> 1
   |   |-> 4
   |   |   |-> 5
   |   |   |-> 6
   |   |-> 7
   |-> 2
   |-> 3
*/
void show_subtree( node *root, int level )/* help function for tree, for formatted output */
{
    int i;

    if( root == NULL )
        return;/* subtree is empty, exit from recursing  */

    for( i = 0; i < level; i++ )/* print | to display the level */
        printf( "%4c", '|');

    printf( "-> %s\n", root->name );

    for( i = 0; i < COUNT; i++ )
        show_subtree( root->child[ i ], level + 1 );/* next recursion level */

}

void drop( node **cur )
{
    int i;

    if( *cur == NULL )
        return;/* exit from recursion */

    for( i = 0; i < COUNT; i++ )
        drop( &( ( *cur )->child[ i ] ) );

    free( *cur );
    *cur = 0;
}

int delete_leaf( node **cur, char *key )
{
    int i, number, res;
    node **parent, **child;

    for( i = 0; key[ i ]; i++ )//go to element number key
    {
        parent = cur;
        number = key[ i ] - '0';/* number of child; convert char to int */
        child = &( ( *cur )->child[ number ] );/* next level pointer */

        if( number >= COUNT || number < 0 )//if key is wrong
            return KEY_ERR;
        if( *child == NULL && key[ i + 1 ] != '\0' )/* if parent of our element doesn't exist */
            return KEY_ERR;
        cur = child;
    }

    res = isleaf( *child );
    if( res )//if key is not for leaf
        return res;

    free( *child );//delete leaf
    ( *parent )->child[ number ] = 0;

    return 0;
}

/*--------------------------------------------------
Checks if node is leaf (hasn't children)
Returns NOT_LEAF_CODE if node is note leaf
Returns 0 if success

leaf - pointer to node
*/
int isleaf( node *leaf )
{
    int i;
    for( i = 0; i < COUNT; i++ )
    {
        if( leaf->child[ i ] != NULL )
            return NOT_LEAF_CODE;
    }

    return 0;
}

/*
Select subtree from tree (delete all other items)
Returns KEY_ERR if node with code doesn't exist
Returns 0 if success

root - pointer to pointer to tree root
( we have to change pointer to tree )
*/
int highlight_subtree( node **root, char *key )
{
    int i, step, number;
    node *parent;

    /*we have no right to make a mistake */
    if( check_key( *root, key ) )
        return KEY_ERR;

    for( i = 0; key[ i ]; i++ )
    {
        number = key[ i ] - '0';/* number of child we need; convert char to int */
        for( step = 0; step < COUNT; step++ )
            if( step != number )
                drop( &( ( *root )->child[ step ] ) );//delete brothers of element

        parent = *root;
        *root = ( *root )->child[ number ];//go to next level
        free( parent );//delete parent
    }

    return 0;
}

/*--------------------------------------------------------------------
Checks the correctness of the key
If the node with the given number does not exist,
returns KEY_ERR

root - pointer to tree root, main node
key - code of node in tree
*/
int check_key( node *root, char *key )
{
    int i, number;
    node *next;

    for( i = 0; key[ i ]; i++ )
    {
        number = key[ i ] - '0';/* number of child; convert char to int */
        next = root->child[ number ];/* next level pointer */

        if( number >= COUNT || number < 0 )//if key is wrong
            return KEY_ERR;
        if( next == NULL && key[ i + 1 ] != '\0')/* if parent of our element doesn't exist */
            return KEY_ERR;

        root = next;/* go to child number key[ i ] */
    }

    return 0;
}

/*----------------------------------------------------------
Return the number of branches in tree
the number of branches is equal to the number of leaves

root - pointer to tree root
*/
int branch_val( node *root )
{
    int i, res = 0;
    node *subtree;

    /* the number of branches is equal to the number of leaves */
    if( isleaf( root ) == 0 )
        return 1;//exit from recursion

    for( i = 0; i < COUNT; i++ )
    {
        subtree = root->child[ i ];//pointers to subtrees

        if( subtree )//if pointer is not NULL
            res += branch_val( subtree );//call next recursion level
    }

    return res;
}

/*---------------------------------------------------------------
Shows all branches with a minimum number of nodes

root - pointer to tree root
*/
void show_all_branches_min_nodes( node *root )
{
    int min_level;

    min_level = find_min_level( root );
    show_all_branches_in_level( root, min_level );
}

/*----------------------------------------------
Finds the minimum branch length in a tree

root - pointer to tree root
*/
int find_min_level( node *root )
{
    node *subtree;
    int min = MAX_LEVEL;
    int i, deep = MAX_LEVEL;


    if( isleaf( root ) == 0 )
        return 0;/* exit from recursion */

    for( i = 0; i < COUNT; i++ )
    {
        subtree = root->child[ i ];//pointer to child

        if( subtree != NULL )//if child exists
        {
            deep = find_min_level( subtree );/* subtree depth */
            if( deep < min )
                min = deep;
        }
    }

    return min + 1;
}

/*-----------------------------------------------------
Displays all branches with leaves at the leaf_level

root - pointer to tree root
leaf_level - the level at which the leaves of
the tree should be; branch length
*/
void show_all_branches_in_level( node *root, int leaf_level )
{
    int i;
    int key[ leaf_level ];
    node *cur;

    if( leaf_level == 0 )//if tree is only root
    {
        printf( "Tree is empty, root: %s", root->name );
        return;
    }

    for( i = 0; i < COUNT; i++ )
    {
        cur = root->child[ i ];
        key[ 0 ] = i;//code of first node
        show_branches_in_level( root, cur, 1, leaf_level, key );//because next level is 1
    }
}

/*-------------------------------------------------------------
Recursive function shows all branches with leaf on leaf_level

root - pointer to tree root
cur - pointer to node
level - level of node with pointer cur
leaf_level - the level at which the leaves of the tree should be
key - code of node with pointer cur
*/
void show_branches_in_level( node *root, node *cur,
                            int level, int leaf_level,
                            int *key )
{
    int i;
    if( level == leaf_level )
    {
        if( isleaf( cur ) == 0 )//if elem from leaf_level is leaf
            show_branch( root, leaf_level, key );

        return;//exit from recursion
    }

    for( i = 0; i < COUNT; i++ )//next recursion level
    {
        key[ level ] = i;
        if( cur->child[ i ] )//if child exists
            show_branches_in_level( root, cur->child[ i ],
                                   level + 1, leaf_level,
                                   key );
    }
}

/*------------------------------------------------------------------
The function displays the branch on the screen
along with the data in the nodes and their code

root - pointer to tree root
leaf_level - branch length, leaf level
key - pointer to arr with code of leaf on branch
*/
void show_branch( node *root, int leaf_level, int *key )
{
    int i, step, number;
    node *cur = root;

    printf( "%s / ", root->name );//show root name
    for( i = 0; i < leaf_level; i++ )
    {
        number = key[ i ];
        cur = cur->child[ number ];
        printf( "(" );
        for( step = 0; step <= i; step++ )
            printf( "%i", key[ step ] );//show element's code

        printf( ") %s", cur->name );//show data from node

        if( i != leaf_level - 1 )//formatted output,
            printf( " / " );//not required for last element
    }

    printf( "\n" );
}

/*--------------------------------------------------------
Shows transaction information
processes the operation code selected by the user

root - pointer to pointer to tree root( we can change pointer to tree )
operations - pointer to int arr with operations data
position - number of next filling position in operations array
*/
void menu_main( node **root, int *operations, int position )
{
    int item;
    show_menu_main();
    scanf( "%i", &item );
    printf( "\n" );

    operations[ position++ ] = item;
    switch( item )
    {
        case 1 :
            show( *root );
            break;
        case 2 :/*Show operation menu*/
            menu_operations( root, operations, position );
            break;
        case 3 :/*Exit the program*/
            drop( root );
            show_selected_operations( operations );//show list with selected menu items
            exit( 0 );
        default :
            operations[ --position ] = 0;//deselection
            printf( "Invalid operation, try again!" );
    }
    printf( "\n" );
    menu_main( root, operations, position );
}

/*------------------------------------------------------
Function shows main program menu
*/
void show_menu_main()
{
    printf( "Menu:\n" );
    printf( "1 - Show tree\n" );
    printf( "2 - Show operation menu\n" );
    printf( "3 - Exit the program\n" );
    printf( "Operation: " );
}

/*--------------------------------------------------------
Shows transaction information
processes the operation selected by the user

root - pointer to pointer to tree root( we can change pointer to tree )
operations - pointer to int arr with operations data
position - number of next filling position in operations array
*/
void menu_operations( node **root, int *operations, int position )
{
    int item;/* selected code */
    char key[ KEY ];
    show_menu_operation();
    scanf( "%i", &item );
    printf( "\n" );

    operations[ position++ ] = item;/*fill operation arr */
    switch( item )
    {
        case 1 :/*Show tree */
            show( *root );
            break;
        case 2 :/*Delete leaf */
            printf( "Enter key of leaf: " );
            scanf( "%s", key );
            delete_leaf( root, key );
            break;
        case 3 :/*Select subtree from tree (delete all other items) */
            printf( "To cancel, enter -1. Enter key of node: " );
            scanf( "%s", key );/* subtree root key */
            highlight_subtree( root, key );
            break;
        case 4 :/*Count the number of branches */
            printf( "Number of branches = %i\n", branch_val( *root ) );
            break;
        case 5 :/* Print branches with a minimum number of nodes */
            show_all_branches_min_nodes( *root );
            break;
        case 6 :/* Back to the main menu */
            if( operations[ position - 2 ] == 2 )/* delete operation from history */
            {
                operations[ --position ] = 0;
                operations[ --position ] = 0;
            }
            menu_main( root, operations, position );
        default :
            if( operations[ position] )
            operations[ --position ] = 0;/* deselection */
            printf( "Invalid operation, try again!" );
    }
    printf( "\n" );
    menu_operations( root, operations, position );
}

/*----------------------------------------------
Print on display user selected operations
Example: 1 1 2 3 3 5 3

operations - pointer to int arr with operations data
*/
void show_selected_operations( int *operations )
{
    int i;
    printf("Selected operations: " );

    for( i = 0; operations[ i ]; i++ )
        printf( "%i ", operations[ i ] );

    printf("\n" );
}

/*------------------------------------------------------
Function shows information about operations in program
*/
void show_menu_operation()
{
    printf( "Operations menu:\n" );
    printf( "1 - Show tree\n" );
    printf( "2 - Delete leaf\n" );
    printf( "3 - Select subtree (delete all other items)\n" );
    printf( "4 - Count the number of branches\n" );
    printf( "5 - Print branches with a minimum number of nodes\n" );
    printf( "6 - Back to the main menu\n" );
    printf( "Selected operation: " );
}
