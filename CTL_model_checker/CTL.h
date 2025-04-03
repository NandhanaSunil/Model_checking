#ifndef CTL_H
#define CTL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kripke.h"

typedef struct satisfying_states {
    struct State* state;
    struct satisfying_states *next;
}satisfying_states;

typedef struct Node {
    char *type;
    struct Node *left;
    struct Node *right;
    struct satisfying_states *sat_states;
    int sat_states_count;
} Node;

typedef struct queue{
    struct State* state;
    struct queue* next;
}queue;

typedef struct scc{
    struct State* state;
    struct scc* next;
}scc;

typedef struct scc_list{
    struct scc* scc;
    struct scc_list* next;
}scc_list;

void printformula(Node *node);
void print_formula(Node* node);
void freetree(Node *node);
Node *new_node(char *type, Node *left, Node *right);
void print_tree(Node *node, int depth, int *num_nodes);
void printtree(Node *node);
satisfying_states* add_state(satisfying_states* head, State* state);
void print_states(Node* node);
void process_prop(Node* node, Kripke* ks);

#endif