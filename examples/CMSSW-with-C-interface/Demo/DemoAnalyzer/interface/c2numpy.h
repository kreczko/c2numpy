// Copyright 2016 Jim Pivarski
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef C2NUMPY
#define C2NUMPY

#include <inttypes.h>
#include <stdarg.h>
#include <string.h>

const char* C2NUMPY_VERSION = "1.1";

// http://docs.scipy.org/doc/numpy/user/basics.types.html
typedef enum {
    C2NUMPY_BOOL,        // Boolean (True or False) stored as a byte
    C2NUMPY_INT,         // Default integer type (same as C long; normally either int64 or int32)
    C2NUMPY_INTC,        // Identical to C int (normally int32 or int64)
    C2NUMPY_INTP,        // Integer used for indexing (same as C ssize_t; normally either int32 or int64)
    C2NUMPY_INT8,        // Byte (-128 to 127)
    C2NUMPY_INT16,       // Integer (-32768 to 32767)
    C2NUMPY_INT32,       // Integer (-2147483648 to 2147483647)
    C2NUMPY_INT64,       // Integer (-9223372036854775808 to 9223372036854775807)
    C2NUMPY_UINT8,       // Unsigned integer (0 to 255)
    C2NUMPY_UINT16,      // Unsigned integer (0 to 65535)
    C2NUMPY_UINT32,      // Unsigned integer (0 to 4294967295)
    C2NUMPY_UINT64,      // Unsigned integer (0 to 18446744073709551615)
    C2NUMPY_FLOAT,       // Shorthand for float64.
    C2NUMPY_FLOAT16,     // Half precision float: sign bit, 5 bits exponent, 10 bits mantissa
    C2NUMPY_FLOAT32,     // Single precision float: sign bit, 8 bits exponent, 23 bits mantissa
    C2NUMPY_FLOAT64,     // Double precision float: sign bit, 11 bits exponent, 52 bits mantissa
    C2NUMPY_COMPLEX,     // Shorthand for complex128.
    C2NUMPY_COMPLEX64,   // Complex number, represented by two 32-bit floats (real and imaginary components)
    C2NUMPY_COMPLEX128,  // Complex number, represented by two 64-bit floats (real and imaginary components)

    C2NUMPY_STRING       = 100,  // strings are C2NUMPY_STRING + their fixed size (up to 155)
    C2NUMPY_END          = 255   // ensure that c2numpy_type is at least a byte
} c2numpy_type;

// a Numpy writer object
typedef struct {
    char buffer[16];              // (internal) used for temporary copies in c2numpy_row

    FILE *file;                   // output file handle
    char *outputFilePrefix;       // output file name, not including the rotating number and .npy
    int64_t sizeSeekPosition;     // (internal) keep track of number of rows to modify before closing
    int64_t sizeSeekSize;         // (internal)

    int32_t numColumns;           // number of columns in the record array
    char **columnNames;           // column names
    c2numpy_type *columnTypes;    // column types

    int32_t numRowsPerFile;       // maximum number of rows per file
    int32_t currentColumn;        // current column number
    int32_t currentRowInFile;     // current row number in the current file
    int32_t currentFileNumber;    // current file number
} c2numpy_writer;

const char *c2numpy_descr(c2numpy_type type) {
    // FIXME: all of the "<" signs should be system-dependent (they mean little endian)
    static const char *c2numpy_bool = "|b1";
    static const char *c2numpy_int = "<i8";
    static const char *c2numpy_intc = "<i4";   // FIXME: should be system-dependent
    static const char *c2numpy_intp = "<i8";   // FIXME: should be system-dependent
    static const char *c2numpy_int8 = "|i1";
    static const char *c2numpy_int16 = "<i2";
    static const char *c2numpy_int32 = "<i4";
    static const char *c2numpy_int64 = "<i8";
    static const char *c2numpy_uint8 = "|u1";
    static const char *c2numpy_uint16 = "<u2";
    static const char *c2numpy_uint32 = "<u4";
    static const char *c2numpy_uint64 = "<u8";
    static const char *c2numpy_float = "<f8";
    static const char *c2numpy_float16 = "<f2";
    static const char *c2numpy_float32 = "<f4";
    static const char *c2numpy_float64 = "<f8";
    static const char *c2numpy_complex = "<c16";
    static const char *c2numpy_complex64 = "<c8";
    static const char *c2numpy_complex128 = "<c16";

    static const char *c2numpy_str[155] = {"|S0", "|S1", "|S2", "|S3", "|S4", "|S5", "|S6", "|S7", "|S8", "|S9", "|S10", "|S11", "|S12", "|S13", "|S14", "|S15", "|S16", "|S17", "|S18", "|S19", "|S20", "|S21", "|S22", "|S23", "|S24", "|S25", "|S26", "|S27", "|S28", "|S29", "|S30", "|S31", "|S32", "|S33", "|S34", "|S35", "|S36", "|S37", "|S38", "|S39", "|S40", "|S41", "|S42", "|S43", "|S44", "|S45", "|S46", "|S47", "|S48", "|S49", "|S50", "|S51", "|S52", "|S53", "|S54", "|S55", "|S56", "|S57", "|S58", "|S59", "|S60", "|S61", "|S62", "|S63", "|S64", "|S65", "|S66", "|S67", "|S68", "|S69", "|S70", "|S71", "|S72", "|S73", "|S74", "|S75", "|S76", "|S77", "|S78", "|S79", "|S80", "|S81", "|S82", "|S83", "|S84", "|S85", "|S86", "|S87", "|S88", "|S89", "|S90", "|S91", "|S92", "|S93", "|S94", "|S95", "|S96", "|S97", "|S98", "|S99", "|S100", "|S101", "|S102", "|S103", "|S104", "|S105", "|S106", "|S107", "|S108", "|S109", "|S110", "|S111", "|S112", "|S113", "|S114", "|S115", "|S116", "|S117", "|S118", "|S119", "|S120", "|S121", "|S122", "|S123", "|S124", "|S125", "|S126", "|S127", "|S128", "|S129", "|S130", "|S131", "|S132", "|S133", "|S134", "|S135", "|S136", "|S137", "|S138", "|S139", "|S140", "|S141", "|S142", "|S143", "|S144", "|S145", "|S146", "|S147", "|S148", "|S149", "|S150", "|S151", "|S152", "|S153", "|S154"};

    switch (type) {
      case C2NUMPY_BOOL:
          return c2numpy_bool;
      case C2NUMPY_INT:
          return c2numpy_int;
      case C2NUMPY_INTC:
          return c2numpy_intc;
      case C2NUMPY_INTP:
          return c2numpy_intp;
      case C2NUMPY_INT8:
          return c2numpy_int8;
      case C2NUMPY_INT16:
          return c2numpy_int16;
      case C2NUMPY_INT32:
          return c2numpy_int32;
      case C2NUMPY_INT64:
          return c2numpy_int64;
      case C2NUMPY_UINT8:
          return c2numpy_uint8;
      case C2NUMPY_UINT16:
          return c2numpy_uint16;
      case C2NUMPY_UINT32:
          return c2numpy_uint32;
      case C2NUMPY_UINT64:
          return c2numpy_uint64;
      case C2NUMPY_FLOAT:
          return c2numpy_float;
      case C2NUMPY_FLOAT16:
          return c2numpy_float16;
      case C2NUMPY_FLOAT32:
          return c2numpy_float32;
      case C2NUMPY_FLOAT64:
          return c2numpy_float64;
      case C2NUMPY_COMPLEX:
          return c2numpy_complex;
      case C2NUMPY_COMPLEX64:
          return c2numpy_complex64;
      case C2NUMPY_COMPLEX128:
          return c2numpy_complex128;
      default:
          if (0 < type - C2NUMPY_STRING  &&  type - C2NUMPY_STRING < 155)
              return c2numpy_str[type - C2NUMPY_STRING];
    }

    return NULL;
}

int c2numpy_init(c2numpy_writer *writer, const char *outputFilePrefix, int32_t numRowsPerFile) {
    writer->file = NULL;
    writer->outputFilePrefix = (char*)malloc(strlen(outputFilePrefix) + 1);
    strcpy(writer->outputFilePrefix, outputFilePrefix);
    writer->sizeSeekPosition = 0;
    writer->sizeSeekSize = 0;

    writer->numColumns = 0;
    writer->columnNames = NULL;
    writer->columnTypes = NULL;

    writer->numRowsPerFile = numRowsPerFile;
    writer->currentColumn = 0;
    writer->currentRowInFile = 0;
    writer->currentFileNumber = 0;

    return 0;
}

int c2numpy_addcolumn(c2numpy_writer *writer, const char *name, c2numpy_type type) {
    writer->numColumns += 1;

    char *newColumnName = (char*)malloc(strlen(name) + 1);
    strcpy(newColumnName, name);

    char **oldColumnNames = writer->columnNames;
    writer->columnNames = (char**)malloc(writer->numColumns * sizeof(char*));
    for (int column = 0;  column < writer->numColumns - 1;  ++column)
        writer->columnNames[column] = oldColumnNames[column];
    writer->columnNames[writer->numColumns - 1] = newColumnName;
    if (oldColumnNames != NULL)
        free(oldColumnNames);

    c2numpy_type *oldColumnTypes = writer->columnTypes;
    writer->columnTypes = (c2numpy_type*)malloc(writer->numColumns * sizeof(c2numpy_type));
    for (int column = 0;  column < writer->numColumns - 1;  ++column)
        writer->columnTypes[column] = oldColumnTypes[column];
    writer->columnTypes[writer->numColumns - 1] = type;
    if (oldColumnTypes != NULL)
        free(oldColumnTypes);

    return 0;
}

int c2numpy_open(c2numpy_writer *writer) {
    char *fileName = (char*)malloc(strlen(writer->outputFilePrefix) + 15);
    sprintf(fileName, "%s%d.npy", writer->outputFilePrefix, writer->currentFileNumber);
    writer->file = fopen(fileName, "wb");

    // FIXME: better initial guess about header size before going in 128 byte increments
    char *header = NULL;
    for (int64_t headerSize = 128;  headerSize <= 4294967295;  headerSize += 128) {
        if (header != NULL) free(header);
        header = (char*)malloc(headerSize + 1);

        char version1 = headerSize <= 65535;
        uint32_t descrSize;
        if (version1)
            descrSize = headerSize - 10;
        else
            descrSize = headerSize - 12;

        header[0] = 147;                            // magic
        header[1] = 'N';
        header[2] = 'U';
        header[3] = 'M';
        header[4] = 'P';
        header[5] = 'Y';
        if (version1) {
            header[6] = 1;                          // format version 1.0
            header[7] = 0;
            const uint16_t descrSize2 = descrSize;
            *(uint16_t*)(header + 8) = descrSize2;   // version 1.0 has a 16-byte descrSize
        }
        else {
            header[6] = 2;                          // format version 2.0
            header[7] = 0;
            *(uint32_t*)(header + 8) = descrSize;   // version 2.0 has a 32-byte descrSize
        }

        int64_t offset = headerSize - descrSize;
        offset += snprintf((header + offset), headerSize - offset + 1, "{'descr': [");
        if (offset >= headerSize) continue;

        for (int column = 0;  column < writer->numColumns;  ++column) {
            offset += snprintf((header + offset), headerSize - offset + 1, "('%s', '%s')",
                              writer->columnNames[column],
                              c2numpy_descr(writer->columnTypes[column]));
            if (offset >= headerSize) continue;

            if (column < writer->numColumns - 1)
                offset += snprintf((header + offset), headerSize - offset + 1, ", ");
            if (offset >= headerSize) continue;
        }

        offset += snprintf((header + offset), headerSize - offset + 1, "], 'fortran_order': False, 'shape': (");
        if (offset >= headerSize) continue;

        writer->sizeSeekPosition = offset;
        writer->sizeSeekSize = snprintf((header + offset), headerSize - offset + 1, "%d", writer->numRowsPerFile);
        offset += writer->sizeSeekSize;
        if (offset >= headerSize) continue;

        offset += snprintf((header + offset), headerSize - offset + 1, ",), }");
        if (offset >= headerSize) continue;

        while (offset < headerSize) {
            if (offset < headerSize - 1)
                header[offset] = ' ';
            else
                header[offset] = '\n';
            offset += 1;
        }
        header[headerSize] = 0;

        fwrite(header, 1, headerSize, writer->file);

        return 0;
    }

    return -1;
}

#define C2NUMPY_CHECK_ITEM {                                                    \
    if (writer->file == NULL) {                                                 \
        int status = c2numpy_open(writer);                                      \
        if (status != 0)                                                        \
            return status;                                                      \
    }                                                                           \
}

#define C2NUMPY_INCREMENT_ITEM {                                                \
    if (writer->currentColumn == 0) {                                           \
        writer->currentRowInFile += 1;                                          \
        if (writer->currentRowInFile == writer->numRowsPerFile) {               \
            fclose(writer->file);                                               \
            writer->file = NULL;                                                \
            writer->currentRowInFile = 0;                                       \
            writer->currentFileNumber += 1;                                     \
        }                                                                       \
    }                                                                           \
    return 0;                                                                   \
}

int c2numpy_bool(c2numpy_writer *writer, int8_t data) {   // "bool" is just a byte
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_BOOL) return -1;
    fwrite(&data, sizeof(int8_t), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_int(c2numpy_writer *writer, int64_t data) {   // Numpy's default int is 64-bit
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_INT) return -1;
    fwrite(&data, sizeof(int64_t), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_intc(c2numpy_writer *writer, int data) {      // the built-in C int
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_INTC) return -1;
    fwrite(&data, sizeof(int), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_intp(c2numpy_writer *writer, size_t data) {   // intp is Numpy's way of saying size_t
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_INTP) return -1;
    fwrite(&data, sizeof(size_t), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_int8(c2numpy_writer *writer, int8_t data) {
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_INT8) return -1;
    fwrite(&data, sizeof(int8_t), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_int16(c2numpy_writer *writer, int16_t data) {
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_INT16) return -1;
    fwrite(&data, sizeof(int16_t), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_int32(c2numpy_writer *writer, int32_t data) {
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_INT32) return -1;
    fwrite(&data, sizeof(int32_t), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_int64(c2numpy_writer *writer, int64_t data) {
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_INT64) return -1;
    fwrite(&data, sizeof(int64_t), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_uint8(c2numpy_writer *writer, uint8_t data) {
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_UINT8) return -1;
    fwrite(&data, sizeof(uint8_t), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_uint16(c2numpy_writer *writer, uint16_t data) {
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_UINT16) return -1;
    fwrite(&data, sizeof(uint16_t), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_uint32(c2numpy_writer *writer, uint32_t data) {
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_UINT32) return -1;
    fwrite(&data, sizeof(uint32_t), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_uint64(c2numpy_writer *writer, uint64_t data) {
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_UINT64) return -1;
    fwrite(&data, sizeof(uint64_t), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_float(c2numpy_writer *writer, double data) {   // Numpy's "float" is a double
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_FLOAT) return -1;
    fwrite(&data, sizeof(double), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

// int c2numpy_float16(c2numpy_writer *writer, ??? data) {   // how to do float16 in C?
//     C2NUMPY_CHECK_ITEM
//     if (writer->columnTypes[writer->currentColumn] != C2NUMPY_FLOAT16) return -1;
//     fwrite(&data, sizeof(???), 1, writer->file);
//     writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
//     C2NUMPY_INCREMENT_ITEM
// }

int c2numpy_float32(c2numpy_writer *writer, float data) {
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_FLOAT32) return -1;
    fwrite(&data, sizeof(float), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_float64(c2numpy_writer *writer, double data) {
    C2NUMPY_CHECK_ITEM
    if (writer->columnTypes[writer->currentColumn] != C2NUMPY_FLOAT64) return -1;
    fwrite(&data, sizeof(double), 1, writer->file);
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
    C2NUMPY_INCREMENT_ITEM
}

// int c2numpy_complex(c2numpy_writer *writer, ??? data) {    // how to do complex in C?
//     C2NUMPY_CHECK_ITEM
//     if (writer->columnTypes[writer->currentColumn] != C2NUMPY_COMPLEX) return -1;
//     fwrite(&data, sizeof(???), 1, writer->file);
//     writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
//     C2NUMPY_INCREMENT_ITEM
// }

// int c2numpy_complex64(c2numpy_writer *writer, ??? data) {
//     C2NUMPY_CHECK_ITEM
//     if (writer->columnTypes[writer->currentColumn] != C2NUMPY_COMPLEX64) return -1;
//     fwrite(&data, sizeof(???), 1, writer->file);
//     writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
//     C2NUMPY_INCREMENT_ITEM
// }

// int c2numpy_complex128(c2numpy_writer *writer, ??? data) {
//     C2NUMPY_CHECK_ITEM
//     if (writer->columnTypes[writer->currentColumn] != C2NUMPY_COMPLEX128) return -1;
//     fwrite(&data, sizeof(???), 1, writer->file);
//     writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;
//     C2NUMPY_INCREMENT_ITEM
// }

int c2numpy_string(c2numpy_writer *writer, const char *data) {
    C2NUMPY_CHECK_ITEM

    int stringlength = writer->columnTypes[writer->currentColumn] - C2NUMPY_STRING;
    if (0 < stringlength  &&  stringlength < 155)
        fwrite(data, 1, stringlength, writer->file);
    else
        return -1;
    writer->currentColumn = (writer->currentColumn + 1) % writer->numColumns;

    C2NUMPY_INCREMENT_ITEM
}

int c2numpy_close(c2numpy_writer *writer) {
    if (writer->file != NULL) {
        // we wrote fewer rows than we promised
        if (writer->currentRowInFile < writer->numRowsPerFile) {
            // so go back to the part of the header where that was written
            fseek(writer->file, writer->sizeSeekPosition, SEEK_SET);
            // overwrite it with spaces
            for (int i = 0;  i < writer->sizeSeekSize;  ++i)
                fputc(' ', writer->file);
            // now go back and write it again (it MUST be fewer or an equal number of digits)
            fseek(writer->file, writer->sizeSeekPosition, SEEK_SET);
            fprintf(writer->file, "%d", writer->currentRowInFile);
        }
        // now close it
        fclose(writer->file);
    }

    // and clear the malloc'ed memory
    free(writer->outputFilePrefix);
    for (int column = 0;  column < writer->numColumns;  ++column)
        free(writer->columnNames[column]);
    free(writer->columnNames);
    free(writer->columnTypes);

    return 0;
}

#endif // C2NUMPY
