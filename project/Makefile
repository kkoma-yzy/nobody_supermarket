OBJ_DIR=/home/linux/myProject/project/obj
INCLUDE_DIR=/home/linux/myProject/project/include
BIN_DIR=/home/linux/myProject/project/bin

export OBJ_DIR INCLUDE_DIR BIN_DIR

all:
	make -C src
	make -C obj
clean:
	rm -rf ${OBJ_DIR}/server/*.o
	rm -rf ${OBJ_DIR}/client/*.o
	rm -rf ${BIN_DIR}/*
