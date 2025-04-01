%{
#include "kripke.h"

typedef struct Node {
    char *type;
    struct Node *left;
    struct Node *right;
} Node;

Node *new_node(char *type, Node *left, Node *right) {
    Node *n = (Node *)malloc(sizeof(Node));
    n->type = strdup(type);
    n->left = left;
    n->right = right;
    return n;
}

void print_tree(Node *node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("%s\n", node->type);
    print_tree(node->left, depth + 1);
    print_tree(node->right, depth + 1);
}

%}

%union {
    char *str;
    struct Node *node;
}

%token OR AND NOT IMPLIES EX EG EU T F LPAREN RPAREN PROP
%type <node> expression

%%

expression:
      expression OR expression                        { $$ = new_node("|", $1, $3); }
    | expression AND expression                       { $$ = new_node("&", $1, $3); }
    | expression IMPLIES expression                   { $$ = new_node("->", $1, $3); }
    | E expression LBRACKET U expression RBRACKET     { $$ = new_node("EU", $2, $5); }
    | NOT expression                                  { $$ = new_node("~", $2, NULL); }
    | EX expression                                   { $$ = new_node("EX", $2, NULL); }
    | EG expression                                   { $$ = new_node("EG", $2, NULL); }
    | LPAREN expression RPAREN                        { $$ = $2; }
    | PROP                                            { $$ = new_node($1, NULL, NULL); }
    | T                                               { $$ = new_node("T", NULL, NULL); }
    | F                                               { $$ = new_node("F", NULL, NULL); }
    | AG expression                                   { node* neg = new_node("~", $2, NULL);
                                                        node* EFneg = new_node("EF", neg, NULL); 
                                                        $$ = new_node("~", EFneg, NULL); }
    | AF expression                                   { node* EFneg = new_node("EF", $2, NULL); 
                                                        $$ = new_node("~", EFneg, NULL); }
    | AX expression                                   { node* neg = new_node("~", $2, NULL);
                                                        $$ = new_node("EX", neg, NULL); }
    | A expression LBRACKET U expression RBRACKET     { node* neg = new_node("~", $2, NULL);
                                                        node* EFneg = new_node("EF", neg, NULL);
                                                        $$ = new_node("~", EFneg, NULL); }
    | EF expression                                   { $$ = new_node("EF", $2, NULL); }
    ;

%%

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <kripke.json>\n", argv[0]);
        return 1;
    }

    Kripke ks = initkripke();
    parseKripke(argv[1], &ks);
    printKripke(&ks);
    printf("Enter CTL formula:\n");
    yyparse();
    freeKripke(&ks);
    return 0;
}

void yyerror(const char *s) {
    printf("Parse error: %s\n", s);
}
