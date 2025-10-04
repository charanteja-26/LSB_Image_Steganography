#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "magic.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo_pst)
{
    //strstr returns address of '.' and then strcmp .bmp and .bmp(from . address it will contain bmp) 
    //checking wheather there is .bmp file is provided or not
    if(strcmp((strchr(argv[2],'.')),".bmp") == 0)
    {
        strcpy(decInfo_pst->de_stego_image_fname,argv[2]);
    }
    else
    {
        return e_failure;
    }
    //checking wheather there is .txt (or) .c (or).sh file is provided or not
    if( argv[3] != NULL)
    {
        if ((strchr(argv[3] , '.')) == NULL)
        {
            printf("Output File name without extension is mentioned\n");
            strcpy(decInfo_pst->output_fname,argv[3]);
        }
        else if((strcmp((strchr(argv[3],'.')),".txt") == 0) || (strcmp((strchr(argv[3],'.')),".c") == 0) || (strcmp((strchr(argv[3],'.')),".sh") == 0))
        {
            printf("Output name is given with file extn removing extn and storing file name\n");
            
             for(int i = 0;argv[3][i] != '\0';i++)
             {
                if(argv[3][i] == '.')
                {
                    argv[3][i] = '\0';
                    break;
                }
             }
             strcpy(decInfo_pst->output_fname,argv[3]);
             return e_success;
        }
    }
    else
    {
        // printf("hello\n");
        printf("Output File not mentioned. Creating output as default without extension\n");
        strcpy(decInfo_pst->output_fname,"output");
       
    }
    return e_success;
}

Status de_open_files(DecodeInfo *decInfo_pst)
{
    // stego Image file
    printf("Opening required files\n");
    decInfo_pst->fptr_de_stego_image = fopen(decInfo_pst->de_stego_image_fname, "r");
    // Do Error handling
    if (decInfo_pst->fptr_de_stego_image != NULL)
    {
        printf("Opened Stego.bmp\n");
    }
    else if (decInfo_pst->fptr_de_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo_pst->de_stego_image_fname);

    	return e_failure;
    }
    return e_success;
}

Status skip_header(FILE *fptr_de_stego_image)
{
    //printf("secret file pos -> %ld\n",ftell(fptr_de_stego_image));
    fseek(fptr_de_stego_image,54,SEEK_SET);

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo_pst)
{
    //start decoding
    if(de_open_files(decInfo_pst) == e_success)
    {
        printf("Open file is successful\n");
        if(skip_header(decInfo_pst->fptr_de_stego_image) == e_success)
        {
            printf("Skipping header file of image is successfull\n");
            if(decode_magic_string(decInfo_pst) == e_success)
            {
                printf("Decoding magic string is successfull\n");
                if(decode_secret_file_extn_size(decInfo_pst) == e_success)
                {
                    printf("Decoding secret file extn size is successfull\n");
                    if(decode_secret_file_extn_open_file(decInfo_pst) == e_success)
                    {
                        printf("Decoding file extn and creating output file is successfull\n");
                        if(decode_secret_file_size(decInfo_pst) == e_success)
                        {
                            printf("Decoding secret file size is successfull\n");
                            if(decode_secret_file_data(decInfo_pst) == e_success)
                            {
                                printf("\n## Decoding Stego.bmp to output data is done ##\n");
                            }
                            else
                            {
                                printf("Decoding secret file data is failed\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Decoding secret file size is failed\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Decoding file extn and creating output file is failed\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Decoding secret file extn size is failed\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Decoding magic string is failed\n");
                return e_failure;
            }
        }
        else
        {
            printf("Skipping header file of image is failed\n");
            return e_failure;
        }
    }
    else
    {
        printf("Open file is Failure\n");
        return e_failure;
    }
     
    return e_success;
}


Status decode_lsb_to_byte(char *data,char *buff)
{
    
    *data = 0;
    for (int i = 0; i <= 7; i++)
    {
        int ch = buff[i] & 1;
        // printf("ch    -> %d\n",ch);
        *data =  ((buff[i]&1) << (7-i)) | *data;
    }
    // printf("data -> %c\n",*data);
    return e_success;
}

Status decode_lsb_to_size(int *size, char *image_buff)
{
    *size = 0;
    for (int i = 0; i < 32; i++)
    {
        //printf("file buffer %d\n",buffer[i] & 1);
        *size = ((image_buff[i]&1) << (31-i)) | *size;
    }
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo_pst)
{
   // printf("secret file pos -> %ld\n",ftell(decInfo_pst->fptr_de_stego_image));
    printf("Decoding magic string\n");
    char data;
    char check_magic[strlen(MAGIC_STRING)+1];
    char buff[8];
    int i=0;
    for ( i; i <  strlen(MAGIC_STRING); i++)
    {
        fread(buff, 8, 1, decInfo_pst->fptr_de_stego_image);
        decode_lsb_to_byte(&data,buff);
        //printf("%c\n",data);
        check_magic[i] = data;  
    }
    check_magic[i] = '\0';
    //printf("Decoded magic string => ");
    //printf("%s\n",check_magic);
    if (strcmp(check_magic,MAGIC_STRING))
        {
            fprintf(stderr, "Error: Magic string is not match\n");
            return e_failure;
        }
        printf("Compairing magic string is successfull both encoded and decoded magic strings are equal\n");
    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo_pst)
{
    //printf("secret file pos -> %ld\n",ftell(decInfo_pst->fptr_de_stego_image));
    int size;
    char buff[32];
    fread(buff,32, 1, decInfo_pst->fptr_de_stego_image);
    decode_lsb_to_size(&size,buff);
    //printf("File extension size: %d\n",size);
    decInfo_pst->de_secret_extn_file_size = size;
    return e_success;
}

Status decode_secret_file_extn_open_file(DecodeInfo *decInfo_pst)
{
    char data;
    char buff[8];
    int i=0;
    //printf("secret file pos -> %ld\n",ftell(decInfo_pst->fptr_de_stego_image));
     for (i;i < decInfo_pst->de_secret_extn_file_size; i++)
    {
        fread(buff, 1, 8, decInfo_pst->fptr_de_stego_image);
         
        decode_lsb_to_byte(&data, buff);
        //printf("file extn is %c",data);
        decInfo_pst->de_secret_file_extn[i] = data;

    }
    //printf("\n");
    decInfo_pst->de_secret_file_extn[i] = '\0';  
    //printf("File extension: %s\n",decInfo_pst->de_secret_file_extn);
    strcat(decInfo_pst->output_fname,decInfo_pst->de_secret_file_extn);
    //printf("file name is %s\n",decInfo_pst->output_fname);
    decInfo_pst->fptr_output_file = fopen(decInfo_pst->output_fname, "w");
    if (decInfo_pst->fptr_output_file == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo_pst->output_fname);
    	return e_failure;
    }
    printf("Opening output file is done");
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo_pst)
{
    //printf("secret file pos -> %ld\n",ftell(decInfo_pst->fptr_de_stego_image));
    int size;
    char buff[32];
    fread(buff,32, 1, decInfo_pst->fptr_de_stego_image);
    decode_lsb_to_size(&size,buff);
   // printf("File extension size: %d\n",size);
    decInfo_pst->de_secret_file_size = size;
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo_pst)
{
    //printf("secret file data pos -> %ld\n",ftell(decInfo_pst->fptr_de_stego_image));
    char data = 0;
    char buff[8];
    //printf("file size -> %d\n",decInfo_pst->de_secret_file_size);
   // printf("Secret file data is => ");
    for (int i = 0; i < decInfo_pst->de_secret_file_size; i++)
    {
        fread(buff, 1, 8, decInfo_pst->fptr_de_stego_image);
        decode_lsb_to_byte(&data,buff);
         //printf("%c",data);
        fputc(data, decInfo_pst->fptr_output_file);
    }
   // printf("\n");
      //printf("after secret file data pos -> %ld\n",ftell(decInfo_pst->fptr_de_stego_image));
    //printf("Done decode_secret_file_data\n");
    return e_success;
}

