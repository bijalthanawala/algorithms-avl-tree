avltree: avltree.c avltree.h
	gcc -o avltree.o -c avltree.c

avltest: avltree test/avltest.c
	gcc -o test/avltest.o -c test/avltest.c
	gcc -o avltest avltree.o test/avltest.o

clean:
	if [ -e avltest ];  then rm avltest;  fi
	find -name "*.o"   -exec rm {} \;
	find -name "*.dot" -exec rm {} \;
	find -name "*.png" -exec rm {} \;
