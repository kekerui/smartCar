#include "ControlVar.h"
EIT_VAR gVar;
void ControlVar_Init(void)
{
	/*Left Motor*/	
	gVar.MotorL_CntInTs = 0;
        gVar.MotorL_Speed   = 0;
	/*Right Motor*/	
	gVar.MotorR_CntInTs = 0;
        gVar.MotorR_Speed   = 0;
     
        gVar.Car_Speed      = 0;
        gVar.time           = 0;
        gVar.InAngle        = 0;
}