#ifndef PATH_H
#define PATH_H

#ifdef _WIN32
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

char *path_join(const char *base, const char *joined);

#endif