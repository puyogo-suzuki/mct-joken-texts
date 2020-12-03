#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct buffer {
    char * buf;  // buffer
    int len; // length
    int cap; // capacity
};

#define BUFFER_EXTEND_SIZE 256

int newString(char * content, int size, struct buffer * result) {
    char * buf = (char *)malloc(size * 2);
    if(buf == NULL) return 0;
    memcpy(buf, content, size);
    result->buf = buf;
    result->len = size;
    result->cap = size * 2;
    return 1;
}

int insertString(struct buffer * me, char * content, int start, int size) {
    if(me->cap - me->len < size) {
        int new_cap = me->cap + BUFFER_EXTEND_SIZE;
        while(new_cap < me->len + size)
            new_cap += BUFFER_EXTEND_SIZE;
        char * new_buf = (char *)realloc(me->buf, new_cap);
        if(new_buf != NULL) {
            me->buf = new_buf;
            me->cap = new_cap;
        } else {
            new_buf = (char *)malloc(new_cap);
            if(new_buf != NULL) {
                memcpy(new_buf, me->buf, me->len);
                free(me->buf);
                me->buf = new_buf;
                me->cap = new_cap;
            } else {
                return 0;
            }
        }
    }
    start = start >= me->len ? me->len : start;
    for(int i = me->len + size - 1; i >= start + size; --i)
        me->buf[i] = me->buf[i-size];
    memcpy(me->buf + start, content, size);
    me->len += size;
    return 1;
}

void removeString(struct buffer * me, int start, int size) {
    if(start >= me->len) return;
    int end = size + start;
    if(end > me->len) end = me->len;
    for(int i = 0; end + i < me->len; ++i) {
        me->buf[i + start] = me->buf[end + i];
    }
    me->len -= (end-start);
}

int isNum(char c) {
    return c >= '0' && c <= '9';
}

int parse(char * buf, int * start, int * end) {
    buf = buf + 1;
    int num_tail_pos = 0;
    int cont = 0;
    for(; buf[num_tail_pos] != '\n'; ++num_tail_pos) {
        if(buf[num_tail_pos] == ',') {
            cont = 1;
            break;
        }
        if(!isNum(buf[num_tail_pos])) return 0;
    }
    if(sscanf(buf, "%d", start) == EOF) return 0;
    if(!cont) {
        *end = *start + 1;
        return 1;
    }
    buf = buf + num_tail_pos + 1;
    for(; buf[num_tail_pos] != '\n'; ++num_tail_pos) {
        if(!isNum(buf[num_tail_pos])) return 0;
    }
    if(sscanf(buf, "%d", end) == EOF) return 0;
    return 1;
}

int openFile(struct buffer * buf, char * path) {
    FILE * fp = fopen(path, "r");
    char tmp[128];
    if(fp == NULL) {
        printf("Error:Cannot read %s.\n", path);
        return 0;
    }
    char * ret = fgets(tmp, sizeof(tmp), fp);
    if(!newString(tmp, strlen(tmp), buf)) return 0;
    if(ret == NULL) return 1;
    while(1) {
        ret = fgets(tmp, sizeof(tmp), fp);
        if(ret == NULL) break;
        insertString(buf, tmp, buf->len, strlen(tmp));
    }
    return 1;
}

int writeFile(struct buffer * buf, char * path) {
    FILE * fp = fopen(path, "w");
    if(fp == NULL) {
        printf("Error:Cannot open %s.\n", path);
        return 0;
    }
    if(fwrite(buf, sizeof(char), buf->len, fp) != buf->len) {
        printf("Error:Cannot write %s.\n", path);
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;
}

void printLines(struct buffer * buf, int start, int end) {
    int lfCount = 0;
    for(int i = 0; i < buf->len; ++i) {
        if(lfCount >= start)
            putc(buf->buf[i], stdout);
        if(buf->buf[i] == '\n') lfCount++;
        if(lfCount >= end) return;
    }
}

int appendLines(struct buffer * buf, int start, int end) {
    int lfCount = 0;
    int i = 0;
    char tmp[1024];
    for(; i < buf->len; ++i) {
        if(buf->buf[i] == '\n') lfCount++;
        if(lfCount == start+1) break;
    }
    i++;
    while(1) {
        if(fgets(tmp, sizeof(tmp), stdin) == NULL) return 1;
        if(tmp[0] == '.') return 1;
        int size = strlen(tmp);
        int retVal = insertString(buf, tmp, i, size);
        if(retVal == 0) return 0;
        i += size;
    }
    return 1;
}

void removeLines(struct buffer * buf, int start, int end) {
    int lfCount = 0;
    int st_pos = 0, en_pos = 0;
    for(int i = 0; i < buf->len; ++i) {
        if(buf->buf[i] == '\n') {
            lfCount++;
            if(start == lfCount) st_pos = i+1;
            if(end == lfCount) en_pos = i+1;
        }
    }
    if(en_pos == 0) en_pos = buf->len;
    removeString(buf, st_pos, en_pos-st_pos);
}

int main(int argc, char ** argv) {
    struct buffer textBuffer;

    if(argc == 1) {
        printf("Usage: %s [file name].", argv[0]);
        return 1;
    }
    if(!openFile(&textBuffer, argv[1])) return 1;

    while(1) {
        char tmp[128];
        printf("> ");
        fgets(tmp, sizeof(tmp), stdin);
        if(tmp[0] == '\n') {
            continue;
        }
        int start, end;
        if(!parse(tmp, &start, &end)) {
            switch(tmp[0]) {
                case 'w':
                    writeFile(&textBuffer, argv[1]);
                    break;
                case 'q':
                    exit(0);
            }
        }
        switch(tmp[0]) {
            case 'p':
                printLines(&textBuffer, start, end);
                break;
            case 'a':
                if(!appendLines(&textBuffer, start, end)) {
                    puts("malloc error.");
                    exit(1);
                }
                break;
            case 'r':
                removeLines(&textBuffer, start, end);
                break;
        }
    }
}