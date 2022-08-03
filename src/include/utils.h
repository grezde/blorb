#pragma once

char* read_file(char* filename);

typedef struct {
    int length;
    int buff_size;
    char* cstr;
} string;

/*
typedef struct {
    int length;
    int buff_size;
    int elem_size;
    void* carr;
} vector;

vector vector_new(int elem_size);
void vector_allocate(vector* vec, int length);
void vector_free(vector* vec);
void vector_push(vector* vec, void* elem);
*/

string string_new();
string string_from_lit(const char* s);
void string_allocate(string* str, int length);
void string_free(string* s);

void string_pushc(string* str, const char c);
void string_pushs(string* str, const char* s);
