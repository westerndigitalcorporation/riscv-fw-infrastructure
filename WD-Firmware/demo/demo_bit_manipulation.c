
/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2020 Western Digital Corporation or its affiliates.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http:*www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/**
* @file   demo_bit_manipulation.c
* @author Nati Rapaport
* @date   23.07.2020
* @brief  Demo application for Bit-Manipulation operations for EH2 and EL2
*/

/**
* include files
*/
#include "psp_api.h"
#include "demo_platform_al.h"
#include "demo_utils.h"

/**
* definitions
*/
#define D_DEMO_FFS_INPUT_NUM         0x02F4B203 /* 0000 0010 1111 0100 1011 0010 0000 0011*/
#define D_DEMO_FFS_EXPECTED_RESULT   (31 - 6)   /* 25 i.e. 31 - clz(D_DEMO_FFS_INPUT_NUM) */
/*----------------------------------*/
#define D_DEMO_FLS_INPUT_NUM         0x05681A00 /* 0000 0101 0110 1000 0001 1010 0000 0000 */
#define D_DEMO_FLS_EXPECTED_RESULT   (31 - 9)   /* 22 i.e. 31 - ctz(D_DEMO_FLS_INPUT_NUM) */
/*----------------------------------*/
#define D_DEMO_FFS_FLS_UNSUPPORTED_VALUE              0  /* FFS and FLS does not support input = 0 */
#define D_DEMO_FFS_FLS_RESULT_FOR_UNSUPPORTED_VALUE   -1 /* In this case, the returned value is -1 */
/*----------------------------------*/
#define D_DEMO_CLZ_INPUT_NUM         0x0007B79F /* 0000 0000 0000 0111 1011 0111 1001 1111 */
#define D_DEMO_CLZ_EXPECTED_RESULT   13         /* number of zeros until 1'st '1' at the most significant side of the number */
/*----------------------------------*/
#define D_DEMO_CTZ_INPUT_NUM         0x6FA89020 /* 0110 1111 1010 1000 1001 0000 0010 0000 */
#define D_DEMO_CTZ_EXPECTED_RESULT   5          /* number of zeros until 1'st '1' at the least significant side of the number */
/*----------------------------------*/
#define D_DEMO_PCNT_INPUT_NUM        0x6FA89020 /* 0110 1111 1010 1000 1001 0000 0010 0000 */
#define D_DEMO_PCNT_EXPECTED_RESULT  12         /* number of zeros in the number */
/*----------------------------------*/
#define D_DEMO_ANDN_1ST_ARG          0xFF07B5AC /* 1111 1111 0000 0111 1011 0101 1010 1100 */
#define D_DEMO_ANDN_2ND_ARG          0x89B3E6CF /* 1000 1001 1011 0011 1110 0110 1100 1111 */
#define D_DEMO_ANDN_EXPECTED_RESULT  0x76041120 /* bitwise AND between 1'st number and 2'nd inverted number */
/*----------------------------------*/
#define D_DEMO_ORN_1ST_ARG           0xFF07B5AC /* 1111 1111 0000 0111 1011 0101 1010 1100 */
#define D_DEMO_ORN_2ND_ARG           0x89B3E6CF /* 1000 1001 1011 0011 1110 0110 1100 1111 */
#define D_DEMO_ORN_EXPECTED_RESULT   0xFF4FBDBC /* bitwise OR between 1'st number and 2'nd inverted number */
/*----------------------------------*/
#define D_DEMO_XNOR_1ST_ARG          0xFF07B5AC /* 1111 1111 0000 0111 1011 0101 1010 1100 */
#define D_DEMO_XNOR_2ND_ARG          0x89B3E6CF /* 1000 1001 1011 0011 1110 0110 1100 1111 */
#define D_DEMO_XNOR_EXPECTED_RESULT  0x894BAC9C /* bitwise XOR between 1'st number and 2'nd inverted number */
/*----------------------------------*/
#define D_DEMO_MIN_1ST_ARG           0x8000FFAB /* when treated as signed integer, this number = -65444 */
#define D_DEMO_MIN_2ND_ARG           0x0FFF0B65 /* when treated as signed integer, this number = 268372837 */
#define D_DEMO_MIN_EXPECTED_RESULT   0x8000FFAB /* when treated as signed integer, -65444 is the minimum */
/*----------------------------------*/
#define D_DEMO_MAX_1ST_ARG           0x8000FFAB /* when treated as signed integer, this number = -65444 */
#define D_DEMO_MAX_2ND_ARG           0x0FFF0B65 /* when treated as signed integer, this number = 268372837 */
#define D_DEMO_MAX_EXPECTED_RESULT   0x0FFF0B65 /* when treated as signed integer, 268372837 is the maximum */
/*----------------------------------*/
#define D_DEMO_MINU_1ST_ARG          0x8000FFAB /* when treated as unsigned integer, this number = 2147549099 */
#define D_DEMO_MINU_2ND_ARG          0x0FFF0B65 /* when treated as unsigned integer, this number = 268372837 */
#define D_DEMO_MINU_EXPECTED_RESULT  0x0FFF0B65 /* when treated as unsigned integer, 268372837 is the minimum */
/*----------------------------------*/
#define D_DEMO_MAXU_1ST_ARG          0x8000FFAB /* when treated as unsigned integer, this number = 2147549099 */
#define D_DEMO_MAXU_2ND_ARG          0x0FFF0B65 /* when treated as unsigned integer, this number = 268372837 */
#define D_DEMO_MAXU_EXPECTED_RESULT  0x8000FFAB /* when treated as unsigned integer, 2147549099 is the maximum */
/*----------------------------------*/
#if 0 /* Currently we do not have an example for inpuuut & output for sextb and sexth operations */
 #define D_DEMO_SEXTB_INPUT_NUM       0
 #define D_DEMO_SEXTB_EXPECTED_RESULT 0
/*----------------------------------*/
 #define D_DEMO_SEXTH_INPUT_NUM       0
 #define D_DEMO_SEXTH_EXPECTED_RESULT 0
#endif
/*----------------------------------*/
#define D_DEMO_PACK_1ST_ARG          0x12345678
#define D_DEMO_PACK_2ND_ARG          0x90ABCDEF
#define D_DEMO_PACK_EXPECTED_RESULT  0xCDEF5678 /* pack lower halves of 1'st and 2'nd arguments. 1'st argument half in the least significant part */
/*----------------------------------*/
#define D_DEMO_PACKU_1ST_ARG         0x12345678
#define D_DEMO_PACKU_2ND_ARG         0x90ABCDEF
#define D_DEMO_PACKU_EXPECTED_RESULT 0x90AB1234 /* pack upper halves of 1'st and 2'nd arguments. 1'st argument half in the least significant part */
/*----------------------------------*/
#define D_DEMO_PACKH_1ST_ARG         0x12345678
#define D_DEMO_PACKH_2ND_ARG         0x90ABCDEF
#define D_DEMO_PACKH_EXPECTED_RESULT 0x0000EF78 /* pack least significant bytes of 1'st and 2'nd arguments in least significant 16 bits of result argument. zero the rest */
/*----------------------------------*/
#define D_DEMO_ROTATEL_1ST_ARG         0x05670FB1
#define D_DEMO_NUM_OF_LEFT_ROTATIONS   8
#define D_DEMO_ROTATEL_EXPECTED_RESULT 0x670FB105 /* rotate-left the argument bits while shift in the bits from the opposite side, in order */
/*----------------------------------*/
#define D_DEMO_ROTATER_1ST_ARG         0x0000FFFB
#define D_DEMO_NUM_OF_RIGHT_ROTATIONS  4
#define D_DEMO_ROTATER_EXPECTED_RESULT 0xB0000FFF /* rotate-right the argument bits while shift in the bits from the opposite side, in order */
/*----------------------------------*/
#define D_DEMO_REVERSE_INPUT_NUM         0xB00A8561 /* 10110000000010101000010101100001 - reverse the bits in this 32bits argument (i.e. swap bits 0-31, 1-30, 2-29 etc.) */
#define D_DEMO_REVERSE_EXPECTED_RESULT   0x86A1500D /* 10000110101000010101000000001101 - result of the reverse */
/*----------------------------------*/
#define D_DEMO_REVERSE_8_INPUT_NUM       0xA6B23D25 /* swap the bytes in every word in this 32bit argument */
#define D_DEMO_REVERSE_8_EXPECTED_RESULT 0x253DB2A6 /* result of the swap */
/*----------------------------------*/
#define D_DEMO_ORCB_INPUT_NUM            0 /*  */ /* [Nati to do] complete with inputs and expected results for orc.b and orc.16 operations */
#define D_DEMO_ORCB_EXPECTED_RESULT      0 /* result of the orcb */
/*----------------------------------*/
#define D_DEMO_ORC_16_INPUT_NUM          0 /*  */
#define D_DEMO_ORC_16_EXPECTED_RESULT    0 /* result of the orc16 */
/*----------------------------------*/
#define D_DEMO_SBSET_1ST_ARG             0xF101010F /* 11110001000000010000000100001111 */
#define D_DEMO_SBSET_BIT_POSITION        10         /* Set bit #10 */
#define D_DEMO_SBSET_EXPECTED_RESULT     0xF101050F /* 11110001000000010000010100001111 */
/*----------------------------------*/
#define D_DEMO_SBCLR_1ST_ARG             0xDC3CFFFF /* 11011100001111001111111111111111 */
#define D_DEMO_SBCLR_BIT_POSITION        20         /* Clear bit #20 */
#define D_DEMO_SBCLR_EXPECTED_RESULT     0xDC2CFFFF /* 11011100001011001111111111111111 */
/*----------------------------------*/
#define D_DEMO_SBINVERT_1ST_ARG          0x44F03500 /* 01000100111100000011010100000000 */
#define D_DEMO_SBINVERT_BIT_POSITION     0          /* Invert bit #0 */
#define D_DEMO_SBINVERT_EXPECTED_RESULT  0x44F03501 /* 01000100111100000011010100000001 */
/*----------------------------------*/
#define D_DEMO_SBEXTRACT_1ST_ARG         0xF09F4CD2 /* 11110000100111110100110011010010 */
#define D_DEMO_SBEXTRACT_BIT_POSITION    25         /* Extract bit #25 */
#define D_DEMO_SBEXTRACT_EXPECTED_RESULT 0
/*----------------------------------*/
/**
* macros
*/

/**
* types
*/

/**
* local prototypes
*/

/**
* external prototypes
*/

/**
* global variables
*/

/**
* APIs
*/
void demoBitManipulation(void)
{
  u32_t uiResult;

  /* Find First Set */
  M_PSP_BITMANIP_FFS(D_DEMO_FFS_INPUT_NUM, uiResult);
  if (D_DEMO_FFS_EXPECTED_RESULT != uiResult)
  {
      M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* Find First Set - with 0 as input: expect to get -1 */
  M_PSP_BITMANIP_FFS(D_DEMO_FFS_FLS_UNSUPPORTED_VALUE, uiResult);
  if (D_DEMO_FFS_FLS_RESULT_FOR_UNSUPPORTED_VALUE != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* Find Last Set */
  M_PSP_BITMANIP_FLS(D_DEMO_FLS_INPUT_NUM, uiResult);
  if (D_DEMO_FLS_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* Find Last Set - with 0 as input: expect to get -1 */
  M_PSP_BITMANIP_FLS(D_DEMO_FFS_FLS_UNSUPPORTED_VALUE, uiResult);
  if (D_DEMO_FFS_FLS_RESULT_FOR_UNSUPPORTED_VALUE != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* clz */
  M_PSP_BITMANIP_CLZ(D_DEMO_CLZ_INPUT_NUM, uiResult);
  if (D_DEMO_CLZ_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* ctz */
  M_PSP_BITMANIP_CTZ(D_DEMO_CTZ_INPUT_NUM, uiResult);
  if (D_DEMO_CTZ_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* pcnt*/
  M_PSP_BITMANIP_PCNT(D_DEMO_PCNT_INPUT_NUM, uiResult);
  if (D_DEMO_PCNT_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* andN */
  M_PSP_BITMANIP_ANDN(D_DEMO_ANDN_1ST_ARG, D_DEMO_ANDN_2ND_ARG, uiResult);
  if (D_DEMO_ANDN_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* orN */
  M_PSP_BITMANIP_ORN(D_DEMO_ORN_1ST_ARG, D_DEMO_ORN_2ND_ARG, uiResult);
  if (D_DEMO_ORN_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* xNor */
  M_PSP_BITMANIP_XNOR(D_DEMO_XNOR_1ST_ARG, D_DEMO_XNOR_2ND_ARG, uiResult);
  if (D_DEMO_XNOR_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* min */
  M_PSP_BITMANIP_MIN(D_DEMO_MIN_1ST_ARG, D_DEMO_MIN_2ND_ARG, uiResult);
  if (D_DEMO_MIN_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* max */
  M_PSP_BITMANIP_MAX(D_DEMO_MAX_1ST_ARG, D_DEMO_MAX_2ND_ARG, uiResult);
  if (D_DEMO_MAX_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }


  /* minu */
  M_PSP_BITMANIP_MINU(D_DEMO_MINU_1ST_ARG, D_DEMO_MINU_2ND_ARG, uiResult);
  if (D_DEMO_MINU_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* maxu */
  M_PSP_BITMANIP_MAXU(D_DEMO_MAXU_1ST_ARG, D_DEMO_MAXU_2ND_ARG, uiResult);
  if (D_DEMO_MAXU_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

#if 0 /* Currently we do not have an example for input & output for sextb and sexth operations */
  /* sextb */
  M_PSP_BITMANIP_SEXTB(D_DEMO_SEXTB_INPUT_NUM, uiResult);
  if (D_DEMO_SEXTB_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* sexth */
  M_PSP_BITMANIP_SEXTH(D_DEMO_SEXTH_INPUT_NUM, uiResult);
  if (D_DEMO_SEXTH_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }
#endif

  /* pack */
  M_PSP_BITMANIP_PACK(D_DEMO_PACK_1ST_ARG, D_DEMO_PACK_2ND_ARG, uiResult);
  if (D_DEMO_PACK_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* packu */
  M_PSP_BITMANIP_PACKU(D_DEMO_PACKU_1ST_ARG, D_DEMO_PACKU_2ND_ARG, uiResult);
  if (D_DEMO_PACKU_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* packh */
  M_PSP_BITMANIP_PACKH(D_DEMO_PACKH_1ST_ARG, D_DEMO_PACKH_2ND_ARG, uiResult);
  if (D_DEMO_PACKH_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* rotate left */
  M_PSP_BITMANIP_ROL(D_DEMO_ROTATEL_1ST_ARG, D_DEMO_NUM_OF_LEFT_ROTATIONS,uiResult);
  if (D_DEMO_ROTATEL_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* rotate right */
  M_PSP_BITMANIP_ROR(D_DEMO_ROTATER_1ST_ARG, D_DEMO_NUM_OF_RIGHT_ROTATIONS,uiResult);
  if (D_DEMO_ROTATER_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* rotate right immediate */
  M_PSP_BITMANIP_RORI(D_DEMO_ROTATER_1ST_ARG, D_DEMO_NUM_OF_RIGHT_ROTATIONS,uiResult);
  if (D_DEMO_ROTATER_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* reverse */
  M_PSP_BITMANIP_REV(D_DEMO_REVERSE_INPUT_NUM, uiResult);
  if (D_DEMO_REVERSE_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* reverse8 */
  M_PSP_BITMANIP_REV8(D_DEMO_REVERSE_8_INPUT_NUM, uiResult);
  if (D_DEMO_REVERSE_8_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* orc.b */
  M_PSP_BITMANIP_ORCB(D_DEMO_ORCB_INPUT_NUM, uiResult);
  if (D_DEMO_ORCB_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* orc.16 */
  M_PSP_BITMANIP_ORC16(D_DEMO_ORC_16_INPUT_NUM, uiResult);
  if (D_DEMO_ORC_16_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* sbset */
  M_PSP_BITMANIP_SBSET(D_DEMO_SBSET_1ST_ARG, D_DEMO_SBSET_BIT_POSITION, uiResult);
  if (D_DEMO_SBSET_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* sbseti */
  M_PSP_BITMANIP_SBSETI(D_DEMO_SBSET_1ST_ARG, D_DEMO_SBSET_BIT_POSITION, uiResult);
  if (D_DEMO_SBSET_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* sbclr */
  M_PSP_BITMANIP_SBCLR(D_DEMO_SBCLR_1ST_ARG, D_DEMO_SBCLR_BIT_POSITION, uiResult);
  if (D_DEMO_SBCLR_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* sbclri */
  M_PSP_BITMANIP_SBCLRI(D_DEMO_SBCLR_1ST_ARG, D_DEMO_SBCLR_BIT_POSITION, uiResult);
  if (D_DEMO_SBCLR_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* sbinvert */
  M_PSP_BITMANIP_SBINV(D_DEMO_SBINVERT_1ST_ARG, D_DEMO_SBINVERT_BIT_POSITION, uiResult);
  if (D_DEMO_SBINVERT_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* sbinverti */
  M_PSP_BITMANIP_SBINVI(D_DEMO_SBINVERT_1ST_ARG, D_DEMO_SBINVERT_BIT_POSITION, uiResult);
  if (D_DEMO_SBINVERT_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* sbextract */
  M_PSP_BITMANIP_SBEXT(D_DEMO_SBEXTRACT_1ST_ARG, D_DEMO_SBEXTRACT_BIT_POSITION, uiResult);
  if (D_DEMO_SBEXTRACT_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }

  /* sbextracti */
  M_PSP_BITMANIP_SBEXTI(D_DEMO_SBEXTRACT_1ST_ARG, D_DEMO_SBEXTRACT_BIT_POSITION, uiResult);
  if (D_DEMO_SBEXTRACT_EXPECTED_RESULT != uiResult)
  {
    M_DEMO_ERR_PRINT();
    M_PSP_EBREAK();
  }
}

/**
 * @brief - demoStart - startup point of the demo application. called from main function.
 *
 */
void demoStart(void)
{
  M_DEMO_START_PRINT();

  demoBitManipulation();

  M_DEMO_END_PRINT();
}


