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

#ifndef BARE_BONES_EDI_SEGMENT_H
#define BARE_BONES_EDI_SEGMENT_H

#define NIL 0
#define EDI_HASH(X, Y, Z) \
    ( (X) << (0 << 3)     \
    | (Y) << (1 << 3)     \
    | (Z) << (2 << 3) )

typedef enum {

    EDI_SEGMENT_ISA = EDI_HASH('I', 'S', 'A'),  // Interchange Control Header
    EDI_SEGMENT_GS  = EDI_HASH('G', 'S', NIL),  // Function Group Header
    EDI_SEGMENT_ST  = EDI_HASH('S', 'T', NIL),  // Transaction Set Header
    EDI_SEGMENT_BPR = EDI_HASH('B', 'P', 'R'),  // Financial Information
    EDI_SEGMENT_TRN = EDI_HASH('T', 'R', 'N'),  // Reassociation Trace Number
    EDI_SEGMENT_CUR = EDI_HASH('C', 'U', 'R'),  // Foreign Currency Information
    EDI_SEGMENT_REF = EDI_HASH('R', 'E', 'F'),  // Additional Identification
    EDI_SEGMENT_DTM = EDI_HASH('D', 'T', 'M'),  // Production Date
    EDI_SEGMENT_N1  = EDI_HASH('N', '1', NIL),  // Entity Identification
    EDI_SEGMENT_N3  = EDI_HASH('N', '3', NIL),  // Entity Address
    EDI_SEGMENT_N4  = EDI_HASH('N' ,'4', NIL),  // Entity City, State, Zip Code
    EDI_SEGMENT_PER = EDI_HASH('P', 'E', 'R'),  // Entity Contact Information
    EDI_SEGMENT_RDM = EDI_HASH('R', 'D', 'M'),  // Remittance Delivery Method
    EDI_SEGMENT_LX  = EDI_HASH('L', 'X', NIL),  // Header Number
    EDI_SEGMENT_TS3 = EDI_HASH('T', 'S', '3'),  // Provider Summary Information
    EDI_SEGMENT_TS2 = EDI_HASH('T', 'S', '2'),  // Provider Supplemental Summary Information
    EDI_SEGMENT_CLP = EDI_HASH('C', 'L', 'P'),  // Claim Payment Information
    EDI_SEGMENT_CAS = EDI_HASH('C', 'A', 'S'),  // Claim Adjustment
    EDI_SEGMENT_NM1 = EDI_HASH('N', 'M', '1'),  // Patient Name
    EDI_SEGMENT_MIA = EDI_HASH('M', 'I', 'A'),  // Inpatient Adjudication Information
    EDI_SEGMENT_MOA = EDI_HASH('M', 'O', 'A'),  // Outpatient Adjudication Information
    EDI_SEGMENT_AMT = EDI_HASH('A', 'M', 'T'),  // Claim Supplemental Information
    EDI_SEGMENT_QTY = EDI_HASH('Q', 'T', 'Y'),  // Claim Supplemental Information Quantity
    EDI_SEGMENT_SVC = EDI_HASH('S', 'V', 'C'),  // Service Payment Information
    EDI_SEGMENT_LQ  = EDI_HASH('L', 'Q', NIL),  // Health Care Remark Code
    EDI_SEGMENT_PLB = EDI_HASH('P', 'L', 'B'),  // Provider Level Adjustment
    EDI_SEGMENT_SE  = EDI_HASH('S', 'E', NIL),  // Transaction Set Trailer
    EDI_SEGMENT_GE  = EDI_HASH('G', 'E', NIL),  // Function Group Trailer
    EDI_SEGMENT_IEA = EDI_HASH('I', 'E', 'A'),  // Interchange Control Trailer
    EDI_SEGMENT_ZZZ = EDI_HASH('Z', 'Z', 'Z'),  // ! Do not use

} EDISegmentType;

#endif
