all:main.o
	make -C network
	make -C db
main.o:main.c
	gcc -c $^ -o $@ -I${INCLUDE_DIR}/server
	mv $@ ${OBJ_DIR}/server
