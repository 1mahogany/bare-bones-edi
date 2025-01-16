/**
 * Bare Bones EDI: A bare-bones C library for EDI files
 * Copyright (C) 2025 Ian Wang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef BARE_BONES_EDI_READER_H
#define BARE_BONES_EDI_READER_H

#include <stdint.h>
#include <stdlib.h>

#include "segment.h"

#define MAX_N_ELEM 32

typedef enum {

    EDI_OK,
    EDI_EOF,
    EDI_INVALID_FILE,
    EDI_INVALID_INDEX,
    EDI_UNINITIALIZED,
    EDI_MALLOC_FAILED,

    EDI_MEMCMP_T,
    EDI_MEMCMP_F,

} EDIStatus;

typedef struct {

    char* cursor;
    size_t n_bytes;

} EDIElement;

typedef struct {

    char* cursor;
    uint16_t offset[MAX_N_ELEM];
    EDISegmentType type;

    char delim_elm;
    char delim_sub;
    char delim_seg;

} EDISegment;

typedef struct {

    char* buffer;
    size_t n_bytes;

    EDISegment* seg;
    EDIStatus status;
    int delim_is_set;

} EDIFile;

typedef EDISegment EDIComposite;
typedef EDIElement EDIComponent;

EDIStatus edi_set_delim(EDIFile* edi, char elm, char sub, char seg);

EDIStatus edi_next_segment(EDIFile* edi, EDISegment** seg);
EDIStatus edi_seek_segment(EDIFile* edi, EDISegment** seg, EDISegmentType type);
EDIStatus edi_get_composite(EDISegment* seg, EDIComposite* comp, int n);
EDIStatus edi_get_component(EDISegment* seg, EDIComponent* comp, int n);

int edi_memcpy_element(EDISegment* seg, int n, void* buffer, size_t s);
int edi_memcmp_element(EDISegment* seg, int n, void* buffer, size_t s);
void edi_file_free(EDIFile* edi);

#define EDI_COMPONENT_AUTO() &(EDIComponent) \
{                                            \
    .cursor = NULL,                          \
    .n_bytes = 0                             \
}

#define EDI_COMPOSITE_AUTO() &(EDIComposite) \
{                                            \
    .cursor = NULL,                          \
    .offset = {0},                           \
    .type = EDI_SEGMENT_ZZZ                  \
}

// Initialize a new buffer from reading a file
#define EDI_FILE_INIT(X)    &(EDIFile) \
{                                      \
    .buffer = X,                       \
    .n_bytes = 0,                      \
    .seg = EDI_COMPOSITE_AUTO(),       \
    .status = EDI_UNINITIALIZED,       \
    .delim_is_set = 0                  \
}

// Reference an existing buffer that stores an EDI file
#define EDI_FILE_AUTO(X, Y) &(EDIFile) \
{                                      \
    .buffer = X,                       \
    .n_bytes = Y,                      \
    .seg = EDI_COMPOSITE_AUTO(),       \
    .status = EDI_UNINITIALIZED,       \
    .delim_is_set = 0                  \
}

#define EDI_FILE_FREE(X) edi_file_free(X)

#endif // BARE_BONES_EDI_READER_H
