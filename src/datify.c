#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>

#include "util.h"
#include "sql.h"
#include "config.h"

struct client {
	char *home;
	char *db_file;
	int new;
};

static sqlite3 *db;
static struct client user;
static int opt;

#define L_DB 1
#define W_DB 2
#define D_DB 4
#define T_DB 8

static void get_user_info(void);
static void setup(void);
static void show_help();
static void parameter_handler(int argc, char *argv[]);
static void opt_handler(char *argv[]);
static void cleanup(void);

static void get_user_info(void)
{
	if (!(user.home = getenv("HOME")))
		user.home = getpwuid(getuid())->pw_dir;

	if (!(strcmp(user.home, "")))
		die("[FATAL] Unable to get user's homedir", 1, 0);

	user.db_file = ecalloc(sizeof(char), strlen(user.home) + 19);

	strcpy(user.db_file, user.home);
	strcat(user.db_file, "/.config/datify.db");
}

static void setup(void)
{
	get_user_info();

	if (access(user.db_file, R_OK|W_OK))
		user.new = 1;
}

static void show_help(void)
{
	printf("datify: notify important dates\n");
	printf("Usage: datify [options]\n");
	printf("\nOptions:\n");
	printf("  <clear>                       show today's message\n");
	printf("  list                          list all dates\n");
	printf("  add <day> <month> <\'message\'> add a date\n");
	printf("  remove <day> <month>          remove a date\n");
	printf("  test <day> <month>            test a day to see how it looks\n");
	printf("  -h (--help)                   show this message");
	exit(0);
}

static void parameter_handler(int argc, char *argv[])
{
	if (argc >= 2) {
		switch (argv[1][0]) {
		case '-':
			show_help();
			break;
		case 'l':
			opt |= L_DB;
			break;
		case 'a':
			opt |= W_DB;
			break;
		case 'r':
			opt |= D_DB;
			break;
		case 't':
			opt |= T_DB;
			break;
		default:
			break;
		}
	}
}

static void opt_handler(char *argv[])
{
	if (opt & L_DB)
		sql_show_list(db);
	else if (opt & W_DB)
		sql_insert(db, argv);
	else if (opt & D_DB)
		sql_remove(db, argv);
	else if (opt & T_DB)
		sql_test(db, argv);
	else {
		sql_today(db);
	}
}

static void cleanup(void)
{
	efree(user.db_file);
}

int main(int argc, char *argv[])
{
	parameter_handler(argc, argv);
	setup();

	if (sqlite3_open(user.db_file, &db))
		die("[FATAL] Unable to open the database", 1, 0);

	if (user.new)
		sql_create_table(db);

	opt_handler(argv);

	cleanup();
	sqlite3_close(db);
	return 0;
}
