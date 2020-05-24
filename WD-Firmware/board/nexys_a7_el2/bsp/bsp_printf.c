// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Western Digital Corporation or its affiliates.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//
// Based on a public domain version of printf
// Web link: http://www.rte.se/sites/default/files/Blog/Modesty/ee_printf.c 
//


#include <stdarg.h>
#include "psp_types.h"
#include "bsp_mem_map.h"
#include "bsp_printf.h"


/*---------------------------------------------------*/
/* Define                       */
/*---------------------------------------------------*/
#if 0

#define UART_RX_DATA  (*((volatile unsigned int*)(D_UART_BASE_ADDRESS + 0x0)))
#define UART_TX_DATA  (*((volatile unsigned int*)(D_UART_BASE_ADDRESS + 0x4)))
#define UART_STAT    (*((volatile unsigned int*)(D_UART_BASE_ADDRESS + 0x8)))
#define UART_CTRL     (*((volatile unsigned int*)(D_UART_BASE_ADDRESS + 0xC)))

#define UART_TX_BUSY  (1<<3)
#define UART_RX_AVAIL  (1<<0)

#endif



#define M_UART_WR_REG_BRDL(_VAL_) (*((volatile unsigned int*)(D_UART_BASE_ADDRESS + (4*0x00) )) = _VAL_) /* Baud rate divisor (LSB)        */
#define M_UART_WR_REG_IER(_VAL_)  (*((volatile unsigned int*)(D_UART_BASE_ADDRESS + (4*0x01) )) = _VAL_) /* Interrupt enable reg.          */
#define M_UART_WR_REG_FCR(_VAL_)  (*((volatile unsigned int*)(D_UART_BASE_ADDRESS + (4*0x02) )) = _VAL_) /* FIFO control reg.              */
#define M_UART_WR_REG_LCR(_VAL_)  (*((volatile unsigned int*)(D_UART_BASE_ADDRESS + (4*0x03) )) = _VAL_) /* Line control reg.              */
#define M_UART_WR_REG_LSR(_VAL_)  (*((volatile unsigned int*)(D_UART_BASE_ADDRESS + (4*0x05) )) = _VAL_) /* Line control reg.              */

#define M_UART_RD_REG_LSR()  (*((volatile unsigned int*)(D_UART_BASE_ADDRESS + (4*0x05) )))              /* Line status reg.               */


#define D_BAUD_RATE            (115200)
#define D_UART_LCR_CS8         (0x03)  /* 8 bits data size */
#define D_UART_LCR_1_STB       (0x00)  /* 1 stop bit */
#define D_UART_LCR_PDIS        (0x00)  /* parity disable */

#define D_UART_LSR_THRE_BIT    (0x20)
#define D_UART_FCR_FIFO_BIT    (0x01)  /* enable XMIT and RCVR FIFO */
#define D_UART_FCR_RCVRCLR_BIT (0x02)  /* clear RCVR FIFO */
#define D_UART_FCR_XMITCLR_BIT (0x04)  /* clear XMIT FIFO */
#define D_UART_FCR_MODE0_BIT   (0x00)  /* set receiver in mode 0 */
#define D_UART_FCR_MODE1_BIT   (0x08)  /* set receiver in mode 1 */
#define D_UART_FCR_FIFO_8_BIT  (0x80)  /* 8 bytes in RCVR FIFO */
#define D_UART_DLAB_BIT        (0x80)  /* DLAB bit in LCR */

#define M_UART_WR_CH(_CHAR_) (*((volatile unsigned int*)(D_UART_BASE_ADDRESS + (0x00) )) = _CHAR_)

/*---------------------------------------------------*/
/* static                                            */
/*---------------------------------------------------*/

/*---------------------------------------------------*/
/* bsp_strlen                       */
/*---------------------------------------------------*/
int bsp_strlen(const char *s)
{
    const char *it = s;

  while(*it)
  {
    ++it;
  }
    return it - s;
}


/*---------------------------------------------------*/
/* printUartPutchar: Write character to UART       */
/*---------------------------------------------------*/
#if 0 /* this works for WD SweRV EH1*/
int printUartPutchar(char ch)
{   
  if (ch == '\n')
    printUartPutchar('\r');

  //check status
  while (UART_STAT & UART_TX_BUSY);

  //write char
  UART_RX_DATA = ch;

  return 0;
}
#endif

/**
* The function put chars in UART 16550
*
* @param ch     - char
*
* @return u32_t - 0
*/
int printUartPutchar(char ch)
{
  if (ch == '\n')
    printUartPutchar('\r');

  /* Check for space in UART FIFO */
  while((M_UART_RD_REG_LSR() & D_UART_LSR_THRE_BIT) == 0);

  //write char
  M_UART_WR_CH(ch);

  return 0;
}

/**
* The function put chars in UART 16550
*
* @param ch     - char
*
* @return u32_t - 0
*/
void uartInit(void)
{
  /* SET LSR to be 1's so Whisper will be happy that ch is ready */
  M_UART_WR_REG_LSR(0xff);

  /* Set DLAB bit in LCR */
  M_UART_WR_REG_LCR(D_UART_DLAB_BIT);

  /* Set divisor regs  devisor = 27: clock_freq/baud_rate*16 -->> clock = 50MHz, baud=115200*/
  M_UART_WR_REG_BRDL((D_CLOCK_RATE/D_BAUD_RATE)/16);

  /* 8 data bits, 1 stop bit, no parity, clear DLAB */
  M_UART_WR_REG_LCR((D_UART_LCR_CS8  | D_UART_LCR_1_STB | D_UART_LCR_PDIS));
  M_UART_WR_REG_FCR((D_UART_FCR_FIFO_BIT| D_UART_FCR_MODE0_BIT | D_UART_FCR_FIFO_8_BIT | D_UART_FCR_RCVRCLR_BIT |D_UART_FCR_XMITCLR_BIT));

  /* disable interrupts  */
  M_UART_WR_REG_IER(0x00);

}
/*----------------------------------------------------*/
/* Use the following parameter passing structure to   */
/* make printf re-entrant.                         */
/*----------------------------------------------------*/
typedef struct params_s 
{
    int len;
    long num1;
    long num2;
    char pad_character;
    int do_padding;
    int left_flag;
  int upper_hex_digit_flag; //added hexdigit uppercase [A-F]
  int maxium_length; // max_length
} params_t;


/*----------------------------------------------------*/
/* puts: print string                  */
/*----------------------------------------------------*/

int puts( const char * str )
{
  while (*str)
    printUartPutchar(*str++);

  return printUartPutchar('\n');
}

/*----------------------------------------------------*/
/* putchar: print character                */
/*----------------------------------------------------*/
int putchar( int c )
{
  printUartPutchar((char)c);
  return c;
}


/*---------------------------------------------------*/
/*                                                   */
/* This routine puts pad characters into the output  */
/* buffer.                                           */
/*                                                   */
/*---------------------------------------------------*/
static void padding( const int l_flag, params_t *par)
{
    int i;

    if (par->do_padding && l_flag && (par->len < par->num1))
        for (i=par->len; i<par->num1; i++)
            printUartPutchar( par->pad_character);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine moves a string to the output buffer  */
/* as directed by the padding and positioning flags. */
/*                                                   */
/*---------------------------------------------------*/
static void outs(  char* lp, params_t *par)
{
    /* pad on left if needed                         */
    par->len = bsp_strlen( lp);
    padding( !(par->left_flag), par);

    /* Move string to the buffer                     */
    while (*lp && (par->num2)--)
        printUartPutchar( *lp++);

    /* Pad on right if needed                        */
    par->len = bsp_strlen( lp);
    padding( par->left_flag, par);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine moves a number to the output buffer  */
/* as directed by the padding and positioning flags. */
/*                                                   */
/*---------------------------------------------------*/

static void outnum( const int n, const long base, params_t *par)
{
    char* cp;
    int negative;
    char outbuf[32];
  const char uphexdigits[] = "0123456789ABCDEF";
  const char lohexdigits[] = "0123456789abcdef";
  const char *digits;
    unsigned long num;
  int i;

    /* Check if number is negative                   */
    if (base == 10 && n < 0L) {
        negative = 1;
        num = -(n);
    }
    else{
        num = (n);
        negative = 0;
    }

  if (par->upper_hex_digit_flag)
    digits = uphexdigits;
  else
    digits = lohexdigits;
   
    /* Build number (backwards) in outbuf            */
  cp = outbuf;
  // only for 10 and 16
  // avoid complex divide
  if (base == 10)
  {
    do {
      *cp++ = digits[(int)(num % 10)];
    } while ((num /= 10) > 0);
  }
  else
  {
    do {
      *cp++ = digits[(int)(num % 16)];
    } while ((num /= 16) > 0);
  }

    if (negative)
        *cp++ = '-';
    *cp-- = 0;

    /* Move the converted number to the buffer and   */
    /* add in the padding where needed.              */
    par->len = bsp_strlen(outbuf);
    padding( !(par->left_flag), par);
  i = 0;
    while (cp >= outbuf && i++ < par->maxium_length)
        printUartPutchar( *cp--);
    padding( par->left_flag, par);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine gets a number from the format        */
/* string.                                           */
/*                                                   */
/*---------------------------------------------------*/

static long getnum( char** linep)
{
    long n;
    char* cp;

    n = 0;
    cp = *linep;
    while (((*cp) >= '0' && (*cp) <= '9'))
        n = n*10 + ((*cp++) - '0');
    *linep = cp;
    return(n);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine operates just like a printf/sprintf  */
/* routine. It outputs a set of data under the       */
/* control of a formatting string. Not all of the    */
/* standard C format control are supported. The ones */
/* provided are primarily those needed for embedded  */
/* systems work. Primarily the floaing point         */
/* routines are omitted. Other formats could be      */
/* added easily by following the examples shown for  */
/* the supported formats.                            */
/*                                                   */
/*---------------------------------------------------*/
int uart_printf(const char* ctrl1, va_list argp)
{
    int long_flag;
    int dot_flag;
  int res = 0;

    params_t par;

    char ch;
    char* ctrl = (char*)ctrl1;
    for ( ; *ctrl; ctrl++) 
  {
        /* move format string chars to buffer until a  */
        /* format control is found.                    */
        if ( *ctrl != '%') 
    {
      printUartPutchar( *ctrl);
            continue;
        }

        /* initialize all the flags for this format.   */
        dot_flag  = long_flag = par.left_flag = par.do_padding = 0;
        par.pad_character = ' ';
        par.num2=32767;
    par.maxium_length = 10;

 try_next:
        ch = *(++ctrl);
        if ((ch >= '0' && ch <= '9')) 
    {
            if (dot_flag)
                par.num2 = getnum(&ctrl);
            else {
                if (ch == '0')
                    par.pad_character = '0';

                par.num1 = getnum(&ctrl);
                par.do_padding = 1;
            }
            ctrl--;
            goto try_next;
        }

    par.upper_hex_digit_flag = (ch >= 'A' && ch <= 'Z') ? 1 : 0;

        switch ((par.upper_hex_digit_flag ? ch + 32: ch)) 
    {
            case '%':
        printUartPutchar( '%');
                continue;

            case '-':
                par.left_flag = 1;
                break;

            case '.':
                dot_flag = 1;
                break;

            case 'l':
                long_flag = 1;
                break;

            case 'd':
      case 'u':
                if (long_flag || ch == 'D') 
        {
                    outnum( va_arg(argp, long), 10L, &par);
                    continue;
                }
                else {
                    outnum( va_arg(argp, int), 10L, &par);
                    continue;
                }
            
      case 'x':
      case 'p':
        if (long_flag || ch == 'D')
        {
          par.maxium_length = sizeof(long) * 2;
          outnum( (long)va_arg(argp, long), 16L, &par);
        }
        else
        {
          par.maxium_length = sizeof(int) * 2;
          outnum( (long)va_arg(argp, int), 16L, &par);
        }
                continue;
            case 's':
                outs( va_arg( argp, char*), &par);
                continue;
            case 'c':
      printUartPutchar( va_arg( argp, int));
                continue;
            case '\\':
                switch (*ctrl) {
                    case 'a':
                        printUartPutchar( 0x07);
                        break;
                    case 'h':
                        printUartPutchar( 0x08);
                        break;
                    case 'r':
                        printUartPutchar( 0x0D);
                        break;
                    case 'n':
                        printUartPutchar( 0x0A);
                        break;
                    default:
                        printUartPutchar( *ctrl);
                        break;
                }
                ctrl++;
                break;

            default:
                continue;
        }
        goto try_next;
    }
  return res;
}


/*---------------------------------------------------*/
/* printf: Console based printf            */
/*---------------------------------------------------*/
u32_t printfNexys( const char * cFormat, ... )
{
  u32_t uiRes = 0;
  va_list argp;

  if (cFormat)
  {
     va_start( argp, cFormat);
     // Setup target to be stdout function
     uiRes = uart_printf( cFormat, argp);
     va_end( argp);
  }

  printUartPutchar('\n');

  return uiRes;
}


