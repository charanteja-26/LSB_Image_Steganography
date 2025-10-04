#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

typedef struct _DecodeInfo
{
    /* Stego Image info */
    char de_stego_image_fname[20];
    FILE *fptr_de_stego_image;
    
    int de_secret_extn_file_size;//secret file extn size
    char de_secret_file_extn[10]; //secret file extn.
    int de_secret_file_size; //secret file size.
    //output.bmp
    char output_fname[30];        //filename 
    FILE *fptr_output_file;         //file pointer

} DecodeInfo;

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status de_open_files(DecodeInfo *decInfo);

/*Skiping header file*/
Status skip_header(FILE *fptr_de_stego_image);

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/*Decode secret file extn size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/*decode secret file extn*/
Status decode_secret_file_extn_open_file(DecodeInfo *decInfo);

/*Decode secret file extn*/
Status decode_secret_file_size(DecodeInfo *decInfo);

/*Decode secret file size*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode LSB to byte data array */
Status decode_lsb_to_byte(char *data, char *image_buffer);//

/*Encode LSB into size of image data array*/
Status decode_lsb_to_size(int *size, char *image_buffer);

#endif