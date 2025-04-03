CTL_checker: CTL.y CTL.l kripke.c
	bison -d CTL.y
	flex CTL.l
	gcc -o CTL_checker CTL.tab.c lex.yy.c kripke.c CTL.c -lcjson

val:CTL_checker
	valgrind --leak-check=full --track-origins=yes ./CTL_checker kripke.json <formula.txt

all:CTL_checker
	./CTL_checker kripke.json <formula.txt

seg:CTL.y CTL.l kripke.c
	bison -d CTL.y
	flex CTL.l
	gcc -g -o CTL_checker CTL.tab.c lex.yy.c kripke.c CTL.c -lcjson

sanit:CTL.y CTL.l kripke.c
	bison -d CTL.y
	flex CTL.l
	gcc -g -o CTL_checker CTL.tab.c lex.yy.c kripke.c CTL.c -lcjson -fsanitize=address
	
clean:
	rm -f CTL_checker CTL.tab.c CTL.tab.h lex.yy.c