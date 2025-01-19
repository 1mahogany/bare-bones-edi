
#include <stdio.h>
#include "csv.h"

int main(int argc, char** argv) {

    if (argc < 2) {
        
        printf("missing input file\n");
        return EXIT_FAILURE;
        
    } else {

        EDIFile* edi = EDI_FILE_INIT(argv[1], EDI_X12);
        csv_parse_file(edi, stdout);
        EDI_FILE_FREE(edi);

        return EXIT_SUCCESS;

    }

}
