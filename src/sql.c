#include <sqlite3.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "util.h"
#include "sql.h"
#include "config.h"

static char *colored_str(const unsigned char *str)
{
    /* ex. converts: <!b!>Text<!x!> -> \e[0;34mText\e[0m */

    unsigned long len;
    char *out;
    char tmp[2];

    len = strlen((char *)str);
    out = ecalloc(sizeof(char *), len+101);

    const unsigned char *p = str;
    while (*p != 0) {
        if (*p == '<' && (*(p+1) == '!' || *(p+1) == '*')) {
            if ((*(p+3) == '!' || *(p+3) == '*') && *(p+4) == '>') {
                if (*(p+2) == 'x') {
                    strcat(out, "\e[0m");
                    goto out;
                }

                if (*(p+1) == '*' && *(p+4) == '*')
                    strcat(out, "\e[1;");
                else
                    strcat(out, "\e[0;");

                switch (*(p+2)) {
                case 'a': /* black */
                    strcat(out, "30m");
                    break;
                case 'r': /* red */
                    strcat(out, "31m");
                    break;
                case 'g': /* green */
                    strcat(out, "32m");
                    break;
                case 'y': /* yellow */
                    strcat(out, "32m");
                    break;
                case 'b': /* blue */
                    strcat(out, "34m");
                    break;
                case 'p': /* purple */
                    strcat(out, "35m");
                    break;
                case 'c': /* cyan */
                    strcat(out, "36m");
                    break;
                case 'w': /* white */
                    strcat(out, "37m");
                    break;
                }

            out:
                p=p+5;
                continue;
            }
        }
        sprintf(tmp, "%c", *p);
        strcat(out, tmp);
        p++;
    }
    return out;
}

extern void sql_create_table(sqlite3 *db)
{
    const char *sql;
    char *err;

    sql = "CREATE TABLE dates(" \
          "day INT NOT NULL," \
          "month INT NOT NULL," \
          "msg CHAR(300) NOT NULL);";

    if (sqlite3_exec(db, sql, NULL, NULL, &err)) {
        printf("error: %s\n", err);
        die("[FATAL] Unable to create a sql table", 1, 0);
    }
}

extern void sql_show_list(sqlite3 *db)
{
    const char *sql = "SELECT * FROM dates ORDER BY month, day;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL)) {
        printf("error: %s\n", sqlite3_errmsg(db));
        die("[FATAL] Unable to prepare the sql statement", 1, 0);
    }

    printf("+---+---+-------------------------------------------------\n");
	printf("|DAY|MON|MESSAGE\n");
	printf("+---+---+-------------------------------------------------\n");

    while ((sqlite3_step(stmt)) == SQLITE_ROW) {
        int day = sqlite3_column_int (stmt, 0);
        int mon = sqlite3_column_int(stmt, 1);
        const unsigned char *msg = sqlite3_column_text(stmt, 2);

        if (day < 10)
            printf("|%i  ", day);
        else
            printf("|%i ", day);

        if (mon < 10)
            printf("|%i  ", mon);
        else
            printf("|%i ", mon);

        printf("|%s\n", colored_str(msg));
        printf("+---+---+-------------------------------------------------\n");
    }
    sqlite3_finalize(stmt);
}

extern void sql_insert(sqlite3 *db, char *argv[])
{
    char *sql;
    char *err;

    sql = ecalloc(sizeof(char *), 334);

    /* ex. INSERT INTO dates VALUES(24, 12, 'codemas!'); */
    strcpy(sql, "INSERT INTO dates VALUES(");
    strcat(sql, argv[2]);
    strcat(sql, ", ");
    strcat(sql, argv[3]);
    strcat(sql, ", \'");
    strcat(sql, argv[4]);
    strcat(sql, "\');");

    #if DEBUG
    printf("[DEBUG] %s\n", sql);
    #endif

    sqlite3_exec(db, sql, NULL, NULL, &err);

    if (err)
        die(err, 1, 0);

    efree(sql);
}

extern void sql_remove(sqlite3 *db, char *argv[])
{
    char *sql;
    char *err;

    sql = ecalloc(sizeof(char *), 100);

    /* ex. DELETE FROM dates WHERE day = 24 AND month = 12; */
    strcpy(sql, "DELETE FROM dates WHERE day = ");
    strcat(sql, argv[2]);
    strcat(sql, " AND month = ");
    strcat(sql, argv[3]);
    strcat(sql, ";");

    #if DEBUG
    printf("[DEBUG] %s\n", sql);
    #endif

    sqlite3_exec(db, sql, NULL, NULL, &err);

    if (err)
        die(err, 1, 0);

    efree(sql);
}

extern void sql_today(sqlite3 *db)
{
    char *sql;
    const unsigned char *msg;
    char day[12];
    char mon[12];
    sqlite3_stmt *stmt;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    sprintf(day, "%i", tm.tm_mday);
    sprintf(mon, "%i", tm.tm_mon+1);

    sql = ecalloc(sizeof(char *), 50);

    /* ex. SELECT msg FROM dates WHERE day = 24 AND month = 12; */
    strcpy(sql, "SELECT msg FROM dates WHERE day = ");
    strcat(sql, day);
    strcat(sql, " AND month = ");
    strcat(sql, mon);
    strcat(sql, ";");

    #if DEBUG
    printf("[DEBUG] %s\n", sql);
    #endif

    if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL)) {
        printf("error: %s\n", sqlite3_errmsg(db));
        die("[FATAL] Unable to prepare the sql statement", 1, 0);
    }

    sqlite3_step(stmt);
    if ((msg = sqlite3_column_text(stmt, 0)))
        printf("%s\n", colored_str(msg));

    sqlite3_finalize(stmt);
    efree(sql);
}

extern void sql_test(sqlite3 *db, char *argv[])
{
    char *sql;
    const unsigned char *msg;
    sqlite3_stmt *stmt;

    sql = ecalloc(sizeof(char *), 50);

    strcpy(sql, "SELECT msg FROM dates WHERE day = ");
    strcat(sql, argv[2]);
    strcat(sql, " AND month = ");
    strcat(sql, argv[3]);
    strcat(sql, ";");

    #if DEBUG
    printf("[DEBUG] %s\n", sql);
    #endif

    if (sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL)) {
        printf("error: %s\n", sqlite3_errmsg(db));
        die("[FATAL] Unable to prepare the sql statement", 1, 0);
    }

    sqlite3_step(stmt);
    if ((msg = sqlite3_column_text(stmt, 0)))
        printf("%s\n", colored_str(msg));

    sqlite3_finalize(stmt);
    efree(sql);
}
