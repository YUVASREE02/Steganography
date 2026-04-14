#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

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
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr,0,SEEK_END);
    long int length=ftell(fptr);
    return length;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //checking argv[2] arguement having .bmp extension or not
    char* substr1=strstr(argv[2],".bmp");
    //checking substr is having .bmp file extension or not
    if(substr1!=NULL)
    { //checking string(.bmp) is present or not?
        if((strcmp(substr1,".bmp"))!=0)
        {
            printf("Invalid source image file name extension\n");
            return e_failure;
        }
        else
        {
            //arguement is valid so storing into structure src_image_fname
           encInfo->src_image_fname=argv[2]; 
        }
    }
    else
    {
        printf("Invalid source image file name extension\n");
        return e_failure;
    }
    //checking 3rd arguement having .txt extension or not
    char* substr2=strstr(argv[3],".txt");
    char* substr2_1=strstr(argv[3],".c");
    char* substr2_2=strstr(argv[3],".sh");
    char* substr2_3=strstr(argv[3],".h");
    //checking substr is having .txt file extension or not
    if(substr2!=NULL)
    { //checking string(.txt) is present or not?
        if((strcmp(substr2,".txt"))!=0)
        {
            printf("Invalid secrete file name extension\n");
            return e_failure;
        }
        else
        {
            //arguement is valid so storing into structure secrete_fname
            encInfo->secret_fname=argv[3]; 
        }
    }
    
    else if(substr2_1!=NULL)
    { 
        //checking string(.c) is present or not?
        if((strcmp(substr2_1,".c"))!=0)
        {
            printf("Invalid secrete file name extension\n");
            return e_failure;
        }
        else
        {
            //arguement is valid so storing into structure secrete_fname
            encInfo->secret_fname=argv[3]; 
        }
    }
    
    else if(substr2_2!=NULL)
    { 
        //checking string(.sh) is present or not?
        if((strcmp(substr2_2,".sh"))!=0)
        {
            printf("Invalid secrete file name extension\n");
            return e_failure;
        }
        else
        {
            //arguement is valid so storing into structure secrete_fname
            encInfo->secret_fname=argv[3]; 
        }
    }
    
    else if(substr2_3!=NULL)
    { 
        //checking string(.h) is present or not?
        if((strcmp(substr2_3,".h"))!=0)
        {
            printf("Invalid secrete file name extension\n");
            return e_failure;
        }
        else
        {
            //arguement is valid so storing into structure secrete_fname
            encInfo->secret_fname=argv[3]; 
        }
    }
    else
    {
        printf("Invalid secrete file name extension\n");
        return e_failure;
    }
    //checking 4th arguement is having NULL or not
    if(argv[4]!=NULL)
    {
        //4th arguement is not NULL so check it is having .bmp extension or not
        char* substr3=strstr(argv[4],".bmp");
        //checking substr is having .bmp file extension or not
        if(substr3!=NULL)
        { //checking string(.bmp) is present or not?
            if((strcmp(substr3,".bmp"))!=0)
            {
                printf("Invalid destination image file name extension\n");
                return e_failure;
            }
            else
            {
                //arguement is valid so storing into structure dest_image_fname
                encInfo->dest_image_fname=argv[4]; 
            }
        }
        else
        {
            printf("Invalid source image file name extension\n");
            return e_failure;
        }
    }
    //4th arguement is NULL, storing default name
    else
    {
        //storing default file name structure dest_image_fname
        encInfo->dest_image_fname="dest.bmp";
    }
    //all arguements are valide so returning e_success
    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // dest Image file
    encInfo->fptr_dest_image = fopen(encInfo->dest_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->dest_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    //function call for getting source image file size
    uint image_size=get_image_size_for_bmp(encInfo->fptr_src_image);  
    printf("Image capacity:%u\n",image_size);
    //function call for getting secrete message file
    uint secrete_file_size=get_file_size(encInfo->fptr_secret);
    encInfo->size_secret_file=secrete_file_size;
    //checking is it image file size is greater than secrete file size or not
    if(image_size>(16+32+32+32+secrete_file_size*8))
    {
        return e_success;
    }
    else
    {
        printf("File capacity is not suitable\n");
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char imageBuffer[54];
    rewind(fptr_src_image);
    rewind(fptr_dest_image);
    //reading binary 54 values from source image file
    fread(imageBuffer,sizeof(char),54,fptr_src_image);
    long int pos1=ftell(fptr_src_image);
    //writing binary 54 values from imageBuffer array of characters to destination image file
    fwrite(imageBuffer,sizeof(char),54,fptr_dest_image);
    long int pos2=ftell(fptr_dest_image);
    //checking offset of both source image file and destination image file
    if(pos1==pos2)
    {
        return e_success;
    }
    else
    {
        printf("Error : copying Header file from source image file to destination is not completed\n");
        return e_failure;
    }
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char imageBuffer[8];
    for(int i=0;magic_string[i]!='\0';i++)
    {
        //reading 8 bytes from source image file to imagebuffer
        fread(imageBuffer,sizeof(char),8,encInfo->fptr_src_image);
        //calling function for encoding magic_string
        encode_byte_to_lsb(magic_string[i], imageBuffer);
        //writing 8 bytes from imageBuffer to destination image file
        fwrite(imageBuffer,sizeof(char),8,encInfo->fptr_dest_image);
    }
    //checking properly copying is completed or not
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
    {
        return e_success;
    }
    else
    {
        printf("Error: Copying magic_string is not completed\n");
        return e_failure;
    }

}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char imageBuffer[32];
    //reading 32 bytes from source image file
    fread(imageBuffer,sizeof(char),32,encInfo->fptr_src_image);
    //caling function for encoding the size of the extension file
    encode_size_to_lsb(size,imageBuffer);
    //writing 32 bytes from imageBuffer to destination image file
    fwrite(imageBuffer,sizeof(char),32,encInfo->fptr_dest_image);
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
        return e_success;
    else
    {
        printf("Error: Copying secrete file extension size is not completed\n");
        return e_failure;
    }
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char imageBuffer[8];
    for(int i=0;file_extn[i]!='\0';i++)
    {
        //reading 8 bytes from source image file
        fread(imageBuffer,sizeof(char),8,encInfo->fptr_src_image);
        //caling function for encoding the characters of the extension file
        encode_byte_to_lsb(file_extn[i],imageBuffer);
        //writing 8 bytes from imageBuffer to destination image file
        fwrite(imageBuffer,sizeof(char),8,encInfo->fptr_dest_image);
    }
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
        return e_success;
    else
    {
        printf("Error: Copying secrete file extension is not completed\n");
        return e_failure;
    }
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char imageBuffer[32];
    //reading 32 bytes from source image file
    fread(imageBuffer,sizeof(char),32,encInfo->fptr_src_image);
    //caling function for encoding the size of the extension file
    encode_size_to_lsb(file_size,imageBuffer);
    //writing 32 bytes from imageBuffer to destination image file
    fwrite(imageBuffer,sizeof(char),32,encInfo->fptr_dest_image);
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
        return e_success;
    else
    {
        printf("Error: Copying secret file size is not completed\n");
        return e_failure;
    }
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    //copying secrete data into an array
    char secrete_data_file[encInfo->size_secret_file];
    fread(secrete_data_file,sizeof(char),encInfo->size_secret_file,encInfo->fptr_secret);
    char imageBuffer[8];
    //looping upto secrete_data_file length
    for(int j=0;secrete_data_file[j]!='\0';j++)
    {
        //reading binary values from source image file
        fread(imageBuffer,sizeof(char),8,encInfo->fptr_src_image);
        //encoding the secrete file data
        encode_byte_to_lsb(secrete_data_file[j],imageBuffer);
        //writing imageBuffer into destination file name
        fwrite(imageBuffer,sizeof(char),8,encInfo->fptr_dest_image);
    }
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
        return e_success;
    else
    {
        printf("Error: Copying secrete file data is not completed\n");
        return e_failure;
    }
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char imageBuffer[32];
    //looping src image file upto 0
    //reading 8 bytes from source image file
    while ((fread(imageBuffer, sizeof(char), 1, fptr_src)) != 0)
    {
        //copying imageBuffer to dest image file
        fwrite(imageBuffer, sizeof(char), 1, fptr_dest);
    }
    //printf("Offset of source image file : %ld\n",ftell(fptr_src));
    //printf("Offset of destination image file : %ld\n",ftell(fptr_dest));
    if(ftell(fptr_src) == ftell(fptr_dest))
        return e_success;
    else
    {
        printf("Error: Copying remaining data is not completed\n");
        return e_failure;
    }
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        //get single bit from data
        int data_get=(data&(1<<i))>>i;
        //clear imageBuffer LSB bit from imageBuffer
        image_buffer[i]=image_buffer[i]&(~1);
        //set bit 
        image_buffer[i]=image_buffer[i]|data_get;
    }
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for(int i=0;i<32;i++)
    {
        //get single bit from size(4)
        int size_get=(size&(1<<i))>>i;
        //clear the LSB bit in image buffer
        imageBuffer[i]=imageBuffer[i]&(~1);
        //set the bit 
        imageBuffer[i]=imageBuffer[i] | size_get;
    }
}

Status do_encoding(EncodeInfo *encInfo)
{
    // opening a file and storing into FILE pointer
    if(open_files(encInfo)==e_success)
    { 
        printf("Opeaning all files successfully completed\n");
        //checking capacity of the image storing files
        if((check_capacity(encInfo))==e_success)
        { 
            printf("Capacity of the source image file is greater so we can encode the data\n");
            //copying bmp header from source file to destination file
            if((copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_dest_image))==e_success)
            { 
                printf("Copying header data is completed\n");
                //encoding magic string to destination file
                if((encode_magic_string(MAGIC_STRING,encInfo))==e_success)
                { 
                    printf("Encoding maging string is successfull\n");
                    //copying secrete file extension into extn_secret_file[5]
                    char* extn_character=strchr(encInfo->secret_fname,'.');
                    strcpy(encInfo->extn_secret_file,extn_character);
                    int size=strlen(encInfo->extn_secret_file);
                    encInfo->size_secret_file_ext=size;
                    //encoding secrete file extension size 
                    if((encode_secret_file_extn_size(size, encInfo))==e_success)
                    { 
                        printf("Encoding secrete file extension file size is successful\n");
                        //encoding secrete file extension 
                        if((encode_secret_file_extn(encInfo->extn_secret_file, encInfo))==e_success)
                        { 
                            printf("Encoding secrete file extension is successfull\n");
                            //encoding length of the secrete data
                            if((encode_secret_file_size(encInfo->size_secret_file, encInfo))==e_success)
                            { 
                                printf("Encoding secrete file size is successful\n");
                                //encoding secrete file data
                               if((encode_secret_file_data(encInfo))==e_success)
                               {  
                                printf("Encoding secrete file data is successful\n");
                                    //copying remaining data into destination file
                                    if((copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_dest_image))==e_success)
                                    {
                                        fclose(encInfo->fptr_src_image);
                                        fclose(encInfo->fptr_secret);
                                        fclose(encInfo->fptr_dest_image);
                                        return e_success;
                                    }
                               }
                            }
                        }
                    }
                }
            }
        }
    }
    return e_failure;
}
