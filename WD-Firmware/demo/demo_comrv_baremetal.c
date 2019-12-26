/*
* SPDX-License-Identifier: Apache-2.0
* Copyright 2019 Western Digital Corporation or its affiliates.
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
#include "common_types.h"
#include "psp_macros.h"
#include "comrv_api.h"
#include "demo_platform_al.h"










/* BEEBS edn benchmark

   Copyright (C) 2014 Embecosm Limited and University of Bristol

   Contributor James Pallister <james.pallister@bristol.ac.uk>

   This file is part of the Bristol/Embecosm Embedded Benchmark Suite.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>. */

/* Original code from: WCET Benchmarks,
       http://www.mrtc.mdh.se/projects/wcet/benchmarks.html
   Permission to license under GPL obtained by email from Björn Lisper
 */

//#include "support.h"
#include "common_types.h"
#include "comrv_api.h"
/* This scale factor will be changed to equalise the runtime of the
   benchmarks. */
#define SCALE_FACTOR    (REPEAT_FACTOR >> 0)

#define OVL_benchmark  _OVERLAY_
#define OVL_jpegdct  _OVERLAY_

/*
 * MDH WCET BENCHMARK SUITE. File version $Id: edn.c,v 1.1 2005/11/11
 * 10:14:10 ael01 Exp $
 */

/************************************************************************
*  Simple vector multiply           *
************************************************************************/

/*
 * Changes: JG 2005/12/22: Inserted prototypes, changed type of main to int
 * etc. Added parenthesis in expressions in jpegdct. Removed unused variable
 * dx. Changed int to long to avoid problems when compiling to 16 bit target
 * Indented program.
 * JG 2006-01-27: Removed code in codebook
 */

#define N 100
#define ORDER 50

void            vec_mpy1(short y[], const short x[], short scaler);
long int        mac(const short *a, const short *b, long int sqr, long int *sum);
void            fir(const short array1[], const short coeff[], long int output[]);
void            fir_no_red_ld(const short x[], const short h[], long int y[]);
long int        latsynth(short b[], const short k[], long int n, long int f);
void            iir1(const short *coefs, const short *input, long int *optr, long int *state);
long int        codebook(long int mask, long int bitchanged, long int numbasis, long int codeword, long int g, const short *d, short ddim, short theta);
void            _OVERLAY_ jpegdct(short *d, short *r);

void
vec_mpy1(short y[], const short x[], short scaler)
{
   long int        i;

   for (i = 0; i < 150; i++)
      y[i] += ((scaler * x[i]) >> 15);
}


/*****************************************************
*        Dot Product       *
*****************************************************/
long int
mac(const short *a, const short *b, long int sqr, long int *sum)
{
   long int        i;
   long int        dotp = *sum;

   for (i = 0; i < 150; i++) {
      dotp += b[i] * a[i];
      sqr += b[i] * b[i];
   }

   *sum = dotp;
   return sqr;
}


/*****************************************************
*     FIR Filter          *
*****************************************************/
void
fir(const short array1[], const short coeff[], long int output[])
{
   long int        i, j, sum;

   for (i = 0; i < N - ORDER; i++) {
      sum = 0;
      for (j = 0; j < ORDER; j++) {
         sum += array1[i + j] * coeff[j];
      }
      output[i] = sum >> 15;
   }
}

/****************************************************
*  FIR Filter with Redundant Load Elimination

By doing two outer loops simultaneously, you can potentially  reuse data (depending on the DSP architecture).
x and h  only  need to be loaded once, therefore reducing redundant loads.
This reduces memory bandwidth and power.
*****************************************************/
void
fir_no_red_ld(const short x[], const short h[], long int y[])
{
   long int        i, j;
   long int        sum0, sum1;
   short           x0, x1, h0, h1;
   for (j = 0; j < 100; j += 2) {
      sum0 = 0;
      sum1 = 0;
      x0 = x[j];
      for (i = 0; i < 32; i += 2) {
         x1 = x[j + i + 1];
         h0 = h[i];
         sum0 += x0 * h0;
         sum1 += x1 * h0;
         x0 = x[j + i + 2];
         h1 = h[i + 1];
         sum0 += x1 * h1;
         sum1 += x0 * h1;
      }
      y[j] = sum0 >> 15;
      y[j + 1] = sum1 >> 15;
   }
}

/*******************************************************
*  Lattice Synthesis            *
* This function doesn't follow the typical DSP multiply two  vector operation, but it will point out the compiler's flexibility   ********************************************************/
long int
latsynth(short b[], const short k[], long int n, long int f)
{
   long int        i;

   f -= b[n - 1] * k[n - 1];
   for (i = n - 2; i >= 0; i--) {
      f -= b[i] * k[i];
      b[i + 1] = b[i] + ((k[i] * (f >> 16)) >> 16);
   }
   b[0] = f >> 16;
   return f;
}

/*****************************************************
*        IIR Filter          *
*****************************************************/
void
iir1(const short *coefs, const short *input, long int *optr, long int *state)
{
   long int        x;
   long int        t;
   long int        n;

   x = input[0];
   for (n = 0; n < 50; n++) {
      t = x + ((coefs[2] * state[0] + coefs[3] * state[1]) >> 15);
      x = t + ((coefs[0] * state[0] + coefs[1] * state[1]) >> 15);
      state[1] = state[0];
      state[0] = t;
      coefs += 4; /* point to next filter coefs  */
      state += 2; /* point to next filter states */
   }
   *optr++ = x;
}

/*****************************************************
*  Vocoder Codebook Search         *
*****************************************************/
long int
codebook(long int mask, long int bitchanged, long int numbasis, long int codeword, long int g, const short *d, short ddim, short theta)
/*
 * dfm (mask=d  bitchanged=1 numbasis=17  codeword=e[0] , g=d, d=a, ddim=c,
 * theta =1
 */

{
   long int        j;


   /*
    * Remove along with the code below.
    *
   long int        tmpMask;

   tmpMask = mask << 1;
   */
   for (j = bitchanged + 1; j <= numbasis; j++) {



/*
 * The following code is removed since it gave a memory access exception.
 * It is OK since the return value does not control the flow.
 * The loop always iterates a fixed number of times independent of the loop body.

    if (theta == !(!(codeword & tmpMask)))
         g += *(d + bitchanged * ddim + j);
      else
         g -= *(d + bitchanged * ddim + j);
      tmpMask <<= 1;
*/
   }
   return g;
}


/*****************************************************
*     JPEG Discrete Cosine Transform           *
*****************************************************/
/*__attribute__((noinline))*/ void OVL_jpegdct
jpegdct(short *d, short *r)
{
   long int        t[12];
   short           i, j, k, m, n, p;
   for (k = 1, m = 0, n = 13, p = 8; k <= 8; k += 7, m += 3, n += 3, p -= 7, d -= 64) {
      for (i = 0; i < 8; i++, d += p) {
         for (j = 0; j < 4; j++) {
            t[j] = d[k * j] + d[k * (7 - j)];
            t[7 - j] = d[k * j] - d[k * (7 - j)];
         }
         t[8] = t[0] + t[3];
         t[9] = t[0] - t[3];
         t[10] = t[1] + t[2];
         t[11] = t[1] - t[2];
         d[0] = (t[8] + t[10]) >> m;
         d[4 * k] = (t[8] - t[10]) >> m;
         t[8] = (short) (t[11] + t[9]) * r[10];
         d[2 * k] = t[8] + (short) ((t[9] * r[9]) >> n);
         d[6 * k] = t[8] + (short) ((t[11] * r[11]) >> n);
         t[0] = (short) (t[4] + t[7]) * r[2];
         t[1] = (short) (t[5] + t[6]) * r[0];
         t[2] = t[4] + t[6];
         t[3] = t[5] + t[7];
         t[8] = (short) (t[2] + t[3]) * r[8];
         t[2] = (short) t[2] * r[1] + t[8];
         t[3] = (short) t[3] * r[3] + t[8];
         d[7 * k] = (short) (t[4] * r[4] + t[0] + t[2]) >> n;
         d[5 * k] = (short) (t[5] * r[6] + t[1] + t[3]) >> n;
         d[3 * k] = (short) (t[6] * r[5] + t[1] + t[2]) >> n;
         d[1 * k] = (short) (t[7] * r[7] + t[0] + t[3]) >> n;
      }
   }
}

static short a[200];
static short b[200];
static short c;
static long int d;
static int e;
static long int output[200];


void
initialise_benchmark (void)
{
}


/*__attribute__((noinline))*/ int OVL_benchmark
benchmark(void)
{
   short in_a[200] = {0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000,
      0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00,
      0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200,
      0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300,
      0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000,
      0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00,
      0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200,
      0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300,
      0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000,
      0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00,
      0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200,
      0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300,
      0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000,
      0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00,
      0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200,
      0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300,
      0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000,
      0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00,
      0x0800, 0x0200, 0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200,
      0xf800, 0xf300, 0x0400, 0x0000, 0x07ff, 0x0c00, 0x0800, 0x0200, 0xf800, 0xf300, 0x0400
   };
   short in_b[200] =
   {0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60,
      0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20,
      0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600,
      0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200,
      0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60,
      0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20,
      0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600,
      0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200,
      0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60,
      0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20,
      0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600,
      0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200,
      0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60,
      0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20,
      0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600,
      0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200,
      0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60,
      0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20,
      0x0c00, 0xf600, 0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600,
      0xf400, 0xf200, 0xf000, 0x0c60, 0x0c40, 0x0c20, 0x0c00, 0xf600, 0xf400, 0xf200, 0xf000
   };
   c = 0x3;
   d = 0xAAAA;
        e = 0xEEEE;

        for (int i=0; i<200; i++)
        {
          a[i] = in_a[i];
          b[i] = in_b[i];
        }
   /*
    * Declared as memory variable so it doesn't get optimized out
    */

   vec_mpy1(a, b, c);
   c = mac(a, b, (long int) c, (long int *) output);
   fir(a, b, output);
   fir_no_red_ld(a, b, output);
   d = latsynth(a, b, N, d);
   iir1(a, b, &output[100], output);
   e = codebook(d, 1, 17, e, d, a, c, 1);
   jpegdct(a, b);
   return 0;
}

#include <stdio.h>
int verify_benchmark(int unused)
{
   int i;
   int exp_c = 10243;
   int exp_d = -441886230;
   int exp_e = -441886230;
   long int exp_output[200] = {3760, 4269, 3126, 1030, 2453, -4601, 1981, -1056, 2621, 4269, 3058, 1030, 2378, -4601, 1902, -1056, 2548, 4269, 2988, 1030, 2300, -4601, 1822, -1056, 2474, 4269, 2917, 1030, 2220, -4601, 1738, -1056, 2398, 4269, 2844, 1030, 2140, -4601, 1655, -1056, 2321, 4269, 2770, 1030, 2058, -4601, 1569, -1056, 2242, 4269, 2152, 1030, 1683, -4601, 1627, -1056, 2030, 4269, 2080, 1030, 1611, -4601, 1555, -1056, 1958, 4269, 2008, 1030, 1539, -4601, 1483, -1056, 1886, 4269, 1935, 1030, 1466, -4601, 1410, -1056, 1813, 4269, 1862, 1030, 1393, -4601, 1337, -1056, 1740, 4269, 1789, 1030, 1320, -4601, 1264, -1056,  1667, 4269, 1716, 1030, 1968, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
   for (i=0; i<200; i++) {
      if (output[i] != exp_output[i])
         return 0;
        }
   if (c != exp_c || d != exp_d || e != exp_e)
      return 0;
        return 1;
}








extern void* __OVERLAY_STORAGE_START__ADDRESS__;

#ifdef D_COMRV_FW_INSTRUMENTATION
comrvInstrumentationArgs_t g_stInstArgs;
#endif /* D_COMRV_FW_INSTRUMENTATION */

#define OVL_OverlayFunc0 _OVERLAY_
#define OVL_OverlayFunc1 _OVERLAY_
#define OVL_OverlayFunc2 _OVERLAY_
//#define OVL_benchmark    _OVERLAY_
//#define OVL_jpegdct      _OVERLAY_

//extern int OVL_benchmark benchmark(void);

volatile u32_t globalCount = 0;
volatile u32_t gOverlayFunc0 = 0;
volatile u32_t gOverlayFunc1 = 0;
volatile u32_t gOverlayFunc2 = 0;

/* overlay function 2 */
void OVL_OverlayFunc2 OverlayFunc2(void)
{
   gOverlayFunc2+=3;
}

/* non overlay function */
D_PSP_NO_INLINE void NonOverlayFunc(void)
{
   globalCount+=1;
   OverlayFunc2();
   globalCount+=2;
}

/* overlay function 1 */
void OVL_OverlayFunc1 OverlayFunc1(void)
{
   gOverlayFunc1+=3;
   NonOverlayFunc();
   gOverlayFunc1+=4;
}

typedef void (*funcPtr)(void);
funcPtr myFunc;

/* overlay function 0 */
void OVL_OverlayFunc0 OverlayFunc0(void)
{
   gOverlayFunc0+=1;
   myFunc();
   gOverlayFunc0+=2;
}

void demoStart(void)
{
   comrvInitArgs_t stComrvInitArgs = { 1 };

   comrvInit(&stComrvInitArgs);

   /* demonstrate function pointer usage */
   myFunc = OverlayFunc1;

   globalCount+=1;
   OverlayFunc0();
   benchmark();
   globalCount+=2;
   /* verify function calls where completed successfully */
   if (globalCount != 6 || gOverlayFunc0 != 3 ||
       gOverlayFunc1 != 7 || gOverlayFunc2 != 3)
   {
      /* loop forever */
      M_ENDLESS_LOOP();
   }
}

/**
* memory copy hook
*
* @param  none
*
* @return none
*/
void comrvMemcpyHook(void* pDest, void* pSrc, u32_t sizeInBytes)
{
   u32_t loopCount = sizeInBytes/(sizeof(u32_t)), i;
   /* copy dwords */
   for (i = 0; i < loopCount ; i++)
   {
      *((u32_t*)pDest + i) = *((u32_t*)pSrc + i);
   }
   loopCount = sizeInBytes - (loopCount*(sizeof(u32_t)));
   /* copy bytes */
   for (i = (i-1)*(sizeof(u32_t)) ; i < loopCount ; i++)
   {
      *((u08_t*)pDest + i) = *((u08_t*)pSrc + i);
   }
}

/**
* load overlay group hook
*
* @param pLoadArgs - refer to comrvLoadArgs_t for exact args
*
* @return loaded address or NULL if unable to load
*/
void* comrvLoadOvlayGroupHook(comrvLoadArgs_t* pLoadArgs)
{
   comrvMemcpyHook(pLoadArgs->pDest, (u08_t*)&__OVERLAY_STORAGE_START__ADDRESS__ + pLoadArgs->uiGroupOffset, pLoadArgs->uiSizeInBytes);
   return pLoadArgs->pDest;
}

/**
* notification hook
*
* @param  pInstArgs - pointer to instrumentation arguments
*
* @return none
*/
#ifdef D_COMRV_FW_INSTRUMENTATION
void comrvInstrumentationHook(const comrvInstrumentationArgs_t* pInstArgs)
{
   g_stInstArgs = *pInstArgs;
}
#endif /* D_COMRV_FW_INSTRUMENTATION */

/**
* error hook
*
* @param  pErrorArgs - pointer to error arguments
*
* @return none
*/
void comrvErrorHook(const comrvErrorArgs_t* pErrorArgs)
{
   comrvStatus_t stComrvStatus;
   comrvGetStatus(&stComrvStatus);
   /* we can't continue so loop forever */
   while (1);
}

/**
* crc calculation hook (itt)
*
* @param pAddress         - memory address to calculate
*        memSizeInBytes   - number of bytes to calculate
*        uiExpectedResult - expected crc result
*
* @return calculated CRC
*/
u32_t comrvCrcCalcHook (const void* pAddress, u16_t usMemSizeInBytes, u32_t uiExpectedResult)
{
   return 0;
}

/******************** start temporary build issue workaround ****************/
void _kill(void)
{
}
void _sbrk(void)
{
}
void _getpid(void)
{
}
/******************** end temporary build issue workaround ****************/

