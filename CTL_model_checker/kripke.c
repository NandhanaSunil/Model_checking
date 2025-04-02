#include "kripke.h"

void parseKripke(const char *filename, Kripke* ks){
    // printf("Parsing Kripke structure from %s\n", filename);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);
    // printf("here\n");

    char *json = malloc(fsize + 1);
    fread(json, 1, fsize, fp);
    fclose(fp);

    json[fsize] = '\0';
    cJSON *root = cJSON_Parse(json);
    if (root == NULL) {
        fprintf(stderr, "Error: Could not parse JSON file %s\n", filename);
        exit(1);
    }


    cJSON *states = cJSON_GetObjectItem(root, "states");
    cJSON *transitions = cJSON_GetObjectItem(root, "transitions");
    cJSON *labels = cJSON_GetObjectItem(root, "labels");
    // parse to get states
    ks->num_states = cJSON_GetArraySize(states);
    ks->states = malloc(ks->num_states * sizeof(State));
    int i = 0;
    cJSON *state;
    cJSON_ArrayForEach(state, states) {
        ks->states[i].name = strdup(state->valuestring);  // Allocate memory for state name
        ks->states[i].labels = NULL;
        ks->states[i].num_labels = 0;
        ks->states[i].visited = 0;
        ks->states[i].outneighbors = NULL;
        ks->states[i].inneighbors = NULL;
        ks->states[i].outdegree = 0;
        ks->states[i].indegree = 0;
        i++;
    }

    // parse to get transitions
     // Count number of transitions
     ks->num_transitions = 0;
     cJSON *from;
     cJSON_ArrayForEach(from, transitions) {
         ks->num_transitions += cJSON_GetArraySize(from);
     }
     ks->transitions = malloc(ks->num_transitions * sizeof(Transition));
 
     // Parse transitions
     //update indegree and outdegree and outneighbors and inneighbors of each state
     i = 0;
     cJSON_ArrayForEach(from, transitions) {
         cJSON *to;
         cJSON_ArrayForEach(to, from) {
             ks->transitions[i].from = strdup(from->string);
             for (int j = 0; j < ks->num_states; j++) {
                 if (strcmp(ks->states[j].name, from->string) == 0) {
                     ks->states[j].outdegree++;
                     state_info_list* temp = ks->states[j].outneighbors;
                     state_info_list* new_node = (state_info_list*)malloc(sizeof(state_info_list));
                     for (int i=0; i < ks->num_states; i++){
                         if (strcmp(ks->states[i].name, to->valuestring)==0){
                             new_node->state = &ks->states[i];
                             new_node->next = temp;
                             ks->states[j].outneighbors = new_node;
                             ks->states[i].indegree++;
                             state_info_list* temp = ks->states[i].inneighbors;
                             state_info_list* new_node = (state_info_list*)malloc(sizeof(state_info_list));
                             new_node->state = &ks->states[j];
                             new_node->next = temp;
                             ks->states[i].inneighbors = new_node;
                             break;
                         }
                     }
                     break;
                 }
             }
             ks->transitions[i].to = strdup(to->valuestring);
             i++;
         }
     }

    // parse to get labels
    i = 0;
    cJSON_ArrayForEach(from, labels) {
        for (int j = 0; j < ks->num_states; j++) {
            if (strcmp(ks->states[j].name, from->string) == 0) {
                int num_labels = cJSON_GetArraySize(from);
                ks->states[j].labels = malloc(num_labels * sizeof(char*));
                ks->states[j].num_labels = num_labels;
                int k = 0;
                cJSON *to;
                cJSON_ArrayForEach(to, from) {
                    // printf("%s\n", to->valuestring);
                    ks->states[j].labels[k] = strdup(to->valuestring);
                    k++;
                }
                break;
            }
        }
    }
    free(json);
    cJSON_Delete(root);
}

void printKripke(Kripke* ks){
    printf("========================\n");
    printf("States:\n");
    for (int i = 0; i < ks->num_states; i++) {
        printf("  %s: ", ks->states[i].name);
        for (int j = 0; j < ks->states[i].num_labels; j++) {
            printf("%s ", ks->states[i].labels[j]);
        }
        printf("\n");
    }
    printf("Transitions:\n");
    for (int i = 0; i < ks->num_transitions; i++) {
        printf("  %s -> %s\n", ks->transitions[i].from, ks->transitions[i].to);
    }
}

void freeKripke(Kripke* ks) {
    if (ks == NULL) return; // Avoid double-free or null pointer dereference

    for (int i = 0; i < ks->num_states; i++) {
        free(ks->states[i].name);
        for (int j = 0; j < ks->states[i].num_labels; j++) {
            free(ks->states[i].labels[j]);
        }
        free(ks->states[i].labels);
        for (int j = 0; j < ks->states[i].outdegree; j++) {
            state_info_list* temp = ks->states[i].outneighbors;
            ks->states[i].outneighbors = ks->states[i].outneighbors->next;
            free(temp);
        }
        for (int j = 0; j < ks->states[i].indegree; j++) {
            state_info_list* temp = ks->states[i].inneighbors;
            ks->states[i].inneighbors = ks->states[i].inneighbors->next;
            free(temp);
        }
    }
    free(ks->states);

    for (int i = 0; i < ks->num_transitions; i++) {
        free(ks->transitions[i].from);
        free(ks->transitions[i].to);
    }
    free(ks->transitions);
}
