gnrclist/gnrclist.o: gnrclist/gnrclist.h gnrclist/gnrclist.c
	gcc -o gnrclist/gnrclist.o -c gnrclist/gnrclist.c

gnrcstack/gnrcstack.o: gnrclist/gnrclist.o gnrcstack/gnrcstack.h gnrcstack/gnrcstack.c
	gcc -o gnrcstack/gnrcstack.o -c gnrcstack/gnrcstack.c

avltree.o: gnrcstack/gnrcstack.o avltree.c avltree.h
	gcc -o avltree.o -c avltree.c 

avltest: avltree.o test/avltest.c
	gcc -o test/avltest.o -c test/avltest.c
	gcc -o avltest avltree.o test/avltest.o gnrcstack/gnrcstack.o gnrclist/gnrclist.o

clean:
	if [ -e avltest ];  then rm avltest;  fi
	find -name "*.o"   -exec rm {} \;
	find -name "*.dot" -exec rm {} \;
	find -name "*.png" -exec rm {} \;
