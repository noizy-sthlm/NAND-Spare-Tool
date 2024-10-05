/**
*Copyright (c) 2024 Amin Nouiser
*
*Permission is hereby granted, free of charge, to any person obtaining a copy
*of this software and associated documentation files (the "Software"), to deal
*in the Software without restriction, including without limitation the rights
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*copies of the Software, and to permit persons to whom the Software is
*furnished to do so, subject to the following conditions:
*
*The above copyright notice and this permission notice shall be included in all
*copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define EXPECTED_ARGC 4

struct arguments
{
    const char* file_in_path;
    const char* file_out_path;
    size_t spareSize;
    size_t pageSize_data;
};

void printUsage();
int argumentParser(int argc, char* argv[], struct arguments* arguments);
int spareRemoval(FILE* in, FILE* out, size_t pageSize, size_t spareSize);

int main(int argc, char* argv[])
{
    struct arguments args = {
        .file_out_path = "./out.bin",
    };

    if (argumentParser(argc, argv, &args) != 0)
    {
        printUsage();
        return EXIT_FAILURE;
    }

    FILE* binarySource = fopen(args.file_in_path, "rb");
    if (binarySource == NULL)
    {
        perror("Error opening file_in");
        return EXIT_FAILURE;
    }
    FILE* binaryOutput = fopen(args.file_out_path, "wb");
    if (binaryOutput == NULL)
    {
        perror("Error creating output stream");
        fclose(binarySource);
        return EXIT_FAILURE;
    }

    if (spareRemoval(binarySource, binaryOutput, args.pageSize_data, args.spareSize) != 0)
    {
        fprintf(stderr, "Error during spare removal\n");
        fclose(binarySource);
        fclose(binaryOutput);
        return EXIT_FAILURE;
    }

    fclose(binarySource);
    fclose(binaryOutput);

    printf("Spare was removed\n");
    return EXIT_SUCCESS;
}

int spareRemoval(FILE* in, FILE* out, const size_t pageSize, const size_t spareSize)
{
    unsigned char* buffer = malloc(pageSize);
    if (buffer == NULL)
    {
        perror("Error allocating memory for buffer");
        return EXIT_FAILURE;
    }

    size_t read_bytes;
    while ((read_bytes = fread(buffer, 1, pageSize, in)) > 0)
    {
        if (ferror(in))
        {
            perror("Error reading from input file");
            free(buffer);
            return EXIT_FAILURE;
        }
        if (fwrite(buffer, 1, read_bytes, out) != read_bytes)
        {
            perror("Error writing to output file");
            free(buffer);
            return EXIT_FAILURE;
        }
        if (fseek(in, spareSize, SEEK_CUR))
        {
            if (!feof(in))
            {
                perror("Error when seeking to next page");
                free(buffer);
                return EXIT_FAILURE;
            }
        }
    }

    free(buffer);
    return 0;
}


//Make use of errno
int argumentParser(const int argc, char* argv[], struct arguments* arguments)
{
    int c;
    while ((c = getopt(argc, argv, "o:")) != -1)
    {
        switch (c)
        {
        case 'o':
            arguments->file_out_path = optarg;
            break;
        default:
            return -1;
        }
    }

    if (argc - (optind - 1) != EXPECTED_ARGC)
        return -1;

    arguments->file_in_path = argv[optind];

    char* endPtr;
    arguments->pageSize_data = strtoul(argv[optind + 1], &endPtr, 10);
    if (*endPtr != '\0' || arguments->pageSize_data == 0)
    {
        fprintf(stderr, "Error: Invalid main area size '%s'\n", argv[optind + 1]);
        return -1;
    }
    arguments->spareSize = strtoul(argv[optind + 2], &endPtr, 10);
    if (*endPtr != '\0' || arguments->spareSize == 0)
    {
        fprintf(stderr, "Error: Invalid spare area size '%s'\n", argv[optind + 2]);
        return -1;
    }

    return 0;
}


void printUsage()
{
    fprintf(stderr,
            "Usage : sprm [OPTIONS] <file> <pageSize> <spareSize>\n\n"
            "NAND Spare Tool v1.0 by noizy-sthlm\n"
            "A simple tool for removing your NAND's spare areas\n\n"
            "Positional Arguments:\n"
            "   file        Input dump\n"
            "   pageSize    Page size in bytes\n"
            "   spareSize   OOB area\n"
            "Options:\n"
            "   -o <out>    Default is ./out.bin\n");
}