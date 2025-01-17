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

#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "x12.h"

#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

static EDIStatus edi_set_delim(EDIFile* edi) {

    // automatic detection requires standard ISA segment (106 bytes)
    // files with fewer than 106 bytes are invalid, cannot proceed

    if (edi->n_bytes > 105) {

        EDISegment* s = edi->seg;
        const char* b = edi->buffer;

        s->delim_elm = b[103];
        s->delim_sub = b[104];
        s->delim_seg = b[105];

        // set starting cursor position
        s->cursor = edi->buffer - 1;
        return EDI_OK;

    } else { return EDI_INVALID_FILE; }

}

static EDIStatus edi_read_file(EDIFile* edi) {

    // do nothing if EDIFile is already initialized
    if (edi->status != EDI_UNINITIALIZED) { return edi->status; }

    // if EDIFile uses a reference buffer, set delimiters only
    if (edi->n_bytes) { return edi->status = edi_set_delim(edi); }

    // before initialization, edi->buffer stores the file name
    FILE* f = fopen(edi->buffer, "r");

    if (f) {

        // TODO: error checking on stat?
        struct stat s;
        stat(edi->buffer, &s);
        edi->n_bytes = (size_t)s.st_size;

        edi->buffer = malloc(edi->n_bytes);

    } else { return edi->status = EDI_INVALID_FILE; }

    if (edi->buffer) {

        // TODO: check bytes read and feof(f)
        fread(edi->buffer, 1, edi->n_bytes, f);
        edi->status = edi_set_delim(edi);

    } else { edi->status = EDI_MALLOC_FAILED; }

    // TODO: error checking on fclose?
    fclose(f);
    return edi->status;

}

EDIStatus edi_next_segment(EDIFile* edi, EDISegment** seg) {

    EDISegment* s = edi->seg;
    if (edi_read_file(edi) != EDI_OK) { return edi->status; }
    else { s->cursor += s->offset[MAX_N_ELEM - 1] + 1; }

    const int64_t n = edi->n_bytes - (s->cursor - edi->buffer);

    if (n > 0) { s->type = 0; }
    else { return edi->status = EDI_EOF; }

    int64_t i = 0;
    int64_t e = 0;

    for (; i < 4; i++) { // register the segment type

        char c = s->cursor[i];

        if (c == s->delim_elm || c == s->delim_seg) { break; }
        else { s->type |= c << (i << 3); }

    }

    for (; i < n && e < MAX_N_ELEM; i++) { // register the element offsets

        char c = s->cursor[i];

        if (c == s->delim_elm) { s->offset[e] = i; e++; }
        else if (c == s->delim_seg) { break; }

    }

    // Fill any unused offsets
    for (; e < MAX_N_ELEM; e++) { s->offset[e] = i; }

    *seg = s;
    return EDI_OK;

}

EDIStatus edi_seek_segment(EDIFile* edi, EDISegment** seg, EDISegmentType type) {

    while (edi_next_segment(edi, seg) == EDI_OK) {

        if (edi->seg->type == type) { return EDI_OK; }
        else { /* no match, continue searching */ }

    }

    // if no match found, EOF is reached and returned
    // if EDIFile has any existing error, it will be returned as is.
    return edi->status;

}

EDIStatus edi_get_composite(EDISegment* seg, EDIComposite* comp, int n) {

    if (n >= 0 && n < MAX_N_ELEM - 1) {
        
        comp->delim_elm = seg->delim_elm;
        comp->delim_sub = seg->delim_sub;
        comp->delim_seg = seg->delim_seg;
        comp->type = seg->type;
        
    } else { return EDI_INVALID_INDEX; }

    const int64_t n_bytes = seg->offset[n + 1] - seg->offset[n];

    if (n_bytes > 0) {

        comp->cursor = seg->cursor + seg->offset[n];
        comp->offset[0] = 0;

    } else { return EDI_INVALID_INDEX; }

    // offset[0] is always 0, no need to include in the loop
    int64_t i = 1;
    int64_t e = 1;

    for (; i < n_bytes && e < MAX_N_ELEM; i++) {

        const bool b = comp->cursor[i] == comp->delim_sub;
        if (b) { comp->offset[e] = i; e++; }

    }

    // Fill any unused offsets
    for (; e < MAX_N_ELEM; e++) { comp->offset[e] = i; }
    return EDI_OK;

}

EDIStatus edi_get_component(EDISegment* seg, EDIComponent* comp, int n) {

    if (n >= 0 && n < MAX_N_ELEM - 1) {

        comp->cursor = seg->cursor + seg->offset[n];
        comp->n_bytes = seg->offset[n + 1] - seg->offset[n];

        if (comp->n_bytes) { return EDI_OK; }
        else { return EDI_INVALID_INDEX; }

    } else { return EDI_INVALID_INDEX; }

}

int edi_memcpy_element(EDISegment* seg, int n, void* buffer, size_t s) {

    EDIComponent* comp = EDI_COMPONENT_AUTO();
    EDIStatus status = edi_get_component(seg, comp, n);

    if (status == EDI_OK) {

        const char* c = comp->cursor + 1;
        size_t n_bytes = MIN(s, comp->n_bytes - 1);

        memcpy(buffer, c, n_bytes);
        return n_bytes;

    } else { return -1; } // error signal

}

int edi_memcmp_element(EDISegment* seg, int n, void* buffer, size_t s) {

    EDIComponent* comp = EDI_COMPONENT_AUTO();
    EDIStatus status = edi_get_component(seg, comp, n);

    if (status == EDI_OK) {

        const char* c = comp->cursor + 1;

        if (s != comp->n_bytes - 1)    { return 1; }
        else if (memcmp(buffer, c, s)) { return 1; }
        else /* the 2 buffers match */ { return 0; }

    } else { return -1; } // error signal

}
