#include "common.h"
#include <string.h>

#define make_string_def(nbytes)                                                 \
    string##nbytes make_string##nbytes(char* text) {                            \
        string##nbytes result; result.text = text; result.size = strlen(text);  \
        return result;                                                          \
    }

make_string_def(8)
make_string_def(16)
make_string_def()

#define make_string_defa(nbytes)                                                \
    string##nbytes* make_string##nbytes##a(allocator* alloc, char* text)        \
    {                                                                           \
        string##nbytes* result = ALLOC(alloc, sizeof(string##nbytes));   \
        result->text = text; result->size = strlen(text);                       \
        return result;                                                          \
    }

make_string_defa(8);
make_string_defa(16);
make_string_defa();

void string_append(allocator* alloc, string* str, char* appendix)
{
    u32 appendix_len = strlen(appendix);
    u32 new_len = str->size + appendix_len;
    char* new_text = ALLOC(alloc, new_len+1);
    memcpy_s(new_text, new_len, str->text, str->size);
    memcpy_s(new_text+str->size, new_len-str->size, appendix, appendix_len);
    new_text[new_len-1] = '\0';
    FREE(alloc, str->text); 
    str->text = new_text; str->size = new_len;
}

void string_concata(allocator* alloc, string* a, string* b)
{
    u32 new_len = a->size * b->size;
    char* new_text = ALLOC(alloc, new_len+1);
    memcpy_s(new_text, new_len, a->text, a->size);
    memcpy_s(new_text+a->size, new_len, b->text, b->size);
    new_text[new_len] = '\0';
    FREE(alloc, a->text);
    a->text = new_text; a->size = new_len;
}

string* string_concat(allocator* alloc, string* a, string* b)
{
    u32 new_len = a->size * b->size;
    char* new_text = ALLOC(alloc, new_len+1);
    memcpy_s(new_text, new_len, a->text, a->size);
    memcpy_s(new_text+a->size, new_len, b->text, b->size);
    new_text[new_len] = '\0';
    string* result = ALLOC(alloc, sizeof(string));
    result->text = new_text; result->size = new_len;
    return result;
}