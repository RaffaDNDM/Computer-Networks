###################################################################
                BASE64 ENCRYPTION  & DECRYPTION
###################################################################
###################################################################
To compile the program you need to execute the makefile by typing:
- make 
or
- make base64

To erase executable file, you can type:
- make clean
###################################################################
###################################################################

******************************************************************
You can use the program by command line, specifying a filename
or execute the program, typing the string during the exectution
*******************************************************************
            The mandatory argument is either:
                 -d for the decryption
                         or
                 -e for the encryption

       You can type only one of them in the same time
*******************************************************************
If no filename is specified on the command line, the program waits
for the insertion of the string, on a line during the execution

If you specify a file in the input, the result of the execution of
the program will be in "output.txt"
*******************************************************************

Content of the folder

.
├── base64.c
├── base64.h
├── file.txt
├── makefile
└── README.txt


