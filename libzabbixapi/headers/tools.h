#ifndef TOOLS_H_INCLUDED
#define TOOLS_H_INCLUDED

int modulo(int a, int b);
int reallocs(void **ptr, size_t taille);
int countDigits( int value);
void inttostr(char **ret, const int number);
int strrncat(char **ptr, const char *s2, const int nb);
int strancpy(char **ptr, const char *s2, const int nb);
int strContains(const char *str_regex, const char *chaine);

#endif
