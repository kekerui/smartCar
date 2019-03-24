#include "EIT_Buzzer.H"

void Buzzer_Init(void)
{
    gpio_init(BUZZER,GPO,0);
    
    adc_init(ADC0_SE11);              //ADC≥ı ºªØ
}

void EnBuzzer(void)
{
    gpio_set (BUZZER,ENABLE);
}

void DisBuzzer(void)
{
    gpio_set (BUZZER,DISABLE);
}