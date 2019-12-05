extern "C" {
#include <stdio.h>
extern int exit(int st);
}

#include "ring.h"

slot::slot(int id, int number) {
    thread_id = id;
    value = number;
}

Ring::Ring(int sz) {
    if (sz < 1) {
        fprintf(stderr, "Error: Ring: size %d too small\n", sz);
        exit(1);
    }
    size = sz;
    in = 0;
    out = 0;
    buffer = new slot[size]; //allocate an array of slots.
}

Ring::~Ring() { delete[] buffer; }

void Ring::Put(slot *message) {
    buffer[in].thread_id = message->thread_id;
    buffer[in].value = message->value;
    in = (in + 1) % size;
    // 放入一个新的，那么 empty 肯定为 false， full 看情况
    if (in == out) empty = false, full = true;
    else empty = false, full = false;
}

void Ring::Get(slot *message) {
    message->thread_id = buffer[out].thread_id;
    message->value = buffer[out].value;
    out = (out + 1) % size;
    // 拿走一个，那么 full 肯定为 false ，empty 看情况
    if (in == out) empty = true, full = false;
    else empty = false, full = false;
}

int Ring::Empty() { return empty; }

int Ring::Full() { return full; }


