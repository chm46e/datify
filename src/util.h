#ifndef _UTIL_H
#define _UTIL_H

extern void wlog(const char *msg);
extern void die(const char *err, int exit_code, int log_switch);
extern void *emalloc(int size);
extern void *ecalloc(int num, int size);
extern void efree(void *mem);

#endif /* _UTIL_H */
