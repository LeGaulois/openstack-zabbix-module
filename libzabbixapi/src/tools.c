#include <stdio.h>
#include <stdlib.h>
#include "tools.h"
#include <string.h>
#include <regex.h>


int modulo(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

int reallocs(void **ptr, size_t taille){
    void *ptr_realloc = realloc(*ptr, taille);

    if (ptr_realloc != NULL){
        *ptr = ptr_realloc;
        return 1;
    }
    else{
        free(ptr);
        return -1;
    }
}


int countDigits( int value )
{
    int result = 0;
    if(value==0) return 1;
    while( value != 0 ) {
       value /= 10;
       result++;
    }
    return result;
}

void inttostr(char **ret, const int number){
    *ret = calloc(countDigits(number)+1, sizeof(char));
    sprintf(*ret,"%d", number);
}

int strrncat(char **ptr, const char *s2, const int nb){
    if(reallocs((void**)(ptr),strlen(*ptr)+nb+1)==-1){
        return -1;
    }
    strncat(*ptr, s2, nb);
    return 1;
}

int strancpy(char **ptr, const char *s2, const int nb){
    if((*ptr=calloc(nb+1,sizeof(char)))==NULL){
        return -1;
    }
    strncpy(*ptr, s2, nb);
    return 1;
}

int strContains(const char *str_regex, const char *chaine){
    int err,match;
    regex_t preg;


    err = regcomp (&preg,str_regex, REG_NOSUB | REG_EXTENDED);
    if (err == 0){
        match = regexec (&preg, chaine, 0, NULL, 0);
        regfree(&preg);
        return match;
    }
    else {
        regfree(&preg);
        return -1;
    }
}
