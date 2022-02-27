#ifndef _SQL_H
#define _SQL_H

extern void sql_create_table(sqlite3 *db);
extern void sql_show_list(sqlite3 *db);
extern void sql_insert(sqlite3 *db, char *argv[]);
extern void sql_remove(sqlite3 *db, char *argv[]);
extern void sql_today(sqlite3 *db);
extern void sql_test(sqlite3 *db, char *argv[]);

#endif /* _SQL_H */
