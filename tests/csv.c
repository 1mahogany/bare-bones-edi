
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "csv.h"

static const int csv_col_element[] = {

    [CSV_COL_BPR_PAYMENT_DATE]   = 15,
    [CSV_COL_TRN_CHECK_NUMBER]   =  1,
    [CSV_COL_N1_PAYER]           =  1,
    [CSV_COL_N1_PAYEE]           =  1,
    [CSV_COL_N1_NPI]             =  3,
    [CSV_COL_CLP_PCN]            =  0,
    [CSV_COL_CLP_ICN]            =  6,
    [CSV_COL_CLP_MRN]            = 31,
    [CSV_COL_NM1_LAST_NAME]      =  2,
    [CSV_COL_NM1_FIRST_NAME]     =  3,
    [CSV_COL_NM1_MIDDLE_INITIAL] =  4,
    [CSV_COL_NM1_PATIENT_ID]     =  8,
    [CSV_COL_SVC_PROC_CD]        =  1,
    [CSV_COL_SVC_M1]             =  2,
    [CSV_COL_SVC_M2]             =  3,
    [CSV_COL_SVC_M3]             =  4,
    [CSV_COL_SVC_M4]             =  5,
    [CSV_COL_SVC_BILLED_AMT]     =  1,
    [CSV_COL_SVC_PAID_AMT]       =  2,
    [CSV_COL_SVC_ALLOW_UNITS]    =  4,
    [CSV_COL_DTM_DATE_FROM]      =  1,
    [CSV_COL_DTM_DATE_TO]        =  1,
    [CSV_COL_CAS_GROUP_CODE]     =  0,
    [CSV_COL_CAS_REASON_CODE]    =  1,
    [CSV_COL_CAS_AMOUNT]         =  2,
    [CSV_COL_REF_LINE_ITEM]      =  1,
    [CSV_COL_LQ_REMARK_CODE_1]   =  1,
    [CSV_COL_LQ_REMARK_CODE_2]   =  1,
    [CSV_COL_LQ_REMARK_CODE_3]   =  1,
    [CSV_COL_LQ_REMARK_CODE_4]   =  1,
    [CSV_COL_ZZZ]                = 31,

};

static inline void csv_fwrite_row(CSVRow* row) {

    for (CSVCol col = 0; col < CSV_COL_ZZZ; col++) {

        const char* cursor = row->buffer + row->offset[col];
        const uint8_t length = row->length[col] + 1;
        fwrite(cursor, 1, length, row->output);

    }

}

static int csv_memcpy_element(EDISegment* seg, CSVRow* row, CSVCol col) {

    //* when memcpy requests a backwards column - render row first
    //* row->length[CSV_COL_ZZZ] stores the column counter/cursor
    if (col < row->length[CSV_COL_ZZZ]) {
        
        row->remark = 0;
        csv_fwrite_row(row);
        row->length[CSV_COL_ZZZ] = (uint8_t)col;
        for (CSVCol i = col; i < CSV_COL_ZZZ; i++) { row->length[i] = 0; }

    } else { row->length[CSV_COL_ZZZ] = (uint8_t)col; }

    uint32_t s = row->offset[col + 1] - row->offset[col];
    char* c = row->buffer + row->offset[col] + 1;
    
    int n_bytes = edi_memcpy_element(seg, csv_col_element[col], c, s);

    if (n_bytes < 0) { return -1; }
    else { return row->length[col] = n_bytes; }

}

static CSVStatus csv_parse_segment(EDISegment* seg, CSVRow* row) {

    switch (seg->type) {

    case EDI_SEGMENT_GS:
    {
        const int r = edi_memcmp_element(seg, 7, "005010X221A1", 12);
        return r == 0 ? CSV_SKIPPED_SEGMENT : CSV_UNKNOWN_RELEASE;
    }
    case EDI_SEGMENT_BPR:

        csv_memcpy_element(seg, row, CSV_COL_BPR_PAYMENT_DATE);
        return CSV_OK;

    case EDI_SEGMENT_TRN:

        csv_memcpy_element(seg, row, CSV_COL_TRN_CHECK_NUMBER);
        return CSV_OK;

    case EDI_SEGMENT_N1:

        if (edi_memcmp_element(seg, 0, "PR", 2) == 0) {

            csv_memcpy_element(seg, row, CSV_COL_N1_PAYER);
            return CSV_OK;

        } else if (edi_memcmp_element(seg, 0, "PE", 2) == 0) {

            csv_memcpy_element(seg, row, CSV_COL_N1_PAYEE);
            csv_memcpy_element(seg, row, CSV_COL_N1_NPI);
            return CSV_OK;

        } else { return CSV_SKIPPED_SEGMENT; }

    case EDI_SEGMENT_CLP:

        csv_memcpy_element(seg, row, CSV_COL_CLP_PCN);
        csv_memcpy_element(seg, row, CSV_COL_CLP_ICN);
        csv_memcpy_element(seg, row, CSV_COL_CLP_MRN);
        return CSV_OK;

    case EDI_SEGMENT_NM1:
        
        if (edi_memcmp_element(seg, 0, "QC", 2) == 0) {

            csv_memcpy_element(seg, row, CSV_COL_NM1_LAST_NAME);
            csv_memcpy_element(seg, row, CSV_COL_NM1_FIRST_NAME);
            csv_memcpy_element(seg, row, CSV_COL_NM1_MIDDLE_INITIAL);
            csv_memcpy_element(seg, row, CSV_COL_NM1_PATIENT_ID);
            return CSV_OK;

        } else { return CSV_SKIPPED_SEGMENT; }

    case EDI_SEGMENT_SVC:
    {
        EDIComposite* comp = EDI_COMPOSITE_AUTO();

        if (edi_get_composite(seg, comp, 0) == EDI_OK) {

            csv_memcpy_element(comp, row, CSV_COL_SVC_PROC_CD);
            csv_memcpy_element(comp, row, CSV_COL_SVC_M1);
            csv_memcpy_element(comp, row, CSV_COL_SVC_M2);
            csv_memcpy_element(comp, row, CSV_COL_SVC_M3);
            csv_memcpy_element(comp, row, CSV_COL_SVC_M4);

        } else { return CSV_ILLEGAL_SEGMENT; }

        csv_memcpy_element(seg, row, CSV_COL_SVC_BILLED_AMT);
        csv_memcpy_element(seg, row, CSV_COL_SVC_PAID_AMT);
        csv_memcpy_element(seg, row, CSV_COL_SVC_ALLOW_UNITS);
        return CSV_OK;
    }
    case EDI_SEGMENT_DTM:

        if (edi_memcmp_element(seg, 0, "472", 3) == 0) {

            csv_memcpy_element(seg, row, CSV_COL_DTM_DATE_FROM);
            csv_memcpy_element(seg, row, CSV_COL_DTM_DATE_TO);
            return CSV_OK;

        } else if (edi_memcmp_element(seg, 0, "150", 3) == 0) {

            csv_memcpy_element(seg, row, CSV_COL_DTM_DATE_FROM);
            return CSV_OK;

        } else if (edi_memcmp_element(seg, 0, "151", 3) == 0) {

            csv_memcpy_element(seg, row, CSV_COL_DTM_DATE_TO);
            return CSV_OK;

        } else { return CSV_SKIPPED_SEGMENT; }

    case EDI_SEGMENT_CAS:

        csv_memcpy_element(seg, row, CSV_COL_CAS_GROUP_CODE);
        csv_memcpy_element(seg, row, CSV_COL_CAS_REASON_CODE);
        csv_memcpy_element(seg, row, CSV_COL_CAS_AMOUNT);
        return CSV_OK;

    case EDI_SEGMENT_REF:

        if (edi_memcmp_element(seg, 0, "6R", 2) == 0) {

            csv_memcpy_element(seg, row, CSV_COL_REF_LINE_ITEM);
            return CSV_OK;

        } else { return CSV_SKIPPED_SEGMENT; }

    case EDI_SEGMENT_LQ:

        if (edi_memcmp_element(seg, 0, "HE", 2) == 0) {

            //* when there are more than 4 remarks (consecutive LQ segments),
            //* additional LQ segments will overflow to ZZZ and be discarded
            CSVCol col = CSV_COL_LQ_REMARK_CODE_1 + MIN(row->remark, 4);
            csv_memcpy_element(seg, row, col);
            row->remark++;
            return CSV_OK;

        } else { return CSV_SKIPPED_SEGMENT; }

    case EDI_SEGMENT_GE:
    {
        // end of functional group loop
        return CSV_SKIPPED_SEGMENT;
    }
    default:
        return CSV_SKIPPED_SEGMENT;

    }

}

int csv_parse_file(EDIFile* edi, FILE* out) {

    static_assert(CSV_COL_ZZZ < MAX_N_COLS);
    static_assert(sizeof(CSVRow) == 1024UL);

    CSVRow* row = CSV_ROW_AUTO(out);
    EDISegment* seg = NULL;

    row->buffer[0] = '\n';

    for (CSVCol col = 1; col < CSV_COL_ZZZ; col++) {

        const uint16_t offset = row->offset[col];
        row->buffer[offset] = DELIM_COLS;
        
    }

    while (edi_next_segment(edi, &seg) == EDI_OK) {
        
        CSVStatus s = csv_parse_segment(seg, row);
        if (s != CSV_UNKNOWN_RELEASE) { continue; }
        else { edi_seek_segment(edi, &seg, EDI_SEGMENT_GE); }
        
    }

    csv_fwrite_row(row);
    return 0;

}
