#ifndef DECODE_H
#define DECODE_H
#include<stdio.h>
#include "encode.h"
#include "types.h" // Contains user defined types

typedef struct DecodeInf
{
    /*Dest image information*/
    char *decode_dest_fname;          // To store the dest file name
    FILE *decode_dest_fptr;           // To store the address of stego image
    char decode_extn_secret_file[5];  // To store the Secret file extension
    int decode_secret_extn_size;      // To store the secret file extension size
    long decode_size_secret_file;     // To store the size of the secret data

    /*secrete out data information*/
    char *secrete_out_fname;          // To store the secrete data filename
    FILE *secrete_out_fptr;           // To store the address of the secrete data
    long int secret_size_data; 
}DecodeInfo;

/*Decoding function prototype*/

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(char* argv[],DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(char *argv[], DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
Status decode_byte_to_lsb(char *character, char *image_buffer);

/* Decode a size to lsb */
Status decode_size_to_lsb(char* character, char *imageBuffer);

#endif