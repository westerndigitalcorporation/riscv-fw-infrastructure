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
/**
* @file   main.c
* @author Nati Rapaport
* @date   23.10.2019
* @brief  main C file of the demonstration application
*/

/**
* include files
*/
#include <unistd.h>
#include "platform_al.h"
#include "demo_app.h"

#ifdef D_USE_RTOSAL
  #include "rtosal_util_api.h"
#else
   #error "\n\n --- You have to use RTOS abstraction layer --- \n\n"
#endif

/**
* definitions
*/

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




//////////////////////////////////////////////////////////////////////
/**
* Initialize the platform and then calls the apropriate demo function
*
* @param  - void
*
* @return - this function never returns        
*/
//////////////////////////////////////////////////////////////////////
int main(void)
{
	/* Initilize the platform first */
	platformInit();

   /* Now is the time to activate the relevant demonstration function */
   rtosalStart(demoInit);

   /* If all is well, the scheduler will now be running, and the following line
   will never be reached.  If the following line does execute, then there was
   insufficient FreeRTOS heap memory available for the idle and/or timer tasks
   to be created.  See the memory management section on the FreeRTOS web site
   for more details.  */
   write(1,"\n-- We should not reach here. Check what went wrong. --\n", 55);
   for( ;; );
}
