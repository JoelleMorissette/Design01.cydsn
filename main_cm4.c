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
volatile QueueHandle_t print_queue;
volatile int nombreEntree = 0; 

void isr_bouton(void){
    //Cy_SysPm_PmicUnlock();
    nombreEntree = nombreEntree +1;
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
            UART_1_PutString("\r""Bouton relache""\n");
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


void print_loop(void* params){
    for(;;){
        int delais =0;
        char *message = NULL;
        if(params == &task_A){
            message = task_A.message;
            delais = task_A.delay;
        }
        if(params == &task_B){
            message = task_B.message;
            delais = task_B.delay;
        }
        vTaskDelay(delais);
        xQueueSend(print_queue, &message, delais);
    }
}

void print_loopDeux(void *params){
    for(;;){    
        task_params_t* paramTemporaire = (task_params_t*)params;
        vTaskDelay(pdMS_TO_TICKS(paramTemporaire->delay));
        UART_1_PutString(paramTemporaire->message);
            
    }
}

void print(){
    char*message;
    for(;;){
        xQueueReceive(print_queue, &message, portMAX_DELAY);
        UART_1_PutString(message);
    }
}


int main(void)
{  
    UART_1_Start(); // Demarre le module UART
    
    bouton_semph = xSemaphoreCreateBinary(); // Initialisation du semaphore
    print_queue = xQueueCreate(2, sizeof(char*));
    // Initialisation de l'isr
    //Cy_SysPm_PmicUnlock(); // Fonction trouvee dans les recherches qui permet a l'isr de se produire 
    Cy_SysInt_Init(&bouton_isr_cfg,isr_bouton);
    NVIC_ClearPendingIRQ(bouton_isr_cfg.intrSrc);
    NVIC_EnableIRQ(bouton_isr_cfg.intrSrc);
    
    __enable_irq(); /* Enable global interrupts. */
     
    
   // Creation de taches
    
//    xTaskCreate(vInverseLED, "red",80,NULL,2,NULL);
   xTaskCreate(bouton_task, "Etat du bouton", 80,NULL, 1, NULL);
//   xTaskCreate(print_loopDeux,"task A", configMINIMAL_STACK_SIZE,(void *)&task_A, 1, NULL);
//   xTaskCreate(print_loopDeux,"task_B", configMINIMAL_STACK_SIZE,(void *)&task_B, 1, NULL);
 
    // Essai    
    xTaskCreate(print_loop,"task A", configMINIMAL_STACK_SIZE, (void*)&task_A, 1, NULL);
    xTaskCreate(print_loop,"task_B", configMINIMAL_STACK_SIZE,(void *)&task_B, 1, NULL);
    xTaskCreate(print , "Task_print", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    vTaskStartScheduler();
    

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
