#ifndef KRIPKE_H
#define KRIPKE_H

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#define MAX_STATES 10
#define MAX_LABELS 10
typedef struct {
    char* name;         // State name
    char** labels;   // Atomic propositions
    int num_labels;     // Number of atomic propositions
    int visited;        // Used for DFS
} State;

typedef struct {
    char* from;  // Start state
    char* to;    // End state
} Transition;

typedef struct {
    State states[MAX_STATES];
    Transition transitions[MAX_STATES * 2];  // Adjacency list representation
    int num_states;
    int num_transitions;
} Kripke;

Kripke initkripke();
void parseKripke(const char *filename, Kripke* ks);
void printKripke(Kripke* ks);
void freeKripke(Kripke* ks);
#endif