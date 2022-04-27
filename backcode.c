/*
 * backcode.c
 *
 *  Created on: Apr 14, 2022
 *      Author: BillyC
 */

#include <stdio.h>
#include <stdlib.h>
 #include "printmsg.h"

struct CPSR_fields {
     // LSB

    uint32_t mode:5;
    uint32_t  T:1;
    uint32_t  FIQ:1;
    uint32_t  IRQ:1;
    uint32_t  A:1;
    uint32_t  E:1;
    uint32_t  IT6:6;
    uint32_t  GE:4;
    uint32_t  DNM:4;
    uint32_t  J:1;
    uint32_t  IT2:2;
    uint32_t  Q:1;
    uint32_t  V:1;
    uint32_t  C:1;
    uint32_t  Z:1;
    uint32_t  N:1;

     // MSB
  };


typedef union CPSR_reg_tag {
	uint32_t CPSR_raw;
    struct CPSR_fields fields;
} CPSR_reg_t;


CPSR_reg_t cpsr;
cpsr.fields.mode = 0b11111;





struct fields_tag {
     // LSB
    unsigned int three:3;
    unsigned int five:5;
    unsigned int a:1;
    unsigned int b:1;
    unsigned int c:1;
    unsigned int d:1;
    unsigned int e:1;
    unsigned int f:1;
    unsigned int g:1;
    unsigned int h:1;

     // MSB
  };

  // here is a tag and typedef for less typing
  // to modify the 8-bit value as a whole
  // and read in parts.
  typedef union some_reg_tag {
      uint16_t raw;
      struct fields_tag fields;
  } some_reg_t;

  some_reg_t a_register;
  a_register.fields.five = 5;
  a_register.fields.three = 3;
  a_register.fields.a =1;
  a_register.fields.b =1;
  a_register.fields.c =1;
  a_register.fields.d =1;
  a_register.fields.e =1;
  a_register.fields.f =1;
  a_register.fields.g =1;
  a_register.fields.h =1;





printf("some reg = %x\n\r",a_register.raw);
printf("some reg.three =  %x\n\r",a_register.fields.three);
printf("some reg.five =  %x\n\r",a_register.fields.five);

printf("some reg.a =  %x\n\r",a_register.fields.a);
printf("some reg.b =  %x\n\r",a_register.fields.b);
printf("some reg.c =  %x\n\r",a_register.fields.c);
printf("some reg.d =  %x\n\r",a_register.fields.d);
printf("some reg.e =  %x\n\r",a_register.fields.e);
printf("some reg.f =  %x\n\r",a_register.fields.f);
printf("some reg.g =  %x\n\r",a_register.fields.g);
printf("some reg.h =  %x\n\r",a_register.fields.h);



  //Flash_Read_Data(FLASH_SECTOR3_BASE_ADDRESS ,(uint32_t *) &Rx_Data, 10);

