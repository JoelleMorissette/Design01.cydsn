/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "params.h"
#include "queue.h"

void vInverseLED(){
   Cy_GPIO_Write(Pin_1_0_PORT,Pin_1_0_NUM,1);
    for(;;){
        vTaskDelay(pdMS_TO_TICKS(500));
//     Cy_GPIO_Write(Pin_1_0_PORT,Pin_1_0_NUM, ~(Cy_GPIO_Read(Pin_1_0_PORT, Pin_1_0_NUM)));
         Cy_GPIO_Write(Pin_1_0_PORT,Pin_1_0_NUM,0);
         vTaskDelay(pdMS_TO_TICKS(500));
        Cy_GPIO_Write(Pin_1_0_PORT,Pin_1_0_NUM,1);
    }
}

int main(void)
{
   
    __enable_irq(); /* Enable global interrupts. */
  
    xTaskCreate(vInverseLED, "red",80,NULL,3,NULL);
        
    
    vTaskStartScheduler();

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
