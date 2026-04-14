/*Name : D. Yuvasree
Submission date : 24/01/2026
Project Name : Steganography
********************************************************************
Description :
This project helps to understand the concept of image steganography,
which is the technique of hiding secret information inside a digital
image without causing noticeable changes to the image.

The program uses the Least Significant Bit (LSB) method to embed
secret data into a BMP image. In this technique, the least significant
bit of each pixel byte is replaced with the bits of the secret data.
Since changes in the LSB do not significantly affect the image quality,
the hidden data remains invisible to the human eye.

The project supports both encoding and decoding operations.

Encoding Process :
- The source image is read in binary format.
- A magic string is embedded first to indicate the presence of hidden data.
- The size and extension of the secret file are encoded.
- The actual secret message data is embedded into the image pixels
  by modifying their LSBs.
- The remaining image data is copied without modification.
- A new stego-image is generated containing the hidden information.

Decoding Process :
- The stego-image is read.
- The magic string is extracted and verified to ensure valid steganography.
- The size and extension of the secret file are decoded.
- The hidden secret data is extracted from the LSBs of the image pixels.
- The recovered secret message is written into an output file.

This project demonstrates important concepts such as file handling,
bitwise operations, command-line arguments, and data security.
It provides a practical understanding of how information can be
securely concealed within digital media.
********************************************************************/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    //checking command arguements count
    if(argc>=4)
    {
        EncodeInfo encInfo;
        DecodeInfo decInfo;
        //calling operation checking function to check opeartion type
        //if command is -e returning e_encode
        if(check_operation_type(argv[1])==e_encode)
        {
            //checking all the arguements are valide are not if valid then receiving e_success
            if(read_and_validate_encode_args(argv,&encInfo)==e_success)
            {
                //calling encoding operation
                printf("\t\t\t##Encoding procedure started##\n");
                if((do_encoding(&encInfo))==e_success)
                {
                    printf("##Encoding successfully completed##\n");
                }
                else
                {
                    printf("##Encoding is not successfully completed##\n");
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else if(check_operation_type(argv[1])==e_decode)
        {
            //checking all the arguements are valide are not if valid then receiving e_success
            if(read_and_validate_decode_args(argv,&decInfo)==e_success)
            {
                //calling decoding operation
                printf("Reading and validating arguements is completed\n");
                printf("\t\t\t##Decoding procedure started##\n");
                if((do_decoding(argv,&decInfo))==e_success)
                {
                    printf("##Decoding successfully completed##\n");
                }
                else
                {
                    printf("##decoding is not completed\n");
                    return 0;
                }
            }
            else
            { 
                //some of the arguements extensiions are not valid so terminating program
                printf("Error: Invalid command-line arguments\n");
                return 0;   
            }
        }
        //command which is mentioning operation type is not valid 
        else if(check_operation_type(argv[1])==e_unsupported)
        {
            printf("Error: Invalid operation type\n");
            return 0;
        }
    }
    else
    {
        printf("INVALID Command arguements\n");
        printf("Pass Arguements like\n");
        printf("For Encoding: \n./a.out -e beautiful.bmp secret.txt\n");
        printf("For Decoding: \n./a.out -d dest.bmp secretdata\n");
        return 0;
    }
}
OperationType check_operation_type(char *symbol)
{
    //if opeartion command arguement is -e saying as do encode so returning e_encode
    if(strcmp(symbol,"-e")==0)
    {
        return e_encode;
    }
    //if opeartion command arguement is -d saying as do decode so returning e_decode
    else if(strcmp(symbol,"-d")==0)
    {
        return e_decode;
    }
    //command arguement which is mentioning opeartion is not valid so returning e_unsupported
    else
    {
        return e_unsupported;
    }
}
