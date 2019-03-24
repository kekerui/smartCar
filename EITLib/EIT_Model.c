#include "EIT_Model.H"

void Model_Init(void)
{  
    gpio_init(Model1,GPI,0);
    gpio_init(Model2,GPI,0);
    gpio_init(Model3,GPI,0);
    port_init_NoALT(Model1,PULLUP);
    port_init_NoALT(Model2,PULLUP);
    port_init_NoALT(Model3,PULLUP);
}