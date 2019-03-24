#include "include.h"
void LCD_showADC(void)                      //ADת����Դ��ѹ
{
    uint16 var;
    uint32 U;      //��Դ��ѹ
    
    var = adc_once(ADC0_SE11, ADC_16bit);
    
    // printf("\nADC�������Ϊ:%d",var);
    // printf("     ��Ӧ��ѹֵΪ%dmV",(3300*var)/((1<<8)-1));
    
    U=(3300*var) * 11/((1<<16)-1) * 1.0821;

    LCD_str(Site_V,"U:",WHITE,BLACK);
    LCD_num(Site_VNum,U,WHITE,BLACK);  
}