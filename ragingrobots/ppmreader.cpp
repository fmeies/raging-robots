#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

/* Reader over the raw file contents. Every read is bounded by end, and the
   first thing that does not add up sets failed, so a truncated or corrupt
   file makes the whole load fail instead of walking off the buffer. */
typedef struct {
    const unsigned char* next;
    const unsigned char* end;
    int failed;
} Reader;

/* largest image accepted, to keep width*height*depth inside an int */
#define MAX_DIMENSION 16384

static int exhausted (const Reader* reader) {
    return reader->next >= reader->end;
}

/* skip spaces and line-comments in file */
static void skipSpace (Reader* reader) {
    while (!exhausted (reader)) {

        /* skip normal spaces*/
        if (isspace (*reader->next)) {
            reader->next++;
            continue;
        }

        /* skip line-comments*/
        if (*reader->next == '#') {
            while (!exhausted (reader) && (*reader->next != '\n')) {
                reader->next++;
            }
            if (!exhausted (reader)) {
                reader->next++;
            }
            continue;
        }
        break;
    }
}

/* read next integer from file. return -1 and mark the reader on failure */
static int readInteger (Reader* reader) {
    int result=0;

    /* check for a digit*/
    skipSpace (reader);
    if (exhausted (reader) || !isdigit (*reader->next)) {
        reader->failed = 1;
        return -1;
    }

    /* read all following digits */
    do {
        if (result > (INT_MAX - 9) / 10) {
            reader->failed = 1;
            return -1;
        }
        result *= 10;
        result += (*reader->next - '0');
        reader->next++;
    } while (!exhausted (reader) && isdigit (*reader->next));

    return result;
}

/* read one raw byte. return 0 and mark the reader once the data runs out */
static int readByte (Reader* reader) {
    if (exhausted (reader)) {
        reader->failed = 1;
        return 0;
    }
    return *reader->next++;
}

/* black and white, ascii */
static void readType1 (int size, unsigned char* pixmap, Reader* reader) {
    int i;
    for (i=0; i<size && !reader->failed; i++) {
        if (readInteger (reader)) {
            *(pixmap++) = 0;
        } else {
            *(pixmap++) = 255;
        }
    }
}

/* grey-scale, ascii */
static void readType2 (int size, unsigned char* pixmap, Reader* reader, const unsigned char* palette) {
    int i;
    for (i=0; i<size && !reader->failed; i++) {
        int value = readInteger (reader);
        *(pixmap++) = reader->failed ? 0 : palette[value & 0xff];
    }
}

/* true-color, ascii */
static void readType3 (int size, unsigned char* pixmap, Reader* reader, const unsigned char* palette) {
    int i;
    for (i=0; i<size && !reader->failed; i++) {
        int red   = readInteger (reader);
        int green = readInteger (reader);
        int blue  = readInteger (reader);
        if (reader->failed) {
            break;
        }
        pixmap[0] = palette[red   & 0xff];
        pixmap[1] = palette[green & 0xff];
        pixmap[2] = palette[blue  & 0xff];
        pixmap += 3;
    }
}

/* black and white, binary */
static void readType4 (int width, int height, unsigned char* pixmap, Reader* reader) {
    unsigned char byte=0;
    int i, j, left = 0;

    for (i=0; i<height && !reader->failed; i++) {
        for (j=0; j<width && !reader->failed; j++) {
            if (!left) {
                byte = (unsigned char) readByte (reader);
                left = 8;
            }
            if (byte & 0x80) {
                *(pixmap++) = 0;
            } else {
                *(pixmap++) = 255;
            }
            byte <<= 1;
            left--;
        }
        left = 0;
    }
}

/* grey-scale, binary */
static void readType5 (int size, unsigned char* pixmap, Reader* reader, const unsigned char* palette) {
    int i;
    for (i=0; i<size && !reader->failed; i++) {
        *(pixmap++) = palette[readByte (reader)];
    }
}

/* true-color, binary */
static void readType6 (int size, unsigned char* pixmap, Reader* reader, const unsigned char* palette) {
    int i;
    for (i=0; i<size && !reader->failed; i++) {
        pixmap[0] = palette[readByte (reader)];
        pixmap[1] = palette[readByte (reader)];
        pixmap[2] = palette[readByte (reader)];
        pixmap += 3;
    }
}

/* 3-3-2 true-color, binary - has not been tested, might be buggy */
static void readType7 (int size, unsigned char* pixmap, Reader* reader, const unsigned char* palette) {
    int i;
    for (i=0; i<size && !reader->failed; i++) {
        int entry = 3 * readByte (reader);
        pixmap[0] = palette[entry+0];
        pixmap[1] = palette[entry+1];
        pixmap[2] = palette[entry+2];
        pixmap += 3;
    }
}

/* Builds a 256-entry ramp. Filling every entry rather than range+1 of them
   means a byte above the declared maximum cannot index past the end. */
static unsigned char* makeRamp (int range) {
    unsigned char* palette = (unsigned char*) malloc (0x100);
    int i;

    if (!palette) {
        return 0;
    }
    for (i=0; i<0x100; i++) {
        palette[i] = (unsigned char) (255 * (i < range ? i : range) / range);
    }
    return palette;
}

static unsigned char* makePackedPalette (void) {
    unsigned char* palette = (unsigned char*) malloc (3 * 0x100);
    int i;

    if (!palette) {
        return 0;
    }
    for (i=0; i<0x100; i++) {
        palette[3*i+0] = 255 * ((i & 0xe0) >> 5) / 7;
        palette[3*i+1] = 255 * ((i & 0x1c) >> 2) / 7;
        palette[3*i+2] = 255 * ((i & 0x03) >> 0) / 3;
    }
    return palette;
}

unsigned char* do_read (unsigned char* data, int len, int* twidth, int* theight) {
    unsigned char* pixmap;
    unsigned char* palette;
    Reader reader;
    int range;
    int type, width, height, size, depth;

    if ((len < 3) || (data[0]!='P') || !strchr ("1234567", data[1]) ||
        !isspace(data[2])) {
        fprintf (stderr, "ppmreader: not a PPM file\n");
        return 0;
    }

    reader.next = data + 3;
    reader.end = data + len;
    reader.failed = 0;
    type = data[1]-'0';

    /* I don't know what this number is for */
    if (type==7) {
        readInteger (&reader);
    }

    /* read image-size */
    width = readInteger (&reader);
    height = readInteger (&reader);
    if (reader.failed || width <= 0 || height <= 0 ||
        width > MAX_DIMENSION || height > MAX_DIMENSION) {
        fprintf (stderr, "ppmreader: bad image size %dx%d\n", width, height);
        return 0;
    }
    *twidth = width;
    *theight = height;
    size = width*height;

    /* read or determine color range */
    if ((type == 1) || (type == 4)) {
        range = 1;
    } else {
        range = readInteger (&reader);
    }

    /* kludge to handle files written by xloadimage */
    if (range == 257) {
        range = 255;
    }
    if (reader.failed || range < 1 || range > 0xff) {
        fprintf (stderr, "ppmreader: bad colour range %d\n", range);
        return 0;
    }

    /* create an image */
    depth = (type==3) || (type==6) || (type==7) ? 3 : 1;

    pixmap = (unsigned char*) malloc( size*depth );
    if (!pixmap) {
        fprintf (stderr, "ppmreader: out of memory for %dx%d\n", width, height);
        return 0;
    }

    palette = 0;

    /* these types are grey-scale / true-color */
    if ((type == 2) || (type == 5) ||
        (type == 3) || (type == 6)) {
        palette = makeRamp (range);
    }

    /* this is 3-3-2 packed true-color */
    if (type == 7) {
        palette = makePackedPalette ();
    }

    if ((type != 1) && (type != 4) && !palette) {
        fprintf (stderr, "ppmreader: out of memory for palette\n");
        free (pixmap);
        return 0;
    }

    /* this will break if we have a CR/LF, but having a CR/LF
       violated the specification (and we can't check for CR/LR
       either, because the CR is a valid whitespace character,
       and the LF might be the pixel value 10)
    */
    reader.next++;

    switch (type) {
        case 1:
            readType1 (size, pixmap, &reader);
            break;
        case 2:
            readType2 (size, pixmap, &reader, palette);
            break;
        case 3:
            readType3 (size, pixmap, &reader, palette);
            break;
        case 4:
            readType4 (width, height, pixmap, &reader);
            break;
        case 5:
            readType5 (size, pixmap, &reader, palette);
            break;
        case 6:
            readType6 (size, pixmap, &reader, palette);
            break;
        case 7:
            readType7 (size, pixmap, &reader, palette);
            break;
    }

    if (palette) {
        free (palette);
    }

    if (reader.failed) {
        fprintf (stderr, "ppmreader: image data is truncated or malformed\n");
        free (pixmap);
        return 0;
    }

    return pixmap;
}

unsigned char* read_ppm_file(const char* filename, int* texWidth, int* texHeight) {
    long size;
    unsigned char* data;
    unsigned char* result;
    FILE* fd;
    struct stat st;

    if (!filename) {
        fprintf (stderr, "ppmreader: no filename given\n");
        return 0;
    }

    fd = fopen(filename, "rb");
    if (!fd) {
        fprintf (stderr, "ppmreader: cannot open %s\n", filename);
        return 0;
    }

    if (fstat(fileno(fd), &st) != 0) {
        fprintf (stderr, "ppmreader: cannot stat %s\n", filename);
        fclose(fd);
        return 0;
    }

    size = (long) st.st_size;
    if (size < 3 || size > INT_MAX) {
        fprintf (stderr, "ppmreader: %s has an implausible size\n", filename);
        fclose(fd);
        return 0;
    }

    data = (unsigned char*) malloc(size);
    if (!data) {
        fprintf (stderr, "ppmreader: out of memory reading %s\n", filename);
        fclose(fd);
        return 0;
    }

    if (fread(data, 1, size, fd) != (size_t) size) {
        fprintf (stderr, "ppmreader: short read on %s\n", filename);
        fclose(fd);
        free(data);
        return 0;
    }

    result = do_read(data, (int) size, texWidth, texHeight);

    fclose(fd);
    free(data);

    return result;
}
