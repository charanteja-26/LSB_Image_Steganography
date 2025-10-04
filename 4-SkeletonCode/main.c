#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc,char *argv[])
{
     if(argc >= 2)
     {
          EncodeInfo encInfo;
          DecodeInfo decInfo;
          //uint img_size;
          //checking weather peforming for encoding or decoding
          OperationType ret = check_operation_type(argv); 
    
          if(ret == e_encode)
           {
               printf("Selected encoding\n");
          // OperationType ret = check_operation_type(argv);
               if(argc >=4)
               {
                    //read and validate arguments
                    if( read_and_validate_encode_args(argv, &encInfo) == e_success)
                    {
                         printf("Read and validate is success\n");
                         //start encoding
                         if(do_encoding(&encInfo) == e_success)
                         {
                              printf("Encoding is successful\n");
                         }
                         else
                         {
                              printf("Encoding is failed\n");
                         }
                    }
                    else
                    {
                         printf("Read and validate is failed\n");
                         printf("Please pass ./a.out -e beautiful.bmp secret.txt\n");
                         return -1;
                    }
               }
               else
               {
                    printf("Argument should be grater or equal to 4\n");
               }

          }
          
          
          else if(ret == e_decode)
          {
               printf("Selected decoding\n");
               if(argc >= 3)
               {
                    //read and validate arguments
                    if( read_and_validate_decode_args(argv, &decInfo) == e_success)
                    {
                         printf("Read and validate is success\n");
                         //start decoding
                         if(do_decoding(&decInfo) == e_success)
                         {
                              printf("Decoding is successful\n");
                         }
                         else
                         {
                              printf("Decoding is failed\n");
                         }
                    }
               }
               else
               {
                    printf("Argument should be grater or equal to 3\n");
               }
               
          }
          else
          {
               printf("Invalid input\n");
          }
    }
    else
    {
          printf("Argument should be grater or equal to 3\n");
    }
}

OperationType check_operation_type(char *argv[])
{
     if(strcmp(argv[1],"-e") == 0 )
     {
          return e_encode;
     }
     if(strcmp(argv[1],"-d") == 0 )
     {
          return e_decode;
     }
     else
     {
          return e_unsupported;
     }
}
