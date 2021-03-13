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
#include <stdio.h>


//void vInverseLED(){
//   Cy_GPIO_Write(Pin_1_0_PORT,Pin_1_0_NUM,1);
//    for(;;){
//        vTaskDelay(pdMS_TO_TICKS(500));
////     Cy_GPIO_Write(Pin_1_0_PORT,Pin_1_0_NUM, ~(Cy_GPIO_Read(Pin_1_0_PORT, Pin_1_0_NUM)));
//         Cy_GPIO_Write(Pin_1_0_PORT,Pin_1_0_NUM,0);
//         vTaskDelay(pdMS_TO_TICKS(500));
//        Cy_GPIO_Write(Pin_1_0_PORT,Pin_1_0_NUM,1);
//    }
//}
volatile SemaphoreHandle_t bouton_semph;

void isr_bouton(void){
     Cy_SysPm_PmicUnlock();
     xSemaphoreGiveFromISR(bouton_semph,NULL);
    Cy_GPIO_ClearInterrupt(BOUTON_0_PORT, BOUTON_0_NUM);
    NVIC_ClearPendingIRQ(bouton_isr_cfg.intrSrc);
   
}

void bouton_task(){
   UART_1_PutString("Bouton relache");
    
    for(;;){
        if(xSemaphoreTake(bouton_semph,1) == true){
            UART_1_PutString("Bouton appuye");
            vTaskDelay(pdMS_TO_TICKS(20));
            UART_1_PutString("Bouton relache");
        }           
    }
}

int main(void)
{  
    UART_1_Init(&UART_1_config);
    UART_1_Enable();
    UART_1_Start(); // Demarre le module UART
    
    UART_1_PutString("Test");
    bouton_semph = xSemaphoreCreateBinary(); // Initialisation du semaphore
   
    // Initialisation de l'isr
    Cy_SysPm_PmicUnlock();
    Cy_SysInt_Init(&bouton_isr_cfg,isr_bouton);
    NVIC_ClearPendingIRQ(bouton_isr_cfg.intrSrc);
    NVIC_EnableIRQ(bouton_isr_cfg.intrSrc);
    
    __enable_irq(); /* Enable global interrupts. */
     
    
   // Creation de taches
    
//    xTaskCreate(vInverseLED, "red",80,NULL,2,NULL);
   xTaskCreate(bouton_task, "Etat du bouton", 80,NULL, 3, NULL);
//    
    vTaskStartScheduler();

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
