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
    for(;;){
        xSemaphoreTake(bouton_semph,0) ;
        if( xSemaphoreTake(bouton_semph,0)== 1){
            UART_1_PutString("\r" "Bouton appuye" "\n");
            vTaskDelay(pdMS_TO_TICKS(20));
            UART_1_PutString("\r" "Bouton relache" "\n");
        }           
    }
}



task_params_t task_A = {
    .delay = 1000,
    .message = "Tache A en cours" "\n\r"
};

task_params_t task_B = {
    .delay = 999,
    .message = "Tache B en cours" "\n\r"
};


void print_loop(void * params){
    for(;;){
       
        UART_1_PutString(params);     
    }
}
volatile void* pointeurA = &task_A;  // Definition pointeur vers adresse de task_A
volatile void* pointeurB = &task_B;  // Definition pointeur vers adresse de task B 

void print_loopDeux(void *params){
    for(;;){       
        if (params == pointeurA){      // Si l'adresse pointee par params est celle de task_A
            UART_1_PutString(task_A.message);  // On affiche le message de task_A
            vTaskDelay(task_A.delay);  // La periode est donc du delais de la task_A 
            
        }
        if (params == pointeurB){   // Meme chose, mais avec task_B 
           UART_1_PutString(task_B.message);
           vTaskDelay(task_B.delay); 
        }
       
    }
}

int main(void)
{  
    UART_1_Start(); // Demarre le module UART
    
    bouton_semph = xSemaphoreCreateBinary(); // Initialisation du semaphore
   
    // Initialisation de l'isr
    //Cy_SysPm_PmicUnlock(); // Fonction trouvee dans les recherches qui permet a l'isr de se produire 
    Cy_SysInt_Init(&bouton_isr_cfg,isr_bouton);
    NVIC_ClearPendingIRQ(bouton_isr_cfg.intrSrc);
    NVIC_EnableIRQ(bouton_isr_cfg.intrSrc);
    
    __enable_irq(); /* Enable global interrupts. */
     
    
   // Creation de taches
    
//    xTaskCreate(vInverseLED, "red",80,NULL,2,NULL);
    xTaskCreate(bouton_task, "Etat du bouton", 80,NULL, 1, NULL);
    xTaskCreate(print_loopDeux,"task A", configMINIMAL_STACK_SIZE,(void *)&task_A, 1, NULL);
    xTaskCreate(print_loopDeux,"task_B", configMINIMAL_STACK_SIZE,(void *)&task_B, 1, NULL);
    vTaskStartScheduler();
    

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
