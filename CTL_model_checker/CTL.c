#include "CTL.h"

void printformula(Node *node) {
    if (!node) return;
    //print fully paranthesised formula from parse tree
    if (strcmp(node->type, "EU")==0) {
        printf("(");
        printf("E");
        printf("[");
        if (node->left) printformula(node->left);
        printf("U");
        if (node->right) printformula(node->right);
        printf("]");
        printf(")");
        return;
    }
    else{
        printf("(");
        printf("%s", node->type);
        if (node->left) printformula(node->left);
        if (node->right) printformula(node->right);
        printf(")");
    }
    return;
}

void print_formula(Node* node){
    printf("========================\n");
    printf("Fully paranthesised formula in terms of EG, EU and EX:\n");
    printformula(node);
    printf("\n");
    return;
}


Node *new_node(char *type, Node *left, Node *right) {
    Node *n = (Node *)malloc(sizeof(Node));
    n->type = strdup(type);
    n->left = left;
    n->right = right;
    n->sat_states = NULL;
    n->sat_states_count = 0;
    return n;
}
void freetree(Node *node) {
    if (!node) return;
    freetree(node->left);
    freetree(node->right);
    free(node->type);
    // recursively free the satisfying states list
    satisfying_states* temp = node->sat_states;
    satisfying_states* next;
    while (temp!=NULL){
        next = temp->next;
    //     free(temp->state);
        free(temp);
        temp = next;
    }
    free(node);
    return;
}

satisfying_states* add_state(satisfying_states* head, State* state) {
    satisfying_states* new_node = (satisfying_states*)malloc(sizeof(satisfying_states));
    new_node->state = state;
    new_node->next = head;
    return new_node;
}

void print_tree(Node *node, int depth, int *num_nodes) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    if (strcmp(node->type, "FORMULA")!=0) (*num_nodes)++;
    printf("%s\n", node->type);
    print_tree(node->left, depth + 1, num_nodes);
    print_tree(node->right, depth + 1, num_nodes);
}

void printtree(Node *node){
    int num_nodes = 0;
    if (strcmp(node->type, "FORMULA")==0) {
        printf("========================\n");
        printf("Printing entire tree\n");
        print_tree(node-> left, 0, &num_nodes);
        printf("Number of nodes in the tree: %d\n", num_nodes);
    }
    else{
        printf("========================\n");
        printf("Printing subtree rooted at %s\n", node->type);
        print_tree(node, 0, &num_nodes);
    }
    return;
}

void print_states(satisfying_states* head){
    satisfying_states* temp = head;
    while (temp!=NULL){
        printf("%s  ", temp->state->name);
        temp = temp->next;
    }
    printf("\n");
    return;
}

void process_prop(Node* node, Kripke* ks){

    // check if the proposition in the node is present in the labels of every state of kripke structure
    for (int i=0; i<ks->num_states; i++){
        for (int j=0; j<ks->states[i].num_labels; j++){
            if (strcmp(node->type, ks->states[i].labels[j])==0){
                node->sat_states = add_state(node->sat_states, &(ks->states[i]));
                 // add the formula to the satisfying formulae list of the state                
                break;
            }
        }
    }  
    print_states(node->sat_states);
    return;
}

void process_neg(Node* node, Kripke* ks){
    // get the satisfying states of the child node
    satisfying_states* sat_states = node->left->sat_states;
    // get the states of the kripke structure
    for (int i=0; i<ks->num_states; i++){
        int flag = 0;
        satisfying_states* temp = sat_states;
        while (temp!=NULL){
            if (strcmp(ks->states[i].name, temp->state)==0){
                flag = 1;
                break;
            }
            temp = temp->next;
        }
        if (flag==0){
            node->sat_states = add_state(node->sat_states, &(ks->states[i]));
        }
    }
    print_states(node->sat_states);
    return;
}

void process_or(Node* node, Kripke* ks){
    // get the satisfying states of the child nodes
    satisfying_states* sat_states1 = node->left->sat_states;
    satisfying_states* sat_states2 = node->right->sat_states;
    // get the states of the kripke structure
    for (int i=0; i<ks->num_states; i++){
        int flag = 0;
        satisfying_states* temp = sat_states1;
        while (temp!=NULL){
            if (strcmp(ks->states[i].name, temp->state)==0){
                flag = 1;
                break;
            }
            temp = temp->next;
        }
        temp = sat_states2;
        while (temp!=NULL){
            if (strcmp(ks->states[i].name, temp->state)==0){
                flag = 1;
                break;
            }
            temp = temp->next;
        }
        if (flag==1){
            node->sat_states = add_state(node->sat_states, &(ks->states[i]));
        }
    }
    print_states(node->sat_states);
    return;
}

void process_T(Node* node, Kripke* ks){
    // add all states to satisfying states
    for (int i=0; i<ks->num_states; i++){
        node->sat_states = add_state(node->sat_states, &(ks->states[i]));
    }   
    print_states(node->sat_states);
    return;
}

void process_F(Node* node, Kripke* ks){
   node->sat_states = NULL;
    print_states(node->sat_states);
    return;
    
}

void process_EX(Node* node, Kripke* ks){
    // get the satisfying states of the child node

    satisfying_states* sat_child_states = node->left->sat_states;
    // satisfying states are the inneighbours of sat_child_states
    while(sat_child_states != NULL){
        state_info_list* inneighbours = sat_child_states->state->inneighbors;
        if (inneighbours != NULL){
            while (inneighbours!= NULL){
                node->sat_states = add_state(node->sat_states, inneighbours->state);
                inneighbours = inneighbours-> next; 
            }
    }
        sat_child_states = sat_child_states -> next;
    }   
    print_states(node->sat_states);
    return;
}

queue* init_queue(){
    queue* q = (queue*)malloc(sizeof(queue));
    q->state = NULL;
    q->next = NULL;
    return q;
    
}
void print_queue(queue* q){
    queue* temp = q;
    while (temp!=NULL){
        printf("%s  ", temp->state->name);
        temp = temp->next;
    }
    printf("\n");
    return;
}

queue* enqueue(queue* q, State* state){
    queue* temp = q;
    while (temp->next!=NULL){
        temp = temp->next;
    }
    queue* new_node = (queue*)malloc(sizeof(queue));
    new_node->state = state;
    new_node->next = NULL;
    if (temp->state == NULL){
        return new_node;
    }
    temp->next = new_node;
    return q;
}

State* dequeue(queue* q){
    State* state = q->state;
    q = q->next;
    return state;
}

void process_EU(Node* node, Kripke* ks){
    
    satisfying_states* sat_states1 = node->left->sat_states;
    if (sat_states1 == NULL){
        printf("No satisfying states for left child\n");
        return;
    }
    
    satisfying_states* sat_states2 = node->right->sat_states;
    if (sat_states2 == NULL){
        printf("No satisfying states for right child\n");
        return;
    }
    node->sat_states = sat_states2;
    queue* q = init_queue();
    satisfying_states* temp = sat_states2;
    while (temp!=NULL){
        temp->state->visited = 1;
        q = enqueue(q, temp->state);
        temp = temp->next;
    }
    // print_queue(q);
    while (q->next!=NULL){
        State* state = dequeue(q);
        q = q->next;
        // print_queue(q);
        if (state!= NULL){
        state_info_list* inneighbours = state->inneighbors;
        while (inneighbours!=NULL){
            State* instate = inneighbours->state;
            if (instate->visited == 0){
                instate->visited = 1;
                satisfying_states* temp = sat_states1;
                while (temp!=NULL){
                    if (strcmp(instate->name, temp->state->name)==0){
                        node->sat_states = add_state(node->sat_states, state);
                        q = enqueue(q, state);
                        break;
                    }
                    temp = temp->next;
                }
            }
            inneighbours = inneighbours->next;
        }
    }}
    print_states(node->sat_states);
    // reset the visited flag
    for (int i=0; i<ks->num_states; i++){
        ks->states[i].visited = 0;
    }
    return;
}

void process_EG(Node* node, Kripke* ks){
    // get the satisfying states of the child node
    satisfying_states* sat_states = node->left->sat_states;
    // get the states of the kripke structure
    int flag = 1;
    while (flag == 1){
        flag = 0;
        satisfying_states* temp = sat_states;
        // for each state in sat_states, if none of its outneighbours are in sat_states, remove that state from sat_states, update flag
        while (temp!=NULL){
            state_info_list* outneighbours = temp->state->outneighbors;
            int flag2 = 0;
            while (outneighbours!=NULL){
                satisfying_states* temp2 = sat_states;
                while (temp2!=NULL){
                    if (strcmp(outneighbours->state->name, temp2->state->name)==0){
                        flag2 = 1;
                        break;
                    }
                    temp2 = temp2->next;
                }
                if (flag2 == 1){
                    break;
                }
                outneighbours = outneighbours->next;
            }
            if (flag2 == 0){ // none of the outneighbours are in sat_states
                flag = 1;
                satisfying_states* temp3 = sat_states;
                satisfying_states* prev = NULL;
                while (temp3!=NULL){
                    if (strcmp(temp3->state->name, temp->state->name)==0){
                        if (prev == NULL){
                            sat_states = sat_states->next;
                        }
                        else{
                            prev->next = temp3->next;
                        }
                        break;
                    }
                    prev = temp3;
                    temp3 = temp3->next;
                }
            }
            temp = temp->next;
        }
    }
    node->sat_states = sat_states;
    print_states(node->sat_states);
    return;
}

void check(Node* node){
    // check if the satisfying states of the node contatin S0
    satisfying_states* temp = node->sat_states;
    while (temp!=NULL){
        if (strcmp(temp->state->name, "S0")==0){
            printformula(node);
            printf(" is satisfied by the kripke structure\n");
            return;
        }
        temp = temp->next;
    }
    print_formula(node);
    printf(" : Kripke structure does not satisfy this formula\n");
    return;
}