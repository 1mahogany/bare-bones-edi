
#ifndef BARE_BONES_EDI_005010X221A1_H
#define BARE_BONES_EDI_005010X221A1_H

#include <stdio.h>
#include <stdint.h>
#include "reader.h"

#define MAX_N_COLS 32
#define DELIM_COLS '*'

typedef enum {

    CSV_OK,
    CSV_SKIPPED_SEGMENT,
    CSV_ILLEGAL_SEGMENT,
    CSV_UNKNOWN_RELEASE,

} CSVStatus;

typedef enum {

    CSV_COL_BPR_PAYMENT_DATE,
    CSV_COL_TRN_CHECK_NUMBER,
    CSV_COL_N1_PAYER,
    CSV_COL_N1_PAYEE,
    CSV_COL_N1_NPI,
    CSV_COL_CLP_PCN,
    CSV_COL_CLP_ICN,
    CSV_COL_CLP_MRN,
    CSV_COL_NM1_LAST_NAME,
    CSV_COL_NM1_FIRST_NAME,
    CSV_COL_NM1_MIDDLE_INITIAL,
    CSV_COL_NM1_PATIENT_ID,
    CSV_COL_SVC_PROC_CD,
    CSV_COL_SVC_M1,
    CSV_COL_SVC_M2,
    CSV_COL_SVC_M3,
    CSV_COL_SVC_M4,
    CSV_COL_SVC_BILLED_AMT,
    CSV_COL_SVC_PAID_AMT,
    CSV_COL_SVC_ALLOW_UNITS,
    CSV_COL_DTM_DATE_FROM,
    CSV_COL_DTM_DATE_TO,
    CSV_COL_CAS_GROUP_CODE,
    CSV_COL_CAS_REASON_CODE,
    CSV_COL_CAS_AMOUNT,
    CSV_COL_REF_LINE_ITEM,
    CSV_COL_LQ_REMARK_CODE_1,
    CSV_COL_LQ_REMARK_CODE_2,
    CSV_COL_LQ_REMARK_CODE_3,
    CSV_COL_LQ_REMARK_CODE_4,
    CSV_COL_ZZZ,

} CSVCol;

typedef struct {

    char buffer[916];
    uint8_t  length[MAX_N_COLS];
    uint16_t offset[MAX_N_COLS];

    uint32_t remark;
    FILE* output;

} CSVRow;

#define CSV_ROW_AUTO(X) &(CSVRow)           \
{                                           \
    .buffer = {0},                          \
    .length = {0},                          \
    .offset = {                             \
        [CSV_COL_BPR_PAYMENT_DATE]   =   0, \
        [CSV_COL_TRN_CHECK_NUMBER]   =   9, \
        [CSV_COL_N1_PAYER]           =  60, \
        [CSV_COL_N1_PAYEE]           = 121, \
        [CSV_COL_N1_NPI]             = 182, \
        [CSV_COL_CLP_PCN]            = 263, \
        [CSV_COL_CLP_ICN]            = 302, \
        [CSV_COL_CLP_MRN]            = 353, \
        [CSV_COL_NM1_LAST_NAME]      = 354, \
        [CSV_COL_NM1_FIRST_NAME]     = 415, \
        [CSV_COL_NM1_MIDDLE_INITIAL] = 451, \
        [CSV_COL_NM1_PATIENT_ID]     = 477, \
        [CSV_COL_SVC_PROC_CD]        = 558, \
        [CSV_COL_SVC_M1]             = 607, \
        [CSV_COL_SVC_M2]             = 610, \
        [CSV_COL_SVC_M3]             = 613, \
        [CSV_COL_SVC_M4]             = 616, \
        [CSV_COL_SVC_BILLED_AMT]     = 619, \
        [CSV_COL_SVC_PAID_AMT]       = 638, \
        [CSV_COL_SVC_ALLOW_UNITS]    = 657, \
        [CSV_COL_DTM_DATE_FROM]      = 673, \
        [CSV_COL_DTM_DATE_TO]        = 682, \
        [CSV_COL_CAS_GROUP_CODE]     = 691, \
        [CSV_COL_CAS_REASON_CODE]    = 694, \
        [CSV_COL_CAS_AMOUNT]         = 700, \
        [CSV_COL_REF_LINE_ITEM]      = 719, \
        [CSV_COL_LQ_REMARK_CODE_1]   = 770, \
        [CSV_COL_LQ_REMARK_CODE_2]   = 801, \
        [CSV_COL_LQ_REMARK_CODE_3]   = 832, \
        [CSV_COL_LQ_REMARK_CODE_4]   = 863, \
        [CSV_COL_ZZZ]                = 894, \
    },                                      \
    .remark = 0,                            \
    .output = X                             \
}

#endif
