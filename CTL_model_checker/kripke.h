#ifndef KRIPKE_H
#define KRIPKE_H

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#define MAX_STATES 10
#define MAX_LABELS 10

typedef struct State State;
typedef struct state_info_list{
    struct State* state;
    struct state_info_list* next;
}state_info_list;

typedef struct State{
    char* name;         // State name
    char** labels;   // Atomic propositions
    int num_labels;     // Number of atomic propositions
    int visited;        // Used for DFS
    struct state_info_list* outneighbors;
    struct state_info_list* inneighbors;
    int outdegree;
    int indegree;
} State;

typedef struct {
    char* from;  // Start state
    char* to;    // End state
} Transition;

typedef struct {
    State* states;
    Transition* transitions;  // Adjacency list representation
    int num_states;
    int num_transitions;
} Kripke;

void parseKripke(const char *filename, Kripke* ks);
void printKripke(Kripke* ks);
void freeKripke(Kripke* ks);

#endif