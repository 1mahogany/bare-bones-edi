
#include <string.h>
#include "005010X222A1.h"

static inline void x12_fwrite_seg(EDISegment* seg, FILE* output) {

    const char* cursor = seg->cursor;
    const size_t n = seg->offset[MAX_N_ELEM - 1] + 1;
    fwrite(cursor, 1, n, output);

}

static inline void x12_fwrite_str(X12Stream* str) {

    if (str->skip) { str->skip = 0; }
    else if (str->inject) { x12_fwrite_seg(str->seg, str->output); }

    if (str->loop < 2400) { str->inject = 0; }
    else if (str->loop > 2420) { str->skip = 1; }
    else { /* still inside service line loop */ }

}

static inline int x12_memcmp_element(EDISegment* seg, int n, char* s) {

    size_t size = strlen(s);
    return edi_memcmp_element(seg, n, s, size);

}

static X12Status x12_parse_segment(EDISegment* seg, X12Stream* str) {

    switch (seg->type) {

    case X12_SEGMENT_GS:

        str->loop = 0;

        if (x12_memcmp_element(seg, 7, "005010X222A1") == 0) { break; }
        else { return X12_UNKNOWN_RELEASE; }

    case X12_SEGMENT_ST:
        
        str->loop = 1000;

        x12_fwrite_str(str);
        break;

    case X12_SEGMENT_NM1:

        if (x12_memcmp_element(seg, 0, "DN") != 0) { break; }
        else if (str->loop >= 2400) { str->skip = 1; break; }
        else if (str->loop <  2300) { return X12_ILLEGAL_SEGMENT; }

        memcpy(str->seg, seg, sizeof(EDISegment));
        str->inject = 1;
        str->skip = 1;
        break;

    case X12_SEGMENT_LQ:

        if (str->loop < 2400) { return X12_ILLEGAL_SEGMENT; }
        else { str->loop = 2440; }

        x12_fwrite_str(str);
        break;

    case X12_SEGMENT_SVD:

        if (str->loop < 2400) { return X12_ILLEGAL_SEGMENT; }
        else { str->loop = 2430; }

        x12_fwrite_str(str);
        break;

    case X12_SEGMENT_LX:

        str->loop = 2400;
        if (str->inject) { str->seg->cursor[5] = 'K'; }

        x12_fwrite_str(str);
        break;

    case X12_SEGMENT_CLM:

        str->loop = 2300;

        x12_fwrite_str(str);
        break;

    case X12_SEGMENT_HL:

        // TODO: use precise loop number
        str->loop = 2000;

        x12_fwrite_str(str);
        break;

    case X12_SEGMENT_SE:

        str->loop = 1000;

        x12_fwrite_str(str);
        break;

    case X12_SEGMENT_GE:

        str->loop = 0;
        break;

    default:
        break;

    }

    x12_fwrite_seg(seg, str->output);
    return X12_OK;

}

static X12Status x12_parse_file(EDIFile* edi, X12Stream* str) {

    EDISegment* seg = NULL;

    while (edi_next_segment(edi, &seg) == EDI_OK) {
        
        X12Status s = x12_parse_segment(seg, str);

        if (s != X12_OK) { return s; }
        else { continue; }
        
    }

    return X12_OK;

}

int main(int argc, char** argv) {

    if (argc < 2) {
        
        printf("missing input file\n");
        return EXIT_FAILURE;
        
    } else {

        EDIFile* edi = EDI_FILE_INIT(argv[1], EDI_X12);
        X12Stream* str = X12_STREAM_AUTO(stdout);
        X12Status s = x12_parse_file(edi, str);
        EDI_FILE_FREE(edi);

        return s;

    }

}
