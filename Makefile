CC=clang
CFLAGS=-I ./ -g
all:	spell_list_test spell_hash_test

spell_list_test: spell_list_test.o spellutil.o
	${CC} ${CFLAGS} -o spell_list_test spell_list_test.o spellutil.o

spell_hash_test: spell_hash_test.o spellutil.o
	${CC} ${CFLAGS} -o spell_hash_test  spell_hash_test.o spellutil.o

spell_hash_test.o: spell_hash_test.c
	${CC} ${CFLAGS} -c spell_hash_test.c

spell_list_test.o: spell_list_test.c
	${CC} ${CFLAGS} -c spell_list_test.c

spellutil.o: spellutil.c spellutil.h
	${CC} ${CFLAGS} -c spellutil.c

clean:
	rm -rf *.o spell_list_test *.a *.so core
