%{
#include <ctype.h>
#include "CTL.h"
Kripke ks;
%}

%union {
    char *str;
    struct Node *node;
}

%type <node> expression formula
%token OR AND NOT IMPLIES EX EG EU T F LPAREN RPAREN LBRACKET RBRACKET AG AF AX A BEG END EF E U
%token <str> PROP


%%
formula:
      BEG expression END     { $$ = new_node("FORMULA", $2, NULL); printtree($$); print_formula($2); check($2); freetree($$); }
    | expression             { $$ = new_node("FORMULA", $1, NULL); printtree($$); print_formula($1); freetree($$); }
    ;
expression:
      expression OR expression                        { $$ = new_node("|", $1, $3); printtree($$); process_or($$,&ks);}
    | expression AND expression                       { Node* negp = new_node("~", $1, NULL); process_neg(negp, &ks);
                                                        Node* negq = new_node("~", $3, NULL); process_neg(negq, &ks);
                                                        Node* negpORnegq = new_node("|", negp, negq); process_or(negpORnegq, &ks);
                                                        $$ = new_node("~", negpORnegq, NULL); printtree($$); process_neg($$, &ks);}
    | expression IMPLIES expression                   { Node* negp = new_node("~", $1, NULL); process_neg(negp, &ks);
                                                        $$ = new_node("|", negp, $3); printtree($$); process_or($$, &ks);}
    | E LBRACKET expression U expression RBRACKET     { $$ = new_node("EU", $3, $5); printtree($$); process_EU($$, &ks);}
    | NOT expression                                  { $$ = new_node("~", $2, NULL); printtree($$); process_neg($$, &ks);}
    | EX expression                                   { $$ = new_node("EX", $2, NULL); printtree($$); process_EX($$, &ks);}
    | EG expression                                   { $$ = new_node("EG", $2, NULL); printtree($$); process_EG($$, &ks);}
    | LPAREN expression RPAREN                        {$$ = $2; }
    | PROP                                            { $$ = new_node($1, NULL, NULL); printtree($$); process_prop($$,&ks ); free($1);}
    | T                                               { $$ = new_node("T", NULL, NULL); printtree($$); process_T($$,&ks);}
    | F                                               { $$ = new_node("F", NULL, NULL); printtree($$); process_F($$,&ks);}
    | AG expression                                   { Node* neg = new_node("~", $2, NULL); process_neg(neg, &ks);
                                                        Node* truenode = new_node("T", NULL, NULL); process_T(truenode, &ks);
                                                        Node* EFneg = new_node("EU", truenode, neg); process_EU(EFneg, &ks); printf("EU states\n"); print_states(EFneg);
                                                        $$ = new_node("~", EFneg, NULL); process_neg($$, &ks); printtree($$);}
    | AF expression                                   { Node* neg = new_node("~", $2, NULL); process_neg(neg, &ks);
                                                        Node* EGneg = new_node("EG", neg, NULL); process_EG(EGneg, &ks);
                                                        $$ = new_node("~", EGneg, NULL); printtree($$); process_neg($$, &ks);}
    | AX expression                                   { Node* neg = new_node("~", $2, NULL); process_neg(neg, &ks);
                                                        Node* negEX = new_node("EX", neg, NULL); process_EX(negEX, &ks);
                                                        $$ = new_node("~", negEX, NULL); printtree($$); process_neg($$, &ks);}
    | A expression LBRACKET U expression RBRACKET     { Node* negpsi = new_node("~", $5, NULL); process_neg(negpsi, &ks);
                                                        Node* EGnegpsi = new_node("EG", negpsi, NULL); process_EG(EGnegpsi, &ks);
                                                        Node* phiorpsi = new_node("|", $2, $5); process_or(phiorpsi, &ks);
                                                        Node* negphiorpsi = new_node("~", phiorpsi, NULL); process_neg(negphiorpsi, &ks);
                                                        Node* EUnode = new_node("EU", negpsi,negphiorpsi); process_EU(EUnode, &ks);
                                                        Node* EUorEG = new_node("|", EUnode, EGnegpsi); process_or(EUorEG, &ks);
                                                        $$ = new_node("~", EUorEG, NULL); printtree($$); process_neg($$, &ks);}                                                       
    | EF expression                                   { Node* truenode = new_node("T", NULL, NULL); process_T(truenode, &ks);
                                                        $$ = new_node("EU",truenode, $2); printtree($$);}
    ;

%%

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <kripke.json>\n", argv[0]);
        return 1;
    }

    
    parseKripke(argv[1], &ks);
    printKripke(&ks);
    // printf("Enter CTL formula:\n");
    yyparse();
    freeKripke(&ks);
    return 0;
}

void yyerror(const char *s) {
    printf("Parse error: %s\n", s);
}
