all: test_maria_r test_maria_c

test_maria_r: clean bulk_insert_row.c
	gcc -g3 -Wall bulk_insert_row.c -o test_maria_r -I/usr/local/src/server/libmariadb/include -L/usr/local/src/server/libmariadb/libmariadb -lmysqlclient

test_maria_c: clean bulk_insert_col.c
	gcc -g3 -Wall bulk_insert_col.c -o test_maria_c -I/usr/local/src/server/libmariadb/include -L/usr/local/src/server/libmariadb/libmariadb -lmysqlclient

clean: 
	rm -f test_maria_r test_maria_c
