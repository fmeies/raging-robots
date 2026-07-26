#ifndef CG_PPM_READER
#define CG_PPM_READER

extern unsigned char* read_ppm_file (
    const char* filename, 
    int* width,
    int* height
);

/* returns a pointer to an alloced array of bytes
   which contains the PPM data from the given file */

#endif

