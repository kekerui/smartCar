/* @filename:Eit_Bmp.c
*
* @brief: ʵ��bmp�ļ��ı��棬���ڱ���������Ϣ��
*
* @the library:  VCAN��kinetis��
*
* @date:2015.1.9
*
* @modification time:2015.1.9
*
* @author: Hu wensong
*
*/

#include "Eit_SD.h"

FIL      bmpfdest;                                                              //�ļ�
FATFS     bmpfs;                                                                 //�ļ�ϵͳ 

uint16 img_sd[CAMERA_H][CAMERA_W];
 
uint8 Header[62] =
{
  0x42,0x4d,0,0,0,0,
  0,0,0,0,62,0,0,0,
  
  40,0,0,0,0,0,0,0,
  0,0,0,0,1,0,1,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  
  0xff,0xff,0xff,0x00,
  0x00,0x00,0x00,0x00
};     
/*!
 *  @brief      ������ͷ�ɼ���ͼ����bmp�ļ����浽sd����
 *  @date       2015.1.9
 *  @modification time:   2015.1.9
 */
void Img_Sdbmp(uint8 hight,uint8 width,uint8 *filename)
{
  long file_size;                                                               //�ļ��Ĵ�С
  long Hight;                                                                   //ͼ��ĸ�
  long Width;                                                                   //ͼ��Ŀ�
  uint8 Byte;
  uint8 bmp_name[30];                                                           //���ļ���
  int res=0,w_res;
  uint32 mybw;
  int x,y,i,j;
  int temp,tempy;
  char kk[4]={0,0,0,0};
  /* ��*�� +������ֽ� + ͷ����Ϣ */
	file_size = (long)width * (long)hight/8+62;                                              //�ļ���С����*��+�ļ�ͷ
  Header[2] = (uint8)file_size&0x000000ff;                                      //�����ļ���С���ļ�ͷ
  Header[3] = (file_size >> 8)&0x000000ff;
  Header[4] = (file_size >> 16)&0x000000ff;
  Header[5] = (file_size >> 24)&0x000000ff;
    
  Width = width;                                                                //�����ȵ��ļ�ͷ
  Header[18] = Width & 0x000000ff;
  Header[19] = (Width >> 8)&0x000000ff;
  Header[20] = (Width >> 16)&0x000000ff;
  Header[21] = (Width >> 24)&0x000000ff;
  
  Hight = hight;                                                                //����߶ȵ��ļ�ͷ
  Header[22] = Hight & 0x000000ff;
  Header[23] = (Hight >> 8)&0x000000ff;
  Header[24] = (Hight >> 16)&0x000000ff;
  Header[25] = (Hight >> 24)&0x000000ff;
  
  sprintf((char *)bmp_name,"0:/%s.bmp",filename);                               //���ļ�����һ����ʽ���浽bmp_name��
  f_mount(0,&bmpfs);                                                            //ע��һ��������
  
  res = f_open(&bmpfdest,(char *)bmp_name,FA_OPEN_ALWAYS | FA_WRITE);       //���ļ�
  printf("%d",res);
  if(res == FR_OK)
  {
    w_res=f_write(&bmpfdest,Header,sizeof(Header),&mybw);                      //���ļ�ͷд���ļ�
    
    if(w_res != FR_OK)
    {
      printf("дͷ�ļ�ʧ�ܣ�\r\n");
    }
    else
      printf("д�ļ�ͷ�ɹ���\r\n");
    for(i=0;i<Hight;i++)
    {
      if(!(Width%4))
      {
        for(j=0;j<Width;j++)
{
          Byte = imgbuff0[j+i*Width];
    
          w_res=f_write(&bmpfdest, &Byte,sizeof(unsigned char), &mybw);
  
        }
       }
      else
      {
        for(j=0;j<Width;j++)
        {
          Byte = imgbuff0[i+j*Width];
    
          w_res=f_write(&bmpfdest, &Byte,sizeof(unsigned char), &mybw);
        }
        w_res = f_write(&bmpfdest, kk,sizeof(unsigned char)*(Width%4), &mybw);
      }
    }
    f_sync(&bmpfdest);
    f_close(&bmpfdest);
  }  
  else
    printf("SD������");
  
}