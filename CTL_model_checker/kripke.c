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
    printf("here\n");

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
    int i = 0;
    cJSON *state;
    cJSON_ArrayForEach(state, states) {
        // printf("%s\n", state->valuestring);
        strcpy(ks->states[i].name, state->valuestring);
        i++;
    }
    ks->num_states = i;

    // parse to get transitions
    i = 0;
    cJSON* from;
    cJSON* to;
    cJSON_ArrayForEach(from, transitions) {
        cJSON_ArrayForEach(to, from) {
            // printf("%s -> %s\n", from->string, to->valuestring);
            strcpy(ks->transitions[i].from, from->string);
            strcpy(ks->transitions[i].to, to->valuestring);
            i++;
        }
    }
    ks->num_transitions = i;

    // parse to get labels
    i = 0;
    cJSON_ArrayForEach(from, labels) {
        for (int j = 0; j < ks->num_states; j++) {
            if (strcmp(ks->states[j].name, from->string) == 0) {
                i = 0;
                cJSON_ArrayForEach(to, from) {
                    printf("%s\n", to->valuestring);
                    strcpy(ks->states[j].labels[i], to->valuestring);
                    i++;
                }
                ks->states[j].num_labels = i;
                break;
            }
        }
    }
    free(json);
    cJSON_Delete(root);
}

void printKripke(Kripke* ks){
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

    // Free memory allocated for states
    for (int i = 0; i < MAX_STATES; i++) {
        free(ks->states[i].name);

        for (int j = 0; j < MAX_LABELS; j++) {
            free(ks->states[i].labels[j]); // Free each label string
        }
        free(ks->states[i].labels); // Free the labels array
    }

    // Free memory allocated for transitions
    for (int i = 0; i < MAX_STATES * 3; i++) {
        free(ks->transitions[i].from);
        free(ks->transitions[i].to);
    }

    // Free the Kripke structure itself
    free(ks);
}



Kripke initkripke(){
    //allocate memory for the kripke structure
    Kripke* ks = (Kripke*)malloc(sizeof(Kripke));
    if (ks == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for Kripke structure\n");
        exit(1);
    }
    //allocate memory for the states
    for (int i = 0; i < MAX_STATES; i++) {
        ks->states[i].name = (char*)malloc(50);
        if (ks->states[i].name == NULL) {
            fprintf(stderr, "Error: Could not allocate memory for state name\n");
            exit(1);
        }
        ks->states[i].labels = (char**)malloc(MAX_LABELS * sizeof(char*));
        if (ks->states[i].labels == NULL) {
            perror("Memory allocation failed for state labels");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < MAX_LABELS; j++) {
            ks->states[i].labels[j] = (char*)malloc(50);
            if (ks->states[i].labels[j] == NULL) {
                fprintf(stderr, "Error: Could not allocate memory for state label\n");
                exit(1);
            }
        }
        ks->states[i].num_labels = 0;
        ks->states[i].visited = 0;
        for (int i = 0; i < MAX_STATES * 3; i++) {
            ks->transitions[i].from = (char*)malloc(50 * sizeof(char));
            ks->transitions[i].to = (char*)malloc(50 * sizeof(char));
            if (ks->transitions[i].from == NULL || ks->transitions[i].to == NULL) {
                perror("Memory allocation failed for transitions");
                exit(EXIT_FAILURE);
            }
        }
    }
    return *ks;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <kripke.json>\n", argv[0]);
        return 1;
    }

    Kripke ks = initkripke();
    parseKripke(argv[1], &ks);
    printKripke(&ks);
    freeKripke(&ks);
    return 0;
}
