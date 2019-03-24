#include "include.h"
void LCD_showADC(void)                      //AD转换电源电压
{
    uint16 var;
    uint32 U;      //电源电压
    
    var = adc_once(ADC0_SE11, ADC_16bit);
    
    // printf("\nADC采样结果为:%d",var);
    // printf("     相应电压值为%dmV",(3300*var)/((1<<8)-1));
    
    U=(3300*var) * 11/((1<<16)-1) * 1.0821;

    LCD_str(Site_V,"U:",WHITE,BLACK);
    LCD_num(Site_VNum,U,WHITE,BLACK);  
}