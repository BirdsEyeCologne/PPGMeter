#include <stm32f4xx.h>
#include "SysControl.h"

int main(void) {

   #ifdef IWDG_ENABLE
      // Halt independent watch dog while debugging.
      DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_IWDG_STOP;
   #endif

   SysControl ctrl;

   ctrl.run();  // No return from here.

   return 0;
}


