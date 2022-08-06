#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* read_file(const char* filename) {
    FILE* f = fopen(filename, "r");
    if(f == NULL)
        return NULL;
    char c;
    string s = string_new();
    while(!feof(f)) {
        c = fgetc(f);
        if(c == EOF)
            break;
        string_pushc(&s, c);
    }
    fclose(f);
    return s.cstr;
}

string string_new() {
    string str;
    str.length = 0;
    str.buff_size = 2;
    str.cstr = malloc(2);
    memset(str.cstr, 0, 2);
    return str;
}

string string_from_lit(const char* s) {
    string str = string_new();
    string_pushs(&str, s);
    return str;
}

string string_from_string(const char* s) {
    string str;
    str.length = strlen(s);
    str.buff_size = 2;
    while(str.buff_size < str.length+1)
        str.buff_size *= 2;
    str.cstr = (char*)s;
    return str;
}

void string_allocate(string* str, int length) {
    int buff_size = str->buff_size;
    while(buff_size < str->length + length + 1)
        buff_size *= 2;
    if(buff_size != str->buff_size) {
        char* cstr = malloc(buff_size);
        memset(cstr + str->length - 1, 0, buff_size - str->length + 1);
        strcpy(cstr, str->cstr);
        free(str->cstr);
        str->cstr = cstr;
        str->buff_size = buff_size;
    }
}

void string_free(string* str) {
    free(str->cstr);
}

void string_pushc(string* str, const char c) {
    if(str->length + 2 > str->buff_size) {
        char* cstr = malloc(2 * str->buff_size);
        memset(cstr + str->length, 0, 2 * str->buff_size - str->length);
        strcpy(cstr, str->cstr);
        free(str->cstr);
        str->buff_size *= 2;
        str->cstr = cstr;
    }
    str->cstr[str->length] = c;
    str->length++;
}

void string_pushs(string* str, const char* s) {
    int l = strlen(s);
    string_allocate(str, l);
    strcpy(str->cstr + str->length, s);
    str->length += l;
}

void string_pushint(string* str, const int i) {
    char buffer[20];
    snprintf(buffer, 18, "%d\0", i);
    int l = strlen(buffer);
    string_allocate(str, l);
    strcpy(str->cstr + str->length, buffer);
    str->length += l;
}

void string_concat(string* str, string tail) {
    string_pushs(str, tail.cstr);
    free(tail.cstr);
}

vector vector_new(int elem_size) {
    vector v;
    v.length = 0;
    v.elem_size = elem_size;
    v.buff_size = 1;
    v.carr = malloc(elem_size);
    memset(v.carr, 0, elem_size);
    return v;
}

void vector_allocate(vector* vec, int length) {
    if(vec->length + length <= vec->buff_size)
        return;
    int buff_size = vec->buff_size;
    while(buff_size < vec->length + length)
        buff_size *= 2;
    void* newarr = malloc(buff_size * vec->elem_size);
    memcpy(newarr, vec->carr, vec->length * vec->elem_size);
    memset(newarr + vec->length * vec->elem_size, 0, (buff_size - vec->length) * vec->elem_size);
    free(vec->carr);
    vec->carr = newarr;
    vec->buff_size = buff_size;
}

void vector_push(vector* vec, void* elem) {
    if(vec->length == vec->buff_size) {
        void* newarr = malloc(2 * vec->buff_size * vec->elem_size);
        memcpy(newarr, vec->carr, vec->buff_size * vec->elem_size);
        memset(newarr + vec->buff_size * vec->elem_size, 0, vec->buff_size * vec->elem_size);
        free(vec->carr);
        vec->buff_size *= 2;
        vec->carr = newarr;
    }
    memcpy(vec->carr + vec->length * vec->elem_size, elem, vec->elem_size);
    vec->length++;
}

void* vector_item(const vector* vec, int index) {
    return vec->carr + index * vec->elem_size;
}

void* vector_pop(vector* vec) {
    void* item = vec->carr + (vec->length - 1) * vec->elem_size;
    void* newitem = malloc(vec->elem_size);
    memcpy(newitem, item, vec->elem_size);
    vec->length--;
    return newitem;
}

void vector_free(vector* vec) {
    free(vec->carr);
}