#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //checking argv[2] arguement having .bmp extension or not
    char* substr1=strstr(argv[2],".bmp");
    //checking substr is having .bmp file extension or not
    if(substr1!=NULL)
    { //checking string(.bmp) is present or not?
        if((strcmp(substr1,".bmp"))!=0)
        {
            printf("Invalid destination image file name extension\n");
            return e_failure;
        }
        else
        {
            //arguement is valid so storing into structure src_image_fname
           decInfo->decode_dest_fname=argv[2]; 
        }
    }
    else
    {
        printf("Invalid destination image file name extension\n");
        return e_failure;
    }

    //checking argv[3] arguement is not have any extension
    //if the extension is there then remove using strtok function
    if((strchr(argv[3],'.'))!=NULL)
    {
        strtok(argv[3], ".");   // split at '.'
    }
    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    //open dest image file in read mode
    decInfo->decode_dest_fptr=fopen(decInfo->decode_dest_fname,"r");

    //error checking
    if (decInfo->decode_dest_fptr == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decode_dest_fname);

        return e_failure;
    }
    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    //rewinding the storing secrete data 
    rewind(decInfo->decode_dest_fptr);
    //setting the offset into 54th place bcz of header
    fseek(decInfo->decode_dest_fptr, 54, SEEK_SET);
    unsigned char imageBuffer[8];
    unsigned char character[8];
    unsigned char ch;
    //loop upto the magic string
    for(int i=0;magic_string[i]!='\0';i++)
    {
        ch=0;
        //reading 8 bytes from the encoded image file
        fread(imageBuffer,sizeof(char),8,decInfo->decode_dest_fptr);
        //call function to decode
        decode_byte_to_lsb(character,imageBuffer);
        //setting the bits bcz we used the method LSB to MSB
        for(int i=0;i<8;i++)
        {
            ch = ch|(character[i] << i);
        }
        //checking the decode is correct or not by checking the character
        if(ch!=magic_string[i])
        {
            printf("Magic string decoding is not successfull\n");
            return e_failure;
        }
    }
    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    unsigned char imageBuffer[32];
    unsigned char character[32];
    int dec_ext_size=0;
    //reading 32 bytes from dest image file
    fread(imageBuffer,sizeof(char),32,decInfo->decode_dest_fptr);
    //caling fuction to decode the 32 bytes
    decode_size_to_lsb(character,imageBuffer);
    for(int i=0;i<32;i++)
    {
        //shifting the bits of decoded
        dec_ext_size=dec_ext_size | (character[i]<<i);
    }
    printf("secret file extension size:%d\n",dec_ext_size);
    //assigning decoded secret file extension size into the structure
    decInfo->decode_secret_extn_size=dec_ext_size;
    return e_success;
}

Status decode_secret_file_extn(char* argv[], DecodeInfo *decInfo)
{
    unsigned char imageBuffer[8];
    unsigned char character[8];
    unsigned char ch;
    char extension_string[5];
    int i;
    //loop upto decoded secret file extension size
    for(i=0;i<decInfo->decode_secret_extn_size;i++)
    {
        ch=0;
        //reading 8 bytes from dest image file
        fread(imageBuffer,sizeof(char),8,decInfo->decode_dest_fptr);
        decode_byte_to_lsb(character,imageBuffer);
        for(int i=0;i<8;i++)
        {
            //shifting the bits of decoded
            ch = ch|(character[i] << i);
        }
        extension_string[i]=ch;
    }
    //Assigning last character as NULL
    extension_string[i]='\0';
    //cancantenate the decoded secret file extension to 4th arguement
    decInfo->secrete_out_fname=strcat(argv[3],extension_string);
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    unsigned char imageBuffer[32];
    unsigned char character[32];
    long int dec_secrete_size=0;
     //reading 32 bytes from dest image file
    fread(imageBuffer,sizeof(char),32,decInfo->decode_dest_fptr);
    //caling fuction to decode the 32 bytes
    decode_size_to_lsb(character,imageBuffer);
    for(int i=0;i<32;i++)
    {
        dec_secrete_size=dec_secrete_size | (character[i]<<i);
    }
    printf("secret file data size:%ld\n",dec_secrete_size);
    //assigning the secret sile data size into the structure
    decInfo->secret_size_data=dec_secrete_size;
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    unsigned char imageBuffer[8];
    unsigned char character[8];
    unsigned char ch;
    char extension_string[5];
    int i;
    //loop upto the secret file data size
    for(i=0;i<decInfo->secret_size_data;i++)
    {
        ch=0;
        //reading binary 8 bits from dest image file
        fread(imageBuffer,sizeof(char),8,decInfo->decode_dest_fptr);
        decode_byte_to_lsb(character,imageBuffer);
        for(int i=0;i<8;i++)
        {
            ch = ch|(character[i] << i);
        }
        //printing the decoded character into the decoded output file
        fprintf(decInfo->secrete_out_fptr,"%c",ch);
    }
    return e_success;
}

Status decode_byte_to_lsb(char *character, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        //getting last bit from the image buffer and assigning into the character
        character[i]=image_buffer[i]&1;
    }
    return e_success;
}

Status decode_size_to_lsb(char *character, char *imageBuffer)
{
    for(int i=0;i<32;i++)
    {
        //getting last bit from the image buffer and assigning into the character
        character[i]=imageBuffer[i]&1;
    }
    return e_success;
}

Status do_decoding(char *argv[],DecodeInfo *decInfo)
{
    //function call to open a files
    if((open_decode_files(decInfo))==e_success)
    {
        printf("Opening file is completed\n");
        //function call to decode the magic string
        if((decode_magic_string(MAGIC_STRING,decInfo))==e_success)
        {
            printf("Decoding magic strings is completd\n");
            //function call to decode secret file extension size
            if((decode_secret_file_extn_size(decInfo))==e_success)
            {
                printf("Decoding secret file extension size is completed\n");
                //function call to decode secret file extension character by character
                if((decode_secret_file_extn(argv,decInfo))==e_success)
                {
                    //after decoding secret file extension and after concatenation open the file
                    //opening a file for secret out data
                    decInfo->secrete_out_fptr=fopen(decInfo->secrete_out_fname,"w");
                    printf("Decoding secret file extension successfully completed\n");
                    //checking the file is opened or not
                    if(decInfo->secrete_out_fptr==NULL)
                    {
                        return e_failure;
                    }
                    else
                    {
                        printf("Opening Secret data out file is successfully completed\n");
                        //function call to decode the secret file data size
                        if((decode_secret_file_size(decInfo))==e_success)
                        {
                            printf("Decoding secret file data size successfully completed\n");
                            //function call to decode secret file data
                            if((decode_secret_file_data(decInfo))==e_success)
                            {
                                printf("Decoding the secrete data successfilly completed\n");
                                //closing the all files
                                fclose(decInfo->decode_dest_fptr);
                                fclose(decInfo->secrete_out_fptr);
                                return e_success;
                            }
                        }
                    }
                }
            }
        }
    }
    return e_failure;
}