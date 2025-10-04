#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "magic.h"



/* Function Definitions */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo_pst)
{
    //strstr returns address of '.' and then strcmp .bmp and .bmp(from . address it will contain bmp) 
    //checking wheather there is .bmp file is provided or not
    if(strcmp((strstr(argv[2],".")),".bmp") == 0)
    {
        encInfo_pst->src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }
    //checking wheather there is .txt (or) .c (or).sh file is provided or not
    if(strcmp((strchr(argv[3],'.')),".txt") == 0 || strcmp((strchr(argv[3],'.')),".c") == 0 || strcmp((strchr(argv[3],'.')),".sh") == 0)
    {
        encInfo_pst->secret_fname = argv[3];
        strcpy(encInfo_pst->extn_secret_file,strchr(argv[3] , '.'));
    }
    else
    {
        return e_failure;
    }
    //checking wheather there is new file is provided or not
     if( argv[4] != NULL)
    {
        encInfo_pst->stego_image_fname = argv[4];
    }
    else
    {
        printf("Output File not mentioned. Creating stego.bmp as default\n");
         encInfo_pst->stego_image_fname = "Stego.bmp";
    }

    return e_success;

}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo_pst)
{
    // Src Image file
    printf("Opening required files\n");
    encInfo_pst->fptr_src_image = fopen(encInfo_pst->src_image_fname, "r");
    // Do Error handling
    if (encInfo_pst->fptr_src_image != NULL)
    {
        printf("Opened beautiful.bmp\n");
    }
    else if (encInfo_pst->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo_pst->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo_pst->fptr_secret = fopen(encInfo_pst->secret_fname, "r");
    // Do Error handling
    if (encInfo_pst->fptr_secret != NULL)
    {
        printf("Opened secret.txt\n");
    }
    else if (encInfo_pst->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo_pst->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo_pst->fptr_stego_image = fopen(encInfo_pst->stego_image_fname, "w");
    // Do Error handling
    if (encInfo_pst->fptr_stego_image != NULL)
    {
        printf("Opened Stego.bmp\n");
        printf("Done\n");
        printf("## Encoding Procedure Started ##\n");
        printf(" Checking for secret.txt size\n");
    }
    else if (encInfo_pst->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo_pst->stego_image_fname);

    	return e_failure;
    }
    printf("Done. Not Empty\n");
    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo_pst)
{
    printf("Checking for SkeletonCode/beautiful.bmp capacity to handle secret.txt\n");
    //get the image size of bmp file
    encInfo_pst->image_capacity = get_image_size_for_bmp(encInfo_pst->fptr_src_image);
    //size of secret.txt file
    encInfo_pst->size_secret_file = get_file_size(encInfo_pst->fptr_secret);
    if(encInfo_pst->image_capacity > (54 + (2+4+4+4+encInfo_pst->size_secret_file)*8) )
    {
        printf("Done. Found OK\n");
            return e_success;
    }
    else
    {
            return e_failure;
    }
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
   char str[54];
   fseek(fptr_src_image,0,SEEK_SET);
   fread(str,54,1,fptr_src_image);
   fwrite(str,54,1,fptr_dest_image);
   return e_success;
}

Status do_encoding(EncodeInfo *encInfo_pst)
{
    //start encoding
    if(open_files(encInfo_pst) == e_success)
    {
        printf("Open file is successful\n");
        if(check_capacity(encInfo_pst) == e_success)
        {
            printf("Check capacity is successfull\n");
            if(copy_bmp_header(encInfo_pst->fptr_src_image,encInfo_pst->fptr_stego_image) == e_success)
            {
                printf("Copying bmp header is successful\n");
                if(encode_magic_string(MAGIC_STRING, encInfo_pst) == e_success )
                {
                    printf("Encoding magic string is successfull\n");
                    if(encode_secret_file_extn_size(strlen(encInfo_pst->extn_secret_file),encInfo_pst) == e_success)
                    {
                        printf("Encoding secret file extension size is successfull\n");
                        if(encode_secret_file_extn(encInfo_pst->extn_secret_file,encInfo_pst) == e_success)
                        {
                            printf("Encoding secret file extension is successfull\n");
                            if(encode_secret_file_size(encInfo_pst->size_secret_file,encInfo_pst) == e_success)
                            {
                                printf("Encoding secret file size is successfull\n");
                                if(encode_secret_file_data(encInfo_pst) == e_success)
                                {
                                    printf("Encoding secret file data is successfull\n");
                                    if(copy_remaining_img_data(encInfo_pst->fptr_src_image,encInfo_pst->fptr_stego_image) == e_success)
                                    {
                                        printf("Copying remaining image data is successfull\n");
                                        printf("\n## Encoding file to Stego.bmp is done ##\n");
                                    }
                                    else
                                    {
                                        printf("Copying remaining image data is failed\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Encoding secret file data is failed\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Encoding secret file size is failed\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Encoding secret file extension  is failed\n");
                            return e_failure;
                        }
                    } 
                    else
                    {
                        printf("Encoding secret file extension size is failed\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Encoded magic string is failed\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Copied bmp header is failure\n");
                return e_failure;
            }
        }
        else
        {
            printf("Check capacity is failure\n");
            return e_failure;
        }
        //return e_success;
    }
    else
    {
        printf("Open file is Failure\n");
        return e_failure;
    }
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo_pst)
{
    encode_data_to_image(MAGIC_STRING,strlen(magic_string),encInfo_pst->fptr_src_image,encInfo_pst->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn_size(int extn_size, EncodeInfo *encInfo_pst)
{
    //printf("extn_size is %d\n",extn_size);
     char buffer[32];
     fread(buffer,32,1,encInfo_pst->fptr_src_image);
    encode_size_to_lsb(extn_size,buffer);
    // for(int i=0;i<32;i++)
    // {
    //     printf("buffer[%d] => %d\n",i,buffer[i]);
    // }
    fwrite(buffer,32,1,encInfo_pst->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo_pst)
{
    encode_data_to_image(file_extn,strlen(file_extn),encInfo_pst->fptr_src_image,encInfo_pst->fptr_stego_image);
    return e_success;
}

 Status encode_secret_file_size(long file_size, EncodeInfo *encInfo_pst)
 {
    char buffer[32];
    fread(buffer,32,1,encInfo_pst->fptr_src_image);
    encode_size_to_lsb(file_size,buffer);
    fwrite(buffer,32,1,encInfo_pst->fptr_stego_image);
    return e_success;
 }

 Status encode_secret_file_data(EncodeInfo *encInfo_pst)
 {
    char buffer[encInfo_pst->size_secret_file];
    rewind(encInfo_pst->fptr_secret);
    //printf("file pos -> %ld\n",ftell(encInfo_pst->fptr_src_image));
    fread(buffer,encInfo_pst->size_secret_file,1,encInfo_pst->fptr_secret);
    encode_data_to_image(buffer,encInfo_pst->size_secret_file,encInfo_pst->fptr_src_image,encInfo_pst->fptr_stego_image);
     //  printf("file pos -> %ld\n",ftell(encInfo_pst->fptr_src_image));
    return e_success;
 }

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];

    for(int i = 0;i < size;i++)
    {
        fread(buffer,8,1,fptr_src_image);
        encode_byte_to_lsb(data[i],buffer);
        
        fwrite(buffer,8,1,fptr_stego_image);
    }
    ////return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    char store[8];
   // printf("data -> %c\n",data);
    for(int i = 0;i <= 7;i++)
    {
        image_buffer[i] = ((data & (1 << (7-i))) >> (7-i) ) | (image_buffer[i] & 0xFE);
    }
    // for(int i = 0;i <= 7;i++)
    // {
    //     image_buffer[i] = store[i];
    // }
   // return e_success;
}

Status encode_size_to_lsb(int size, char *image_buffer)
{
    for(int i = 0;i < 32;i++)
    {
        image_buffer[i] = ((size & (1 << (31-i))) >> (31-i) ) | image_buffer[i] & 0xFE;
    }
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while((fread(&ch,1,1,fptr_src)) > 0)
    {
        fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}