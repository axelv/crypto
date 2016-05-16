         .         .                                                                             
         ,8.       ,8.          8 888888888o.            .8.          8 8888888888     d888888o.  
        ,888.     ,888.         8 8888    `88.          .888.         8 8888         .`8888:' `88.
       .`8888.   .`8888.        8 8888     `88         :88888.        8 8888         8.`8888.   Y8
      ,8.`8888. ,8.`8888.       8 8888     ,88        . `88888.       8 8888         `8.`8888.    
     ,8'8.`8888,8^8.`8888.      8 8888.   ,88'       .8. `88888.      8 888888888888  `8.`8888.   
    ,8' `8.`8888' `8.`8888.     8 888888888P'       .8`8. `88888.     8 8888           `8.`8888.  
   ,8'   `8.`88'   `8.`8888.    8 8888`8b          .8' `8. `88888.    8 8888            `8.`8888. 
  ,8'     `8.`'     `8.`8888.   8 8888 `8b.       .8'   `8. `88888.   8 8888        8b   `8.`8888.
 ,8'       `8        `8.`8888.  8 8888   `8b.    .888888888. `88888.  8 8888        `8b.  ;8.`8888
,8'         `         `8.`8888. 8 8888     `88. .8'       `8. `88888. 8 888888888888 `Y8888P ,88P'
                                                                                                  
         .8.          `8.`8888.      ,8' 8 8888888888   8 8888 `8.`888b           ,8'             
        .888.          `8.`8888.    ,8'  8 8888         8 8888  `8.`888b         ,8'              
       :88888.          `8.`8888.  ,8'   8 8888         8 8888   `8.`888b       ,8'               
      . `88888.          `8.`8888.,8'    8 8888         8 8888    `8.`888b     ,8'                
     .8. `88888.          `8.`88888'     8 888888888888 8 8888     `8.`888b   ,8'                 
    .8`8. `88888.         .88.`8888.     8 8888         8 8888      `8.`888b ,8'        2016   
   .8' `8. `88888.       .8'`8.`8888.    8 8888         8 8888       `8.`888b8'                   
  .8'   `8. `88888.     .8'  `8.`8888.   8 8888         8 8888        `8.`888'        CRYPTO 1          
 .888888888. `88888.   .8'    `8.`8888.  8 8888         8 8888         `8.`8'                     
.8'       `8. `88888. .8'      `8.`8888. 8 888888888888 8 888888888888  `8.`                      
--------------------------------------------------------------------------------------------------------
Michael Raes (michael.raes@student.kuleuven.be)
Axel Vanraes (axel.vanraes@student.kuleuven.be)

01. Intro
This directory contains the code handed in for the P&D Embedded Systems and Multimedia project.
It can be compiled both for x86 and for the Texas Instruments DSP provided for this course.
We were team CRYPTO 1 and we worked together with SPEECH 1, which consists of Louis Vanbiervliet and
Gert Millis. All non-crypto code here was written or sourced by them and not necessarily their
latest version. It was just included to show the integrated (SPEECH+CRYPTO) functionality.

02. Relevant crypto files
asymmetric/ : contains all code for doing public key mathematics and signing.
bbs/	    : code for the Blum Blum Shub CSPRNG.
ocb/ 	    : code implementing the OCB AES block cipher mode.
packman/    : handles packet creation, verification,...
sts/        : implements the Station to Station protocol.
tools/      : some extra functions for convenience.
constants.h : some crypto constants.

main.c            : demo of the key agreement, encryption/decryption functionality on one data buffer. 
main_integration.c: demo of the total integrated functionality (SPEECH+CRYPTO)
crypto_wrapper.c&h: needed for the above demos.

03. Other files
All other files (with exclusion of the SBC directory) are used in the TI Code Composer Studio.

04. Building
First run make clean

For the integrated code run:
make integration
./integration

For the crypto-only code run:
make crypto
./crypto
