
#ifndef BARE_BONES_EDI_005010X222A1_H
#define BARE_BONES_EDI_005010X222A1_H

#include <stdio.h>
#include "reader.h"

typedef enum {

    X12_OK,
    X12_ILLEGAL_SEGMENT,
    X12_UNKNOWN_RELEASE,

} X12Status;

typedef struct {

    EDISegment* seg;
    FILE* output;
    int inject;
    int skip;
    int loop;

} X12Stream;

#define X12_STREAM_AUTO(X) &(X12Stream) \
{                                       \
    .seg = EDI_COMPOSITE_AUTO(),        \
    .output = X,                        \
    .inject = 0,                        \
    .skip = 0,                          \
    .loop = 0                           \
}

#endif
