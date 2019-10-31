db.o:db.c sqlite.o sqliteinterface.o
	gcc -c $< -o $@ -I${INCLUDE_DIR}/server
	mv $@ ${OBJ_DIR}/server
sqlite.o:sqlite3.c
	gcc -c $^ -o $@ -I${INCLUDE_DIR}/server
	mv $@ ${OBJ_DIR}/server
sqliteinterface.o:sqliteinterface.c
	gcc -c $^ -o $@ -I${INCLUDE_DIR}/server
	mv $@ ${OBJ_DIR}/server
