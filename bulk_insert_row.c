/*
 * =====================================================================================
 *
 *       Filename:  logger_bd.c
 *
 *    Description:  connecting and logging a messages to the database
 *    		    Bulk Insert (Row-wise Binding)
 *    		    https://mariadb.com/kb/en/bulk-insert-row-wise-binding/
 *
 *        Version:  1.0
 *        Created:  01/28/2020 11:02:59 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <mysql.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void show_mysql_error(MYSQL *mysql)
{
  printf("Error(%d) [%s] \"%s\"", mysql_errno(mysql),
                                  mysql_sqlstate(mysql),
                                  mysql_error(mysql));
  exit(-1);
}

static void show_stmt_error(MYSQL_STMT *stmt)
{
  printf("Error(%d) [%s] \"%s\"", mysql_stmt_errno(stmt),
                                  mysql_stmt_sqlstate(stmt),
                                  mysql_stmt_error(stmt));
  exit(-1);
}

int main(int argc, char *argv[])
{
  MYSQL *mysql;
  MYSQL_STMT *stmt;
  MYSQL_BIND bind[3];

  struct st_data {
    unsigned long id;
    char id_ind;
    char forename[30];
    char forename_ind;
    char surname[30];
    char surname_ind;
  };

  struct st_data data[]= {
    {0, STMT_INDICATOR_NULL, "Monty", STMT_INDICATOR_NTS, "Widenius", STMT_INDICATOR_NTS},
    {0, STMT_INDICATOR_NULL, "David", STMT_INDICATOR_NTS, "Axmark", STMT_INDICATOR_NTS},
    {0, STMT_INDICATOR_NULL, "default", STMT_INDICATOR_DEFAULT, "N.N.", STMT_INDICATOR_NTS},
  };

  unsigned int array_size= 3;
  size_t row_size= sizeof(struct st_data);

  mysql= mysql_init(NULL);

  /* connect to MariaDB server */
  if (!mysql_real_connect(mysql, "localhost", "pgg", "o2o416", 
                          "example_db", 0, "/tmp/mysql.sock", 0))
    show_mysql_error(mysql);

  if (mysql_query(mysql, "DROP TABLE IF EXISTS bulk_example2"))
    show_mysql_error(mysql);

  if (mysql_query(mysql, "CREATE TABLE bulk_example2 (id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,"\
                         "forename CHAR(30) NOT NULL DEFAULT 'unknown', surname CHAR(30))"))
    show_mysql_error(mysql);

  stmt= mysql_stmt_init(mysql);
  printf("line 80\n");
  const char str[] = "INSERT INTO bulk_example2 VALUES (?,?,?)";
  unsigned long len = sizeof(str);
  printf("%s  %ld\n", str, len);
  if (mysql_stmt_prepare(stmt, str, len))
  {  
    printf("line 86\n");
    show_stmt_error(stmt);
  }
  printf("line 89\n");
  memset(bind, 0, sizeof(MYSQL_BIND) * 3);
  printf("line 91\n");
  /* We autogenerate id's, so all indicators are STMT_INDICATOR_NULL */
  bind[0].u.indicator= &data[0].id_ind;
  bind[0].buffer_type= MYSQL_TYPE_LONG;

  bind[1].buffer= &data[0].forename;
  bind[1].buffer_type= MYSQL_TYPE_STRING;
  bind[1].u.indicator= &data[0].forename_ind;

  bind[2].buffer_type= MYSQL_TYPE_STRING;
  bind[2].buffer= &data[0].surname;
  bind[2].u.indicator= &data[0].surname_ind;

  /* set array size */
  mysql_stmt_attr_set(stmt, STMT_ATTR_ARRAY_SIZE, &array_size);

  /* set row size */
  mysql_stmt_attr_set(stmt, STMT_ATTR_ROW_SIZE, &row_size);

  /* bind parameter */
  mysql_stmt_bind_param(stmt, bind);

  /* execute */
  printf("line 114\n");
  if (mysql_stmt_execute(stmt))
    show_stmt_error(stmt);

  mysql_stmt_close(stmt);
  mysql_close(mysql);
}
