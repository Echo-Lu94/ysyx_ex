#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)


//不含\0
size_t strlen(const char *s) {
    int n = 0;
//    assert(s != NULL);
//    while(s[n] != '\0'){
    while(*s != '\0'){
        n++;
        s++;
    }
    return n;
}

char *strcpy(char *dst, const char *src) {
    assert(dst != NULL);
    assert(src != NULL);
//    assert(strlen(dst) >= strlen(src));
    char *ret = dst;
    while((*dst++ = *src++));
    return ret;
}

char *strncpy(char *dst, const char *src, size_t n) {
    assert(dst != NULL);
    assert(src != NULL);
    char *ret = dst;
        
    while(n && (*(dst++) = *(src++))){
        n--;
    }
    while(n){
        *(dst++) = '\0';
        n--;
    }
    return ret;
}

char *strcat(char *dst, const char *src) {
    assert(dst && src);
    char *ret = dst;
    while(*dst != '\0'){
        dst++;
    }
    while((*dst++ = *src++));
    return ret;
}

int strcmp(const char *s1, const char *s2) {
    assert(s1 && s2);
    while(*s1 == *s2){
        if(*s1 == '\0') return 0;
        s1++;
        s2++;
    };
    return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    assert(s1 && s2);
    while(n && (*s1 == *s2)){
        n--;
        if(*s1 == '\0') return 0;
        s1++;
        s2++;
        }
    return *s1 - *s2;
}

void *memset(void *s, int c, size_t n) {
    assert(s);
    char *s1 = (char *)s;
    while(n--){
//        n--;
        *s1++ = c;
    }
//    *s1 = '\0';
    return s;
}

void *memmove(void *dst, const void *src, size_t n) {
    assert(dst && src);
    if(dst < src){
        char *p = (char *)dst;
        char *q = (char *)src;
        while(n--){
            *p++=*q++;
        }
    }
    else{
        char *p = (char *)dst+n;
        char *q = (char *)src+n;
        while(n--){
            *--p=*--q;
        }
    }
    return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
   assert(out != NULL && in != NULL);
	char *p = (char*)out;
	char *q = (char*)in;
	if(p == q)
		return p;
	while(n--){
		*p++ = *q++;
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
// wrong
//    assert(s1 && s2);
//    char *s1_t = (char *)s1;
//    char *s2_t = (char *)s2;
//    while(n && (*s1_t == *s2_t)){
//        n--;
//        s1_t++;
//        s2_t++;
//        }
//    return *s1_t - *s2_t;
    const char *p = (const char *)s1;
	const char *q = (const char *)s2;
	for(int i = 0;i < n;i++){
		if(*(p+i) > *(q+i))
			return 1;
		else if(*(p+i) < *(q+i))
			return -1;
	}
	return 0;    
}

#endif
