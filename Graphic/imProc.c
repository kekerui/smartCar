#include "imProc.h"
#include "imCom.h"
#include "ControlParam.h"
#ifndef MK60F15
   #include "mex.h"
#endif
extern EIT_PARAM gParam;
extern imUINT8  Image_Data[CAMERA_H][CAMERA_W];
imUINT8  Image_DataF[CAMERA_H][CAMERA_W];
imINT32  gDir_Near=0;
imINT32  gDir_Mid =0;
imINT32  gDir_Far =0;
imINT16  HBoundL[CAMERA_H];
imINT16  HBoundR[CAMERA_H];
imINT16  HBoundM[CAMERA_H];
imINT16  HBoundM_F[CAMERA_H];
imINT16  HBoundM_REAL[CAM_MAX_LENGTH_CM+1];
imINT32  H_Min=H_START;
imINT32  H_MaxL=H_END;
imINT32  H_MaxR=H_END;
imINT32  D_Max =CAM_MAX_LENGTH_CM;

int MaxWhite=0;
int Out=0;
int MaxStart=W_START;
int MaxEnd=W_START;

int S=0;                //S路标志位

int pixel_change=0;       //像素点突变次数
int num=0;                                 //用于初步判断是否是圆环 
int Circle=0;

int  judge_startline1();                  //新起跑线,用于在起跑线处误识别为圆环 或者十字路
void Graph_ExpandImag(unsigned char* src,int width, int height,int strutWidth, int structHeigh);
int Graph_JudgeOut(void)
{
    static int LeftOut=0;

    int WhiteCnt;
    int W;
    int wStart,wEnd;

   
    MaxWhite=0;
    WhiteCnt=0;
    wStart=W_START;
    wEnd=W_START;
    
    pixel_change=0;
    for(W=W_START;W<W_END;W++)
    {
      
        if((Image_Data[H_END][W]>Image_Data[H_END][W+1]||Image_Data[H_END][W]<Image_Data[H_END][W+1])&&(W+1<=W_END))  //白变黑或者黑跳白
        {
            pixel_change++;
        }
        
        if(Image_Data[H_END][W]==WHITE_VAL&&Image_Data[H_END][W+1]==WHITE_VAL)
          WhiteCnt++;
        else
        {
            if(WhiteCnt>MaxWhite)
            {
                wEnd=W;
                MaxWhite=WhiteCnt;
                MaxStart=wStart;
                MaxEnd=wEnd;
            }
            WhiteCnt=0;
            wStart=W;
        }
    }
    if(WhiteCnt>MaxWhite)
    {
        wEnd=W;
        MaxWhite=WhiteCnt;
        MaxStart=wStart;
        MaxEnd=wEnd;
     }
    
    switch(Out)
    {
       case 0:
          if(MaxWhite<=40)
          {
            if(MaxStart<=W_START)
            {
               Out=1;
               LeftOut=1;
            }
            else if (MaxEnd>=W_END)
            {
               Out=1;
               LeftOut=0;
            }
          }
          break;
       case 1:
       if(MaxWhite>=70)
       {
          if(LeftOut==1 && (MaxStart<=W_START))
             Out=0;
          if(LeftOut==0 && (MaxEnd>=W_END))
             Out=0;
       }
       break;
    }
    return Out;
}
//腐蚀
void MorphErosion(unsigned char* src, unsigned char* dst, int width, int height, int strutWidth, int structHeight)
{
    int i,j,m,n,mid;
    unsigned char val;
    if (width - strutWidth < 0 && height - structHeight < 0)return;
    if (strutWidth != structHeight)return;

    mid = (strutWidth + 1) / 2 - 1;
    for (i = mid; i < height - mid; i++)
    {
        for (j = mid; j < width - mid; j++)
        {
            val = WHITE_VAL;
            for ( m = -mid; m <= mid; m++)
            {
                for (n = -mid; n < mid; n++)
                {
                    
                    if (m == 0 && n == 0 )continue;
                    val &=  src[(i+m) * width + j + n];
                    if(!val)break;
                }
                if(!val)break;
            }
            dst[i * width + j] = val;
            
        }
    }
}
//膨胀
void MorphDilition(unsigned char* src, unsigned char* dst, int width, int height, int strutWidth, int structHeight)
{
    int i,j,m,n,mid;
    unsigned char val = 0;
    if (width - strutWidth < 0 && height - structHeight < 0)return;
    if (strutWidth != structHeight)return;

    mid = (strutWidth + 1) / 2 - 1;
    for (i = mid; i < height - mid; i++)
    {
        for (j = mid; j < width - mid; j++)
        {
            val = 0;
            for (m = -mid; m < mid; m++)
            {
                for (n = -mid; n < mid; n++)
                {
                    if (m == 0 && n == 0 )continue;
                    val |=  src[(i+m) * width + j + n];
                    if(val)break;
                }
                if(val)break;
            }

            dst[i * width + j] = val;
        }
    }
}
//先腐蚀后膨胀
void MorphOpen(unsigned char* src, unsigned char* tmp, int width, int height, int strutWidth, int structHeight)
{
    MorphErosion(src, tmp, width, height, strutWidth, structHeight);
    MorphDilition(tmp, src, width, height, strutWidth, structHeight);
    Graph_ExpandImag(src,width,height,strutWidth,structHeight);
}
void MorphClose(unsigned char* src, unsigned char* tmp, int width, int height, int strutWidth, int structHeight)
{
    MorphDilition(src, tmp, width, height, strutWidth, structHeight); 
    Graph_ExpandImag(tmp,width,height,strutWidth,structHeight);
    MorphErosion(tmp, src, width, height, strutWidth, structHeight);
    Graph_ExpandImag(src,width,height,strutWidth,structHeight);
    
}
void Graph_ExpandImag(unsigned char* src,int width, int height,int strutWidth, int structHeigh)
{
    int i,j,mid;
    mid = (strutWidth + 1) / 2 - 1;
    for (i = 0; i < mid+2; i++)
    {
        for (j = 0; j < width ; j++)
        {
            src[i*width+j]=src[(mid+2)*width+j];
        }
    }
    for (i = height-1; i > height-mid-2; i--)
    {
        for (j = 0; j < width ; j++)
        {
            src[i*width+j]=src[(height-mid-2)*width+j];
        }
    }
    
    for (j = 0; j < mid+1; j++)
    {
        for (i = 0; i < height ; i++)
        {
            src[i*width+j]=src[mid*width+mid+2];
        }
    }
    for (j = width-1; j > width-mid-1; j--)
    {
        for (i = 0; i < height ; i++)
        {
            src[i*width+j]=src[i*width+width-mid-2];
        }
    }
}


//int Graph_FindMidLine()                         //此函数用于模式0
//{
//  
//    /*********障碍物变量*******/                 // L代表左边,R代表右边
//    int Lbarrier_Weight=0;                      //左障碍物的宽度
//    int Lbarrier_end=0;
//    int Lbarrier_H=0;                           //障碍物最近一行的行号
//    int Lbarrierstart=0;
//    int Rbarrier_Weight=0;
//    int Rbarrier_end=0;                         //障碍物靠近赛道中心的一端
//    int Rbarrier_H=0;
//    int Rbarrierstart=0;
//    int Lbarrier=0;                             //标记障碍物的有无
//    int Rbarrier=0;
//    int Lchange=0;                              //记录跳变
//    int Rchange=0;
//    int no=1;                                   //用来排除圆环附近赛道的干扰，以防误识别为障碍物
//    int White_Black=0;
//    /**************************/
//      
//    /********找边线变量********/
//    int H,W;
//    int findLline=0;                            //找到左线标志           
//    int findRline=0;                            //找到右线标志
//    int dDir0,dDir1;                            //十字路变量
//    int GetLineCntL=0;
//    int GetLineCntR=0;
//    int Mid;                                    //每次更新的中线
//    int H1,H2,H3,H4,H5,H6;
//    /**************************/
//      
//    /*********圆环变量*********/  
//    int num=0;                                 //用于初步判断是否是圆环                                 
//    int H_circle=0;                            //代表圆环近端的H   
//    int countL=0,countR=0;
//    int direction=1;                          //车行驶方向控制： 0:圆环左转   1:圆环右转
//    /**************************/
//      
//    /********起始线变量********/
//    int startline=0;                          //用来判断是否是起跑线
//    /**************************/
//    
//    /**********S路变量*********/
//    int White_pixel=0;                          //图像中心白像素点个数
//    int sum;                                    //60-120行的中线之和
//    int S_Dir;                                  //60-120行的偏差均值
//    /**************************/
//    
//    H_Min = H_START;
//    
//    
//    /*******判断是否出界*******/
//    if(Graph_JudgeOut()&&(!(pixel_change>6)))      
//        return 0;
//    else
//    {
//        for (H=0;H<=CAMERA_H;H++)
//        {
//            HBoundL[H]=MaxStart;
//            HBoundR[H]=MaxEnd;
//            HBoundM[H]=(MaxStart+MaxEnd)/2;
//        }
//    }
//    /**************************/
//    
//    dDir0=0;//根据偏差算十字拐角
//    dDir1=0;//根据偏差算十字拐角
//    
//    S=0;
//    DisBuzzer();
//    for(H=H_END;H>=H_START;H--)
//    {   
//        /******S路识别*******/
//        if(Image_Data[H][79]==WHITE_VAL)
//            White_pixel++;
//        /********************/
//        
//        H1=int_min(H+1,H_END);
//        H2=int_min(H+2,H_END);
//        H3=int_min(H+3,H_END);
//        H4=int_min(H+4,H_END);
//        H5=int_min(H+5,H_END);
//        H6=int_min(H+6,H_END);
//        Mid=HBoundM[H+1];
//        
//        if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//        {
//             Mid = W_START+(W_END>>2);
//             if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//             {
//                  Mid = W_END-(W_END>>2);
//                  if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                  {
//                        Mid = W_START+(W_END>>4);
//                        if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                        {
//                             Mid = W_END-(W_END>>4);
//                              if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                              {
//                                   HBoundL[H] = HBoundL[H+1] ;
//                                   HBoundR[H] = HBoundR[H+1] ;
//                                   HBoundM[H] = HBoundM[H+1] ;
//                                    continue;
//                              }
//                        }
//
//                  }
//             }
//        }
//        
//        /***************找左线***************/
//        for(W=W_START;W<Mid;W++)
//        {
//            if (Image_Data[H][W]<Image_Data[H][W+1])
//            {
//
//                    HBoundL[H] = W;
//                    GetLineCntL++;
//                    break;
//            }
//        }
//        
//        if(W==Mid)
//        {
//            HBoundL[H] =HBoundL[H1] ;
//        }
//        /***********************************/
//        
//        
//        /***************找右线***************/
//        for(W=W_END;W>Mid;W--)
//        {
//            if (Image_Data[H][W]<Image_Data[H][W-1])
//            {
//
//                    HBoundR[H] = W;
//
//                    GetLineCntR++;
//                    break;
//            }
//        }
//        
//        if(W==Mid)
//        {
//            HBoundR[H] =HBoundR[H1] ;
//        }
//        /***********************************/     
//        
//        
//        /**************十字路***************/ 
//        dDir0=HBoundR[H]-HBoundR[H2];
//        dDir1=HBoundR[H2]-HBoundR[H6];
//        
//        if((dDir0*dDir1<-4 && dDir0>0)/*&&(!startline) */|| ((dDir0 > 10 && dDir1<=0)/*&&(!startline)*/))             //(!startline):在起跑线处会误识别为十字路，所以加上(!startline)
//        {
//            //十字路补右线
//            HBoundR[H]=HBoundR[H3]+HBoundR[H3]-HBoundR[H6];
//            HBoundR[H1]=HBoundR[H4]+HBoundR[H3]-HBoundR[H6];
//        }
//        
//        dDir0=HBoundL[H]-HBoundL[H2];
//        dDir1=HBoundL[H2]-HBoundL[H6];
//        
//        
//        if((dDir0*dDir1<-4 && dDir0<0)/*&&(!startline)*/ || ((dDir0 <-10  && dDir1>=0)/*&&(!startline)*/))            
//        {
//            //十字路补左线
//            HBoundL[H]=HBoundL[H3]+HBoundL[H3]-HBoundL[H6];
//            HBoundL[H1]=HBoundL[H4]+HBoundL[H3]-HBoundL[H6];
//        }
//       
//        HBoundM[H]= (HBoundR[H]+ HBoundL[H])/2;
//        if( HBoundR[H] < W_START+LINE_WIDTH || HBoundL[H] > W_END-LINE_WIDTH || (HBoundR[H]-HBoundL[H]) <LINE_WIDTH )
//        {
//             H_Min = H;
//             break;
//        }
//        /***********************************/ 
//    }
//     
//     /******S路识别*******/
//      sum=0;
//      for(H=H_END;H>60;H--)
//      {
//          sum+=HBoundM[H];
//      }
//      S_Dir=sum/(H_END-60)-79;
//      
//      if(White_pixel>100&&((S_Dir<25&&S_Dir>1)||(S_Dir>-25&&S_Dir<-1))&&(int_abs((HBoundM[60]+HBoundM[H_END])/2-HBoundM[90])>5))
//      {
//          S=1;                    //S路标志位
//          EnBuzzer();             //蜂鸣器响
//      }
//    /********************/
//    
//    
//     /********************圆环*********************/
//     for(H=80;H>30;H--)                          
//     {
//        if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)           //记录图像中心线有没有跳变
//        {    num=1;
//            H_circle=120-(H-1);
//            break;
//        }
//     }
//    
//     if(num==1)
//     {
//         for(H=H_END;H>=120-H_circle;H--)
//         {
//              if(Image_Data[H][0]==WHITE_VAL)
//                   countL++;
//              
//              if(Image_Data[H][159]==WHITE_VAL)
//                   countR++;
//
//         }
//     }
//     
//    if(countL>=H_circle-30&&(countR>=H_circle-30)/*&&Image_Data[120-H_circle-3][159]==WHITE_VAL*/&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle)/*&&(!startline)*/)       //H_circle&&Image_Data[35][40]!=WHITE_VAL:在十字路处如果车身比较斜，十字路会与圆环出口差不多，所以加上这个这个条件
//     {   
//       if(direction)
//       {
//           for(H=H_END;H>=H_START;H--)                                     //圆环补右线，即圆环左转
//           {
//               HBoundR[H]=160-(H-(120-H_circle))*160/(double)(H_circle);
//               HBoundL[H]=159;
//               if(H<(110-H_circle))
//               {
//                   HBoundR[H]=159;
//                 
//               }
//               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//           }
//       }
//         
//       else 
//           for(H=H_END;H>=H_START;H--)                                     //圆环补左线
//           {
//               HBoundL[H]=(H-(120-H_circle))*160/(double)(H_circle);
//               HBoundR[H]=0;
//               if(H<(110-H_circle))
//               {
//                   HBoundL[H]=0;
//                   
//               }
//               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//           }
//     }
//     /*********************************************/
//     
//    /*******************障碍物********************/
//        for(H=100;H>20;H--)                                 //50                    开始是100到40
//        {
//            /********识别左障碍物*******/
//            Lbarrier_Weight=0;
//            Lchange=0;                                                       //在弯道,可能上一行Rchange=1;
//            for(W=HBoundM[H];W>HBoundL[H];W--)                              //检测左障碍物
//            {       
//                if (Image_Data[H][W]>Image_Data[H][W-1]||Lchange)
//                {   
//                    Lchange=1;                                              //有白到黑的跳变
//                    if(Lchange==1&&Image_Data[H][W-1]==BLACK_VAL&&Image_Data[H][W-2]==BLACK_VAL)
//                        Lbarrier_Weight++;                                  //障碍物宽度
//                    else
//                    {
//                         Lbarrier_end=W;
//                         Lbarrier_H=H;
//                         Lchange=0;
//                         break;
//                    } 
//                }
//            }
//            Lbarrierstart=Lbarrier_end+Lbarrier_Weight;                     //障碍物靠近赛道中心的一端
//           /****************************/
//            
//           /********识别右障碍物********/
//           Rbarrier_Weight=0;
//           Rchange=0;                                                       //在弯道,可能上一行Rchange=1;
//           for(W=HBoundM[H];W<HBoundR[H];W++)                               //检测左障碍物
//           {
//                if (Image_Data[H][W]>Image_Data[H][W+1]||Rchange)
//                {   
//                    Rchange=1;                                              //有白到黑的跳变
//                    if(Rchange==1&&Image_Data[H][W+1]==BLACK_VAL&&Image_Data[H][W+2]==BLACK_VAL)
//                        Rbarrier_Weight++;                                  //障碍物宽度
//                    else
//                    {
//                         Rbarrier_end=W;
//                         Rbarrier_H=H;
//                         Rchange=0;
//                         break;
//                    } 
//                }
//                if(HBoundM[H]-HBoundM[H-1]>10)                             //由于赛道环境限制，在圆环那里，周围的赛道太靠近圆环，会误识别为障碍物，所以这个if语句用来排除周围赛道的干扰
//                {
//                    no=0;                                                  //代表不是障碍物，而是周围赛道干扰
//                }
//           }
//           Rbarrierstart=Rbarrier_end-Rbarrier_Weight;                     //障碍物靠近赛道中心的一端
//           /***************************/
//           
//           if(Lbarrier_Weight>20&&HBoundM[Lbarrier_H]!=BLACK_VAL)          //排除远处干扰  ：HBoundM[Rbarrier_H]!=BLACK_VAL                                 
//           {
//                Lbarrier=1;
//                break;
//           }
//           if(Rbarrier_Weight>20&&HBoundM[Rbarrier_H]!=BLACK_VAL)          //Lbarrier_Weight设为10时，起跑线会当成障碍物，所以改为20
//           {
//                Rbarrier=1;
//                break;
//           }
//        }
//    /*******************障碍物补线********************/
//    if(Lbarrier==1&&no&&(Lbarrier_H>H_Min))                            //(Lbarrier_H>H_Min&&(H_Min!=H_START)):排除弯道干扰，急弯时，会误识别为障碍物
//       {
//          for(H=Lbarrier_H;H<H_END;H++)
//          {
//              HBoundL[H]=Lbarrierstart-(H-Lbarrier_H)*Lbarrierstart/(H_END-Lbarrier_H)+10;         //障碍物补线
//          }
//          for(H=Lbarrier_H-1;H>=H_START;H--)
//          {
//              HBoundL[H]=HBoundL[H+1];
//          }
//       }
//       
//    if(Rbarrier==1&&no&&(Rbarrier_H>H_Min))
//       {
//          for(H=Rbarrier_H;H<H_END;H++)
//          {
//              HBoundR[H]=Rbarrierstart+(H-Rbarrier_H)*(160-Rbarrierstart)/(H_END-Rbarrier_H)-10;
//          }
//          for(H=Rbarrier_H-1;H>=H_START;H--)
//          {
//              HBoundR[H]=HBoundR[H+1];
//          }
//       }
//    
//      for(H=H_END;H>=H_START;H--)                                     //障碍物中线规划
//      {
//           
//           HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//          
//          
//      }   
//    /**********************************************/
//     
//    /*for(H=int_min(H_MaxL,H_MaxR);H<=H_END;H++)
//    {
//         HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//    }*/
//    return H_circle;                                                
//    
//}

int Graph_FindMidLine()                         //此函数用于模式1
{
  
    /*********障碍物变量*******/                 // L代表左边,R代表右边
    int Lbarrier_Weight=0;                      //左障碍物的宽度
    int Lbarrier_end=0;
    int Lbarrier_H=0;                           //障碍物最近一行的行号
    int Lbarrierstart=0;
    int Rbarrier_Weight=0;
    int Rbarrier_end=0;                         //障碍物靠近赛道中心的一端
    int Rbarrier_H=0;
    int Rbarrierstart=0;
    int Lbarrier=0;                             //标记障碍物的有无
    int Rbarrier=0;
    int Lchange=0;                              //记录跳变
    int Rchange=0;
    int no=1;                                   //用来排除圆环附近赛道的干扰，以防误识别为障碍物
    int White_Black=0;
    /**************************/
      
    /********找边线变量********/
    int H,W;
    int findLline=0;                            //找到左线标志           
    int findRline=0;                            //找到右线标志
    imINT16  BoundL[CAMERA_H];
    imINT16  BoundR[CAMERA_H];
    int dDir0,dDir1;                            //十字路变量
    int GetLineCntL=0;
    int GetLineCntR=0;
    int Mid;                                    //每次更新的中线
    int H1,H2,H3,H4,H5,H6;
    /**************************/
      
    /*********圆环变量*********/  
//    int num=0;                                 //用于初步判断是否是圆环                                 
    int H_circle=0;                            //代表圆环近端的H   
    int countL=0,countR=0;
    int direction=0;                          //车行驶方向控制： 0:圆环左转   1:圆环右转
    int pixel_change1=0;
    int lastL=0,nowL=0,lastR=0,nowR=0;
    int no_getLineL=0,no_getLineR=0;
    /**************************/
      
    /********起始线变量********/
    int startline=0;                          //用来判断是否是起跑线
    /**************************/
    
    /**********S路变量*********/
    int White_pixel=0;                          //图像中心白像素点个数
    int sum;                                    //60-120行的中线之和
    int S_Dir;                                  //60-120行的偏差均值
    /**************************/
    
    H_Min = H_START;
    
    
    /*******判断是否出界*******/
    if(Graph_JudgeOut()&&(!(pixel_change>6)))      
        return 0;
    else
    {
        for (H=0;H<=CAMERA_H;H++)
        {
            HBoundL[H]=MaxStart;
            HBoundR[H]=MaxEnd;
            HBoundM[H]=(MaxStart+MaxEnd)/2;
        }
         // H_Min = H_START;                      //这个语句没作用，只是为了程序的正确执行，因为如果else里没有语句，遇见圆环会死机
    }
    /**************************/
    
    dDir0=0;//根据偏差算十字拐角
    dDir1=0;//根据偏差算十字拐角
    
    num=0;
    int point_A=0,point_B=0,point_C=0,point_D=0;        //圆环 上 下 左 右 四个点
    int center=0;                                       //圆环中心
    DisBuzzer();
    Circle=0;
    for(H=110;H>30;H--)                          
    {
       if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL&&H_circle==0)           //记录图像中心线有没有跳变
       {    
           num=1;
           H_circle=120-(H-1);
           //break;
       }
       if(H>10&&H<60)
       {
           if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL&&point_A==0)
           {
              point_A=H-1;
              //EnBuzzer();
           }
           if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H+1][W_MID]==BLACK_VAL&&point_B==0)
           {
              point_B=H+1;
              //EnBuzzer();
           }
           if(point_A!=0&&point_B!=0)
              break;
       }
    }
    if(point_A-point_B>15&&(point_A>25&&point_B<25))
    {
        center=(point_A+point_B)/2;
    }
        
//        for(W=W_MID;W>H_START;H--)
//        {
//            if(Image_Data[center][W-1]==WHITE_VAL&&Image_Data[center][W]==BLACK_VAL)
//            {
//                point_C=W;
//                break;
//            }
//        }
//        
//        for(W=W_MID;W<H_END;H++)
//        {
//            if(Image_Data[center][W]==WHITE_VAL&&Image_Data[center][W-1]==BLACK_VAL)
//            {
//                point_D=W-1;
//                break;
//            }
//        }
  //  }
//    if(point_D-point_C>60)
//    {
//        EnBuzzer();
//    }
    
    S=0;
    //DisBuzzer();
    for(H=H_END;H>=H_START;H--)
    {   
        /******S路识别*******/
        if(Image_Data[H][79]==WHITE_VAL)
            White_pixel++;
        /********************/
        
        H1=int_min(H+1,H_END);
        H2=int_min(H+2,H_END);
        H3=int_min(H+3,H_END);
        H4=int_min(H+4,H_END);
        H5=int_min(H+5,H_END);
        H6=int_min(H+6,H_END);
        Mid=HBoundM[H+1];
        
        if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
        {
             Mid = W_START+(W_END>>2);
             if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
             {
                  Mid = W_END-(W_END>>2);
                  if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
                  {
                        Mid = W_START+(W_END>>4);
                        if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
                        {
                             Mid = W_END-(W_END>>4);
                              if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
                              {
                                   HBoundL[H] = HBoundL[H+1] ;
                                   HBoundR[H] = HBoundR[H+1] ;
                                   HBoundM[H] = HBoundM[H+1] ;
                                    continue;
                              }
                        }

                  }
             }
        }
        
        /***************找左线***************/
        for(W=W_START;W<Mid;W++)
        {
            if (Image_Data[H][W]<Image_Data[H][W+1])
            {

                    HBoundL[H] = W;
//                    if(int_abs(HBoundL[H]-HBoundL[H1])>5)
//                        HBoundL[H]=HBoundL[H1];
                    GetLineCntL++;
                    break;
            }
        }
        if(W==Mid)
        {
            lastL=nowL;
            nowL=1;
            if(((H>121-H_circle)&&(H<121-H_circle+20))&&(lastL==1&&nowL==1)&&num==1)
                no_getLineL++;
                
            HBoundL[H] =HBoundL[H1] ;
        }
        
        /***********************************/
        
        
        /***************找右线***************/
        for(W=W_END;W>Mid;W--)
        {
            if (Image_Data[H][W]<Image_Data[H][W-1])
            {

                    HBoundR[H] = W;
//                    if(int_abs(HBoundR[H]-HBoundR[H1])>5)
//                        HBoundR[H]=HBoundR[H1];
                    GetLineCntR++;
                    break;
            }
        }
        if(W==Mid)
        {
            lastR=nowR;
            nowR=1;
            if(((H>121-H_circle)&&(H<121-H_circle+20))&&(lastR==1&&nowR==1)&&num==1)
                no_getLineR++;
          
            HBoundR[H] =HBoundR[H1] ;
        }
        
        if(Image_Data[center][W_MID]==BLACK_VAL&&Image_Data[center][W_MID-1]==BLACK_VAL&&Image_Data[center][W_MID+1]==BLACK_VAL&&Image_Data[center][W_MID-5]==BLACK_VAL&&Image_Data[center][W_MID+5]==BLACK_VAL&&Image_Data[center][W_MID+15]==BLACK_VAL&&Image_Data[center][W_MID-15]==BLACK_VAL&&Image_Data[center][25]==WHITE_VAL&&Image_Data[center][135]==WHITE_VAL&&(no_getLineR>10)&&(no_getLineL>10))
        {
            EnBuzzer();
            Circle=1;
        }
        /***********************************/     
        
        
        /**************十字路***************/ 
        dDir0=HBoundR[H]-HBoundR[H2];
        dDir1=HBoundR[H2]-HBoundR[H6];
        
        if((dDir0*dDir1<-4 && dDir0>0)/*&&(!startline) */|| ((dDir0 > 10 && dDir1<=0)/*&&(!startline)*/))             //(!startline):在起跑线处会误识别为十字路，所以加上(!startline)
        {
            //十字路补右线
            HBoundR[H]=HBoundR[H3]+HBoundR[H3]-HBoundR[H6];
            HBoundR[H1]=HBoundR[H4]+HBoundR[H3]-HBoundR[H6];
        }
        
        dDir0=HBoundL[H]-HBoundL[H2];
        dDir1=HBoundL[H2]-HBoundL[H6];
        
        
        if((dDir0*dDir1<-4 && dDir0<0)/*&&(!startline)*/ || ((dDir0 <-10  && dDir1>=0)/*&&(!startline)*/))            
        {
            //十字路补左线
            HBoundL[H]=HBoundL[H3]+HBoundL[H3]-HBoundL[H6];
            HBoundL[H1]=HBoundL[H4]+HBoundL[H3]-HBoundL[H6];
        }
       
        HBoundM[H]= (HBoundR[H]+ HBoundL[H])/2;
        if( HBoundR[H] < W_START+LINE_WIDTH || HBoundL[H] > W_END-LINE_WIDTH || (HBoundR[H]-HBoundL[H]) <LINE_WIDTH )
        {
             H_Min = H;
             break;
        }
        /***********************************/ 

    }
    
    /******S路识别*******/
    sum=0;
    for(H=H_END;H>60;H--)
    {
        sum+=HBoundM[H];
    }
    S_Dir=sum/(H_END-60)-79;
    
    if(White_pixel>100&&((S_Dir<25&&S_Dir>1)||(S_Dir>-25&&S_Dir<-1))&&(int_abs((HBoundM[60]+HBoundM[H_END])/2-HBoundM[90])>5))
    {
        S=1;                    //S路标志位
        //EnBuzzer();             //蜂鸣器响
    }
    /********************/
    
        
     /********************圆环*********************/
     
    int countbl=0,countbr=0;
     for(W=W_START;W<W_END;W++)
     {
         if((Image_Data[121-H_circle-2][W]>Image_Data[121-H_circle-2][W+1]||Image_Data[121-H_circle-2][W]<Image_Data[121-H_circle-2][W+1])&&(W+1<=W_END))  //白变黑或者黑跳白
        {
            pixel_change1++;                              //像素点突变次数
        }
     }
    
     if(num==1&&(pixel_change1<6))
     {
         for(H=120-H_circle;H<=H_END;H++)
         {
              if(Image_Data[H][0]==WHITE_VAL)
              {
                   countL++;                                   //woxiedelalalallallalla
                if(countL>=H_circle-30)
                 break;
              }
           else    
           {   countbl++;
              if(countbl>4)
              {
                countL=0;
                countbl=0;
                break;
              }
           }
         }
         
         for(H=H_END;H>=120-H_circle;H--)
         {
              if(Image_Data[H][159]==WHITE_VAL)
              {
                   countR++;                                   //woxiedelalalallallalla
                if(countR>=H_circle-30)
                 break;
              }
             else            
             {
               countbr++;
               if(countbr>4)
               {
               countR=0;
               countbr=0;
                 break;
               }
             }
         }
     }
    //Circle=0;
//    int a,b,c,d;
//     for(H=H_END;H>0;H--)                          
//     {
//      
//        if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)           //记录图像中心线有没有跳变
//        { 
//          a=H-1;
//          break;
//        }
//     }
//    for(H=a;H>0;H--)
//    {
//       if(Image_Data[H][W_MID]==BLACK_VAL&&Image_Data[H-1][W_MID]==WHITE_VAL)
//       {
//         b=H;
//       break;
//       }
//    }
//      for(H=b;H>0;H--)
//    {
//         if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)
//       {
//         d=H;
//       break;
//       }
//    }
//    c=a-b;
    //if((countL>=H_circle-30&&(countR>=H_circle-30)/*&&Image_Data[120-H_circle-3][159]==WHITE_VAL*/&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))||((d>5)&&(c>40)&&(countL>=30)&&(countR>=30)&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))/*&&(!startline)*/)       //H_circle&&Image_Data[35][40]!=WHITE_VAL:在十字路处如果车身比较斜，十字路会与圆环出口差不多，所以加上这个这个条件
     if(Circle==1||((H>30&&H<=80)&&(countL>=H_circle-30)&&(countR>=H_circle-30)&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))||((H>30&&H<=110)&&(countL>=H_circle-5)&&(countR>=H_circle-5)&&H_circle&&Image_Data[35][40]!=WHITE_VAL)||(no_getLineL>10&&no_getLineR>10&&Image_Data[25][55]!=WHITE_VAL))       //H_circle&&Image_Data[35][40]!=WHITE_VAL:在十字路处如果车身比较斜，十字路会与圆环出口差不多，所以加上这个这个条件
     {   
       Circle=1;
       if(direction)
       {
           for(H=H_END;H>=H_START;H--)                                     //圆环补右线，即圆环左转
           {
               HBoundR[H]=160-(H-(120-H_circle))*160/(double)(H_circle);
               HBoundL[H]=159;
               if(H<(110-H_circle))
               {
                   HBoundR[H]=159;
                 
               }
               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
           }
       }
         
       else 
           for(H=H_END;H>=H_START;H--)                                     //圆环补左线
           {
               HBoundL[H]=(H-(120-H_circle))*160/(double)(H_circle);
               HBoundR[H]=0;
               if(H<(110-H_circle))
               {
                   HBoundL[H]=0;
                   
               }
               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
           }
     }
     /*********************************************/
     
     /*******************障碍物********************/
    for(H=100;H>20;H--)                                 //50                    开始是100到40
    {
        /********识别左障碍物*******/
        Lbarrier_Weight=0;
        Lchange=0;                                                       //在弯道,可能上一行Rchange=1;
        for(W=HBoundM[H];W>HBoundL[H];W--)                              //检测左障碍物
        {       
            if (Image_Data[H][W]>Image_Data[H][W-1]||Lchange)
            {   
                Lchange=1;                                              //有白到黑的跳变
                if(Lchange==1&&Image_Data[H][W-1]==BLACK_VAL&&Image_Data[H][W-2]==BLACK_VAL)
                    Lbarrier_Weight++;                                  //障碍物宽度
                else
                {
                     Lbarrier_end=W;
                     Lbarrier_H=H;
                     Lchange=0;
                     break;
                } 
            }
        }
        Lbarrierstart=Lbarrier_end+Lbarrier_Weight;                     //障碍物靠近赛道中心的一端
       /****************************/
        
       /********识别右障碍物********/
       Rbarrier_Weight=0;
       Rchange=0;                                                       //在弯道,可能上一行Rchange=1;
       for(W=HBoundM[H];W<HBoundR[H];W++)                               //检测左障碍物
       {
            if (Image_Data[H][W]>Image_Data[H][W+1]||Rchange)
            {   
                Rchange=1;                                              //有白到黑的跳变
                if(Rchange==1&&Image_Data[H][W+1]==BLACK_VAL&&Image_Data[H][W+2]==BLACK_VAL)
                    Rbarrier_Weight++;                                  //障碍物宽度
                else
                {
                     Rbarrier_end=W;
                     Rbarrier_H=H;
                     Rchange=0;
                     break;
                } 
            }
            if(HBoundM[H]-HBoundM[H-1]>10)                             //由于赛道环境限制，在圆环那里，周围的赛道太靠近圆环，会误识别为障碍物，所以这个if语句用来排除周围赛道的干扰
            {
                no=0;                                                  //代表不是障碍物，而是周围赛道干扰
            }
       }
       Rbarrierstart=Rbarrier_end-Rbarrier_Weight;                     //障碍物靠近赛道中心的一端
       /***************************/
       
       if(Lbarrier_Weight>20&&HBoundM[Lbarrier_H]!=BLACK_VAL)          //排除远处干扰  ：HBoundM[Rbarrier_H]!=BLACK_VAL                                 
       {
            Lbarrier=1;
            break;
       }
       if(Rbarrier_Weight>20&&HBoundM[Rbarrier_H]!=BLACK_VAL)          //Lbarrier_Weight设为10时，起跑线会当成障碍物，所以改为20
       {
            Rbarrier=1;
            break;
       }
    }
    
    /*******************障碍物补线********************/
    if(Lbarrier==1&&no&&(Lbarrier_H>H_Min))                            //(Lbarrier_H>H_Min&&(H_Min!=H_START)):排除弯道干扰，急弯时，会误识别为障碍物
       {
          for(H=Lbarrier_H;H<H_END;H++)
          {
              HBoundL[H]=Lbarrierstart-(H-Lbarrier_H)*Lbarrierstart/(H_END-Lbarrier_H)+10;         //障碍物补线
          }
          for(H=Lbarrier_H-1;H>=H_START;H--)
          {
              HBoundL[H]=HBoundL[H+1];
          }
       }
       
       if(Rbarrier==1&&no&&(Rbarrier_H>H_Min))
       {
          for(H=Rbarrier_H;H<H_END;H++)
          {
              HBoundR[H]=Rbarrierstart+(H-Rbarrier_H)*(160-Rbarrierstart)/(H_END-Rbarrier_H)-10;
          }
          for(H=Rbarrier_H-1;H>=H_START;H--)
          {
              HBoundR[H]=HBoundR[H+1];
          }
       }
    
      for(H=H_END;H>=H_START;H--)                                     //障碍物中线规划
      {
           
           HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
          
          
      }  
    /**********************************************/
     
    /*for(H=int_min(H_MaxL,H_MaxR);H<=H_END;H++)
    {
         HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
    }*/
    return S;                                                
    
}

int Graph_FindMidLine1()                         //此函数用于模式1
{
  
    /*********障碍物变量*******/                 // L代表左边,R代表右边
    int Lbarrier_Weight=0;                      //左障碍物的宽度
    int Lbarrier_end=0;
    int Lbarrier_H=0;                           //障碍物最近一行的行号
    int Lbarrierstart=0;
    int Rbarrier_Weight=0;
    int Rbarrier_end=0;                         //障碍物靠近赛道中心的一端
    int Rbarrier_H=0;
    int Rbarrierstart=0;
    int Lbarrier=0;                             //标记障碍物的有无
    int Rbarrier=0;
    int Lchange=0;                              //记录跳变
    int Rchange=0;
    int no=1;                                   //用来排除圆环附近赛道的干扰，以防误识别为障碍物
    int White_Black=0;
    /**************************/
      
    /********找边线变量********/
    int H,W;
    int findLline=0;                            //找到左线标志           
    int findRline=0;                            //找到右线标志
    imINT16  BoundL[CAMERA_H];
    imINT16  BoundR[CAMERA_H];
    int dDir0,dDir1;                            //十字路变量
    int GetLineCntL=0;
    int GetLineCntR=0;
    int Mid;                                    //每次更新的中线
    int H1,H2,H3,H4,H5,H6;
    /**************************/
      
    /*********圆环变量*********/  
//    int num=0;                                 //用于初步判断是否是圆环                                 
    int H_circle=0;                            //代表圆环近端的H   
    int countL=0,countR=0;
    int direction=1;                          //车行驶方向控制： 0:圆环左转   1:圆环右转
    int pixel_change1=0;
    int lastL=0,nowL=0,lastR=0,nowR=0;
    int no_getLineL=0,no_getLineR=0;
    /**************************/
      
    /********起始线变量********/
    int startline=0;                          //用来判断是否是起跑线
    /**************************/
    
    /**********S路变量*********/
    int White_pixel=0;                          //图像中心白像素点个数
    int sum;                                    //60-120行的中线之和
    int S_Dir;                                  //60-120行的偏差均值
    /**************************/
    
    H_Min = H_START;
    
    
    /*******判断是否出界*******/
    if(Graph_JudgeOut()&&(!(pixel_change>6)))      
        return 0;
    else
    {
        for (H=0;H<=CAMERA_H;H++)
        {
            HBoundL[H]=MaxStart;
            HBoundR[H]=MaxEnd;
            HBoundM[H]=(MaxStart+MaxEnd)/2;
        }
         // H_Min = H_START;                      //这个语句没作用，只是为了程序的正确执行，因为如果else里没有语句，遇见圆环会死机
    }
    /**************************/
    
    dDir0=0;//根据偏差算十字拐角
    dDir1=0;//根据偏差算十字拐角
    
    num=0;
    int point_A=0,point_B=0,point_C=0,point_D=0;        //圆环 上 下 左 右 四个点
    int center=0;                                       //圆环中心
    DisBuzzer();
    Circle=0;
    for(H=110;H>30;H--)                          
    {
       if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL&&H_circle==0)           //记录图像中心线有没有跳变
       {    
           num=1;
           H_circle=120-(H-1);
           //break;
       }
       if(H>10&&H<60)
       {
           if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL&&point_A==0)
           {
              point_A=H-1;
              //EnBuzzer();
           }
           if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H+1][W_MID]==BLACK_VAL&&point_B==0)
           {
              point_B=H+1;
              //EnBuzzer();
           }
           if(point_A!=0&&point_B!=0)
              break;
       }
    }
    if(point_A-point_B>15&&(point_A>25&&point_B<25))
    {
        center=(point_A+point_B)/2;
    }
        
//        for(W=W_MID;W>H_START;H--)
//        {
//            if(Image_Data[center][W-1]==WHITE_VAL&&Image_Data[center][W]==BLACK_VAL)
//            {
//                point_C=W;
//                break;
//            }
//        }
//        
//        for(W=W_MID;W<H_END;H++)
//        {
//            if(Image_Data[center][W]==WHITE_VAL&&Image_Data[center][W-1]==BLACK_VAL)
//            {
//                point_D=W-1;
//                break;
//            }
//        }
  //  }
//    if(point_D-point_C>60)
//    {
//        EnBuzzer();
//    }
    
    S=0;
    //DisBuzzer();
    for(H=H_END;H>=H_START;H--)
    {   
        /******S路识别*******/
        if(Image_Data[H][79]==WHITE_VAL)
            White_pixel++;
        /********************/
        
        H1=int_min(H+1,H_END);
        H2=int_min(H+2,H_END);
        H3=int_min(H+3,H_END);
        H4=int_min(H+4,H_END);
        H5=int_min(H+5,H_END);
        H6=int_min(H+6,H_END);
        Mid=HBoundM[H+1];
        
        if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
        {
             Mid = W_START+(W_END>>2);
             if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
             {
                  Mid = W_END-(W_END>>2);
                  if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
                  {
                        Mid = W_START+(W_END>>4);
                        if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
                        {
                             Mid = W_END-(W_END>>4);
                              if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
                              {
                                   HBoundL[H] = HBoundL[H+1] ;
                                   HBoundR[H] = HBoundR[H+1] ;
                                   HBoundM[H] = HBoundM[H+1] ;
                                    continue;
                              }
                        }

                  }
             }
        }
        
        /***************找左线***************/
        for(W=W_START;W<Mid;W++)
        {
            if (Image_Data[H][W]<Image_Data[H][W+1])
            {

                    HBoundL[H] = W;
//                    if(int_abs(HBoundL[H]-HBoundL[H1])>5)
//                        HBoundL[H]=HBoundL[H1];
                    GetLineCntL++;
                    break;
            }
        }
        if(W==Mid)
        {
            lastL=nowL;
            nowL=1;
            if(((H>121-H_circle)&&(H<121-H_circle+20))&&(lastL==1&&nowL==1)&&num==1)
                no_getLineL++;
                
            HBoundL[H] =HBoundL[H1] ;
        }
        
        /***********************************/
        
        
        /***************找右线***************/
        for(W=W_END;W>Mid;W--)
        {
            if (Image_Data[H][W]<Image_Data[H][W-1])
            {

                    HBoundR[H] = W;
//                    if(int_abs(HBoundR[H]-HBoundR[H1])>5)
//                        HBoundR[H]=HBoundR[H1];
                    GetLineCntR++;
                    break;
            }
        }
        if(W==Mid)
        {
            lastR=nowR;
            nowR=1;
            if(((H>121-H_circle)&&(H<121-H_circle+20))&&(lastR==1&&nowR==1)&&num==1)
                no_getLineR++;
          
            HBoundR[H] =HBoundR[H1] ;
        }
        
        if(Image_Data[center][W_MID]==BLACK_VAL&&Image_Data[center][W_MID-1]==BLACK_VAL&&Image_Data[center][W_MID+1]==BLACK_VAL&&Image_Data[center][W_MID-5]==BLACK_VAL&&Image_Data[center][W_MID+5]==BLACK_VAL&&Image_Data[center][W_MID+15]==BLACK_VAL&&Image_Data[center][W_MID-15]==BLACK_VAL&&Image_Data[center][25]==WHITE_VAL&&Image_Data[center][135]==WHITE_VAL&&(no_getLineR>10)&&(no_getLineL>10))
        {
            EnBuzzer();
            Circle=1;
        }
        /***********************************/     
        
        
        /**************十字路***************/ 
        dDir0=HBoundR[H]-HBoundR[H2];
        dDir1=HBoundR[H2]-HBoundR[H6];
        
        if((dDir0*dDir1<-4 && dDir0>0)/*&&(!startline) */|| ((dDir0 > 10 && dDir1<=0)/*&&(!startline)*/))             //(!startline):在起跑线处会误识别为十字路，所以加上(!startline)
        {
            //十字路补右线
            HBoundR[H]=HBoundR[H3]+HBoundR[H3]-HBoundR[H6];
            HBoundR[H1]=HBoundR[H4]+HBoundR[H3]-HBoundR[H6];
        }
        
        dDir0=HBoundL[H]-HBoundL[H2];
        dDir1=HBoundL[H2]-HBoundL[H6];
        
        
        if((dDir0*dDir1<-4 && dDir0<0)/*&&(!startline)*/ || ((dDir0 <-10  && dDir1>=0)/*&&(!startline)*/))            
        {
            //十字路补左线
            HBoundL[H]=HBoundL[H3]+HBoundL[H3]-HBoundL[H6];
            HBoundL[H1]=HBoundL[H4]+HBoundL[H3]-HBoundL[H6];
        }
       
        HBoundM[H]= (HBoundR[H]+ HBoundL[H])/2;
        if( HBoundR[H] < W_START+LINE_WIDTH || HBoundL[H] > W_END-LINE_WIDTH || (HBoundR[H]-HBoundL[H]) <LINE_WIDTH )
        {
             H_Min = H;
             break;
        }
        /***********************************/ 

    }
    
    /******S路识别*******/
    sum=0;
    for(H=H_END;H>60;H--)
    {
        sum+=HBoundM[H];
    }
    S_Dir=sum/(H_END-60)-79;
    
    if(White_pixel>100&&((S_Dir<25&&S_Dir>1)||(S_Dir>-25&&S_Dir<-1))&&(int_abs((HBoundM[60]+HBoundM[H_END])/2-HBoundM[90])>5))
    {
        S=1;                    //S路标志位
        //EnBuzzer();             //蜂鸣器响
    }
    /********************/
    
        
     /********************圆环*********************/
     
    int countbl=0,countbr=0;
     for(W=W_START;W<W_END;W++)
     {
         if((Image_Data[121-H_circle-2][W]>Image_Data[121-H_circle-2][W+1]||Image_Data[121-H_circle-2][W]<Image_Data[121-H_circle-2][W+1])&&(W+1<=W_END))  //白变黑或者黑跳白
        {
            pixel_change1++;                              //像素点突变次数
        }
     }
    
     if(num==1&&(pixel_change1<6))
     {
         for(H=120-H_circle;H<=H_END;H++)
         {
              if(Image_Data[H][0]==WHITE_VAL)
              {
                   countL++;                                   //woxiedelalalallallalla
                if(countL>=H_circle-30)
                 break;
              }
           else    
           {   countbl++;
              if(countbl>4)
              {
                countL=0;
                countbl=0;
                break;
              }
           }
         }
         
         for(H=H_END;H>=120-H_circle;H--)
         {
              if(Image_Data[H][159]==WHITE_VAL)
              {
                   countR++;                                   //woxiedelalalallallalla
                if(countR>=H_circle-30)
                 break;
              }
             else            
             {
               countbr++;
               if(countbr>4)
               {
               countR=0;
               countbr=0;
                 break;
               }
             }
         }
     }
    //Circle=0;
//    int a,b,c,d;
//     for(H=H_END;H>0;H--)                          
//     {
//      
//        if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)           //记录图像中心线有没有跳变
//        { 
//          a=H-1;
//          break;
//        }
//     }
//    for(H=a;H>0;H--)
//    {
//       if(Image_Data[H][W_MID]==BLACK_VAL&&Image_Data[H-1][W_MID]==WHITE_VAL)
//       {
//         b=H;
//       break;
//       }
//    }
//      for(H=b;H>0;H--)
//    {
//         if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)
//       {
//         d=H;
//       break;
//       }
//    }
//    c=a-b;
    //if((countL>=H_circle-30&&(countR>=H_circle-30)/*&&Image_Data[120-H_circle-3][159]==WHITE_VAL*/&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))||((d>5)&&(c>40)&&(countL>=30)&&(countR>=30)&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))/*&&(!startline)*/)       //H_circle&&Image_Data[35][40]!=WHITE_VAL:在十字路处如果车身比较斜，十字路会与圆环出口差不多，所以加上这个这个条件
     if(Circle==1||((H>30&&H<=80)&&(countL>=H_circle-30)&&(countR>=H_circle-30)&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))||((H>30&&H<=110)&&(countL>=H_circle-5)&&(countR>=H_circle-5)&&H_circle&&Image_Data[35][40]!=WHITE_VAL)||(no_getLineL>10&&no_getLineR>10&&Image_Data[25][55]!=WHITE_VAL))       //H_circle&&Image_Data[35][40]!=WHITE_VAL:在十字路处如果车身比较斜，十字路会与圆环出口差不多，所以加上这个这个条件
     {   
       Circle=1;
       if(direction)
       {
           for(H=H_END;H>=H_START;H--)                                     //圆环补右线，即圆环左转
           {
               HBoundR[H]=160-(H-(120-H_circle))*160/(double)(H_circle);
               HBoundL[H]=159;
               if(H<(110-H_circle))
               {
                   HBoundR[H]=159;
                 
               }
               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
           }
       }
         
       else 
           for(H=H_END;H>=H_START;H--)                                     //圆环补左线
           {
               HBoundL[H]=(H-(120-H_circle))*160/(double)(H_circle);
               HBoundR[H]=0;
               if(H<(110-H_circle))
               {
                   HBoundL[H]=0;
                   
               }
               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
           }
     }
     /*********************************************/
     
     /*******************障碍物********************/
    for(H=100;H>20;H--)                                 //50                    开始是100到40
    {
        /********识别左障碍物*******/
        Lbarrier_Weight=0;
        Lchange=0;                                                       //在弯道,可能上一行Rchange=1;
        for(W=HBoundM[H];W>HBoundL[H];W--)                              //检测左障碍物
        {       
            if (Image_Data[H][W]>Image_Data[H][W-1]||Lchange)
            {   
                Lchange=1;                                              //有白到黑的跳变
                if(Lchange==1&&Image_Data[H][W-1]==BLACK_VAL&&Image_Data[H][W-2]==BLACK_VAL)
                    Lbarrier_Weight++;                                  //障碍物宽度
                else
                {
                     Lbarrier_end=W;
                     Lbarrier_H=H;
                     Lchange=0;
                     break;
                } 
            }
        }
        Lbarrierstart=Lbarrier_end+Lbarrier_Weight;                     //障碍物靠近赛道中心的一端
       /****************************/
        
       /********识别右障碍物********/
       Rbarrier_Weight=0;
       Rchange=0;                                                       //在弯道,可能上一行Rchange=1;
       for(W=HBoundM[H];W<HBoundR[H];W++)                               //检测左障碍物
       {
            if (Image_Data[H][W]>Image_Data[H][W+1]||Rchange)
            {   
                Rchange=1;                                              //有白到黑的跳变
                if(Rchange==1&&Image_Data[H][W+1]==BLACK_VAL&&Image_Data[H][W+2]==BLACK_VAL)
                    Rbarrier_Weight++;                                  //障碍物宽度
                else
                {
                     Rbarrier_end=W;
                     Rbarrier_H=H;
                     Rchange=0;
                     break;
                } 
            }
            if(HBoundM[H]-HBoundM[H-1]>10)                             //由于赛道环境限制，在圆环那里，周围的赛道太靠近圆环，会误识别为障碍物，所以这个if语句用来排除周围赛道的干扰
            {
                no=0;                                                  //代表不是障碍物，而是周围赛道干扰
            }
       }
       Rbarrierstart=Rbarrier_end-Rbarrier_Weight;                     //障碍物靠近赛道中心的一端
       /***************************/
       
       if(Lbarrier_Weight>20&&HBoundM[Lbarrier_H]!=BLACK_VAL)          //排除远处干扰  ：HBoundM[Rbarrier_H]!=BLACK_VAL                                 
       {
            Lbarrier=1;
            break;
       }
       if(Rbarrier_Weight>20&&HBoundM[Rbarrier_H]!=BLACK_VAL)          //Lbarrier_Weight设为10时，起跑线会当成障碍物，所以改为20
       {
            Rbarrier=1;
            break;
       }
    }
    
    /*******************障碍物补线********************/
    if(Lbarrier==1&&no&&(Lbarrier_H>H_Min))                            //(Lbarrier_H>H_Min&&(H_Min!=H_START)):排除弯道干扰，急弯时，会误识别为障碍物
       {
          for(H=Lbarrier_H;H<H_END;H++)
          {
              HBoundL[H]=Lbarrierstart-(H-Lbarrier_H)*Lbarrierstart/(H_END-Lbarrier_H)+10;         //障碍物补线
          }
          for(H=Lbarrier_H-1;H>=H_START;H--)
          {
              HBoundL[H]=HBoundL[H+1];
          }
       }
       
       if(Rbarrier==1&&no&&(Rbarrier_H>H_Min))
       {
          for(H=Rbarrier_H;H<H_END;H++)
          {
              HBoundR[H]=Rbarrierstart+(H-Rbarrier_H)*(160-Rbarrierstart)/(H_END-Rbarrier_H)-10;
          }
          for(H=Rbarrier_H-1;H>=H_START;H--)
          {
              HBoundR[H]=HBoundR[H+1];
          }
       }
    
      for(H=H_END;H>=H_START;H--)                                     //障碍物中线规划
      {
           
           HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
          
          
      }  
    /**********************************************/
     
    /*for(H=int_min(H_MaxL,H_MaxR);H<=H_END;H++)
    {
         HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
    }*/
    return S;                                                
    
}


int Graph_FindMidLine2()                         //此函数用于模式1
{
  
    /*********障碍物变量*******/                 // L代表左边,R代表右边
    int Lbarrier_Weight=0;                      //左障碍物的宽度
    int Lbarrier_end=0;
    int Lbarrier_H=0;                           //障碍物最近一行的行号
    int Lbarrierstart=0;
    int Rbarrier_Weight=0;
    int Rbarrier_end=0;                         //障碍物靠近赛道中心的一端
    int Rbarrier_H=0;
    int Rbarrierstart=0;
    int Lbarrier=0;                             //标记障碍物的有无
    int Rbarrier=0;
    int Lchange=0;                              //记录跳变
    int Rchange=0;
    int no=1;                                   //用来排除圆环附近赛道的干扰，以防误识别为障碍物
    int White_Black=0;
    /**************************/
      
    /********找边线变量********/
    int H,W;
    int findLline=0;                            //找到左线标志           
    int findRline=0;                            //找到右线标志
    imINT16  BoundL[CAMERA_H];
    imINT16  BoundR[CAMERA_H];
    int dDir0,dDir1;                            //十字路变量
    int GetLineCntL=0;
    int GetLineCntR=0;
    int Mid;                                    //每次更新的中线
    int H1,H2,H3,H4,H5,H6;
    /**************************/
      
    /*********圆环变量*********/  
//    int num=0;                                 //用于初步判断是否是圆环                                 
    int H_circle=0;                            //代表圆环近端的H   
    int countL=0,countR=0;
    int direction=0;                          //车行驶方向控制： 0:圆环左转   1:圆环右转
    int pixel_change1=0;
    int lastL=0,nowL=0,lastR=0,nowR=0;
    int no_getLineL=0,no_getLineR=0;
    /**************************/
      
    /********起始线变量********/
    int startline=0;                          //用来判断是否是起跑线
    /**************************/
    
    /**********S路变量*********/
    int White_pixel=0;                          //图像中心白像素点个数
    int sum;                                    //60-120行的中线之和
    int S_Dir;                                  //60-120行的偏差均值
    /**************************/
    
    H_Min = H_START;
    
    
    /*******判断是否出界*******/
    if(Graph_JudgeOut()&&(!(pixel_change>6)))      
        return 0;
    else
    {
        for (H=0;H<=CAMERA_H;H++)
        {
            HBoundL[H]=MaxStart;
            HBoundR[H]=MaxEnd;
            HBoundM[H]=(MaxStart+MaxEnd)/2;
        }
         // H_Min = H_START;                      //这个语句没作用，只是为了程序的正确执行，因为如果else里没有语句，遇见圆环会死机
    }
    /**************************/
    
    dDir0=0;//根据偏差算十字拐角
    dDir1=0;//根据偏差算十字拐角
    
    num=0;
    int point_A=0,point_B=0,point_C=0,point_D=0;        //圆环 上 下 左 右 四个点
    int center=0;                                       //圆环中心
    DisBuzzer();
    Circle=0;
    for(H=110;H>30;H--)                          
    {
       if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL&&H_circle==0)           //记录图像中心线有没有跳变
       {    
           num=1;
           H_circle=120-(H-1);
           //break;
       }
       if(H>10&&H<60)
       {
           if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL&&point_A==0)
           {
              point_A=H-1;
              //EnBuzzer();
           }
           if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H+1][W_MID]==BLACK_VAL&&point_B==0)
           {
              point_B=H+1;
              //EnBuzzer();
           }
           if(point_A!=0&&point_B!=0)
              break;
       }
    }
    if(point_A-point_B>15&&(point_A>25&&point_B<25))
    {
        center=(point_A+point_B)/2;
    }
        
//        for(W=W_MID;W>H_START;H--)
//        {
//            if(Image_Data[center][W-1]==WHITE_VAL&&Image_Data[center][W]==BLACK_VAL)
//            {
//                point_C=W;
//                break;
//            }
//        }
//        
//        for(W=W_MID;W<H_END;H++)
//        {
//            if(Image_Data[center][W]==WHITE_VAL&&Image_Data[center][W-1]==BLACK_VAL)
//            {
//                point_D=W-1;
//                break;
//            }
//        }
  //  }
//    if(point_D-point_C>60)
//    {
//        EnBuzzer();
//    }
    
    S=0;
    //DisBuzzer();
    for(H=H_END;H>=H_START;H--)
    {   
        /******S路识别*******/
        if(Image_Data[H][79]==WHITE_VAL)
            White_pixel++;
        /********************/
        
        H1=int_min(H+1,H_END);
        H2=int_min(H+2,H_END);
        H3=int_min(H+3,H_END);
        H4=int_min(H+4,H_END);
        H5=int_min(H+5,H_END);
        H6=int_min(H+6,H_END);
        Mid=HBoundM[H+1];
        
        if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
        {
             Mid = W_START+(W_END>>2);
             if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
             {
                  Mid = W_END-(W_END>>2);
                  if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
                  {
                        Mid = W_START+(W_END>>4);
                        if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
                        {
                             Mid = W_END-(W_END>>4);
                              if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
                              {
                                   HBoundL[H] = HBoundL[H+1] ;
                                   HBoundR[H] = HBoundR[H+1] ;
                                   HBoundM[H] = HBoundM[H+1] ;
                                    continue;
                              }
                        }

                  }
             }
        }
        
        /***************找左线***************/
        for(W=W_START;W<Mid;W++)
        {
            if (Image_Data[H][W]<Image_Data[H][W+1])
            {

                    HBoundL[H] = W;
//                    if(int_abs(HBoundL[H]-HBoundL[H1])>5)
//                        HBoundL[H]=HBoundL[H1];
                    GetLineCntL++;
                    break;
            }
        }
        if(W==Mid)
        {
            lastL=nowL;
            nowL=1;
            if(((H>121-H_circle)&&(H<121-H_circle+20))&&(lastL==1&&nowL==1)&&num==1)
                no_getLineL++;
                
            HBoundL[H] =HBoundL[H1] ;
        }
        
        /***********************************/
        
        
        /***************找右线***************/
        for(W=W_END;W>Mid;W--)
        {
            if (Image_Data[H][W]<Image_Data[H][W-1])
            {

                    HBoundR[H] = W;
//                    if(int_abs(HBoundR[H]-HBoundR[H1])>5)
//                        HBoundR[H]=HBoundR[H1];
                    GetLineCntR++;
                    break;
            }
        }
        if(W==Mid)
        {
            lastR=nowR;
            nowR=1;
            if(((H>121-H_circle)&&(H<121-H_circle+20))&&(lastR==1&&nowR==1)&&num==1)
                no_getLineR++;
          
            HBoundR[H] =HBoundR[H1] ;
        }
        
        if(Image_Data[center][W_MID]==BLACK_VAL&&Image_Data[center][W_MID-1]==BLACK_VAL&&Image_Data[center][W_MID+1]==BLACK_VAL&&Image_Data[center][W_MID-5]==BLACK_VAL&&Image_Data[center][W_MID+5]==BLACK_VAL&&Image_Data[center][W_MID+15]==BLACK_VAL&&Image_Data[center][W_MID-15]==BLACK_VAL&&Image_Data[center][25]==WHITE_VAL&&Image_Data[center][135]==WHITE_VAL&&(no_getLineR>10)&&(no_getLineL>10))
        {
            EnBuzzer();
            Circle=1;
        }
        /***********************************/     
        
        
        /**************十字路***************/ 
        dDir0=HBoundR[H]-HBoundR[H2];
        dDir1=HBoundR[H2]-HBoundR[H6];
        
        if((dDir0*dDir1<-4 && dDir0>0)/*&&(!startline) */|| ((dDir0 > 10 && dDir1<=0)/*&&(!startline)*/))             //(!startline):在起跑线处会误识别为十字路，所以加上(!startline)
        {
            //十字路补右线
            HBoundR[H]=HBoundR[H3]+HBoundR[H3]-HBoundR[H6];
            HBoundR[H1]=HBoundR[H4]+HBoundR[H3]-HBoundR[H6];
        }
        
        dDir0=HBoundL[H]-HBoundL[H2];
        dDir1=HBoundL[H2]-HBoundL[H6];
        
        
        if((dDir0*dDir1<-4 && dDir0<0)/*&&(!startline)*/ || ((dDir0 <-10  && dDir1>=0)/*&&(!startline)*/))            
        {
            //十字路补左线
            HBoundL[H]=HBoundL[H3]+HBoundL[H3]-HBoundL[H6];
            HBoundL[H1]=HBoundL[H4]+HBoundL[H3]-HBoundL[H6];
        }
       
        HBoundM[H]= (HBoundR[H]+ HBoundL[H])/2;
        if( HBoundR[H] < W_START+LINE_WIDTH || HBoundL[H] > W_END-LINE_WIDTH || (HBoundR[H]-HBoundL[H]) <LINE_WIDTH )
        {
             H_Min = H;
             break;
        }
        /***********************************/ 

    }
    
    /******S路识别*******/
    sum=0;
    for(H=H_END;H>60;H--)
    {
        sum+=HBoundM[H];
    }
    S_Dir=sum/(H_END-60)-79;
    
    if(White_pixel>100&&((S_Dir<25&&S_Dir>1)||(S_Dir>-25&&S_Dir<-1))&&(int_abs((HBoundM[60]+HBoundM[H_END])/2-HBoundM[90])>5))
    {
        S=1;                    //S路标志位
        //EnBuzzer();             //蜂鸣器响
    }
    /********************/
    
        
     /********************圆环*********************/
     
    int countbl=0,countbr=0;
     for(W=W_START;W<W_END;W++)
     {
         if((Image_Data[121-H_circle-2][W]>Image_Data[121-H_circle-2][W+1]||Image_Data[121-H_circle-2][W]<Image_Data[121-H_circle-2][W+1])&&(W+1<=W_END))  //白变黑或者黑跳白
        {
            pixel_change1++;                              //像素点突变次数
        }
     }
    
     if(num==1&&(pixel_change1<6))
     {
         for(H=120-H_circle;H<=H_END;H++)
         {
              if(Image_Data[H][0]==WHITE_VAL)
              {
                   countL++;                                   //woxiedelalalallallalla
                if(countL>=H_circle-30)
                 break;
              }
           else    
           {   countbl++;
              if(countbl>4)
              {
                countL=0;
                countbl=0;
                break;
              }
           }
         }
         
         for(H=H_END;H>=120-H_circle;H--)
         {
              if(Image_Data[H][159]==WHITE_VAL)
              {
                   countR++;                                   //woxiedelalalallallalla
                if(countR>=H_circle-30)
                 break;
              }
             else            
             {
               countbr++;
               if(countbr>4)
               {
               countR=0;
               countbr=0;
                 break;
               }
             }
         }
     }
    //Circle=0;
//    int a,b,c,d;
//     for(H=H_END;H>0;H--)                          
//     {
//      
//        if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)           //记录图像中心线有没有跳变
//        { 
//          a=H-1;
//          break;
//        }
//     }
//    for(H=a;H>0;H--)
//    {
//       if(Image_Data[H][W_MID]==BLACK_VAL&&Image_Data[H-1][W_MID]==WHITE_VAL)
//       {
//         b=H;
//       break;
//       }
//    }
//      for(H=b;H>0;H--)
//    {
//         if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)
//       {
//         d=H;
//       break;
//       }
//    }
//    c=a-b;
    //if((countL>=H_circle-30&&(countR>=H_circle-30)/*&&Image_Data[120-H_circle-3][159]==WHITE_VAL*/&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))||((d>5)&&(c>40)&&(countL>=30)&&(countR>=30)&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))/*&&(!startline)*/)       //H_circle&&Image_Data[35][40]!=WHITE_VAL:在十字路处如果车身比较斜，十字路会与圆环出口差不多，所以加上这个这个条件
     if(Circle==1||((H>30&&H<=80)&&(countL>=H_circle-30)&&(countR>=H_circle-30)&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))||((H>30&&H<=110)&&(countL>=H_circle-5)&&(countR>=H_circle-5)&&H_circle&&Image_Data[35][40]!=WHITE_VAL)||(no_getLineL>10&&no_getLineR>10&&Image_Data[35][40]!=WHITE_VAL))       //H_circle&&Image_Data[35][40]!=WHITE_VAL:在十字路处如果车身比较斜，十字路会与圆环出口差不多，所以加上这个这个条件
     {   
       Circle=1;
       if(direction)
       {
           for(H=H_END;H>=H_START;H--)                                     //圆环补右线，即圆环左转
           {
               HBoundR[H]=160-(H-(120-H_circle))*160/(double)(H_circle);
               HBoundL[H]=159;
               if(H<(110-H_circle))
               {
                   HBoundR[H]=159;
                 
               }
               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
           }
       }
         
       else 
           for(H=H_END;H>=H_START;H--)                                     //圆环补左线
           {
               HBoundL[H]=(H-(120-H_circle))*160/(double)(H_circle);
               HBoundR[H]=0;
               if(H<(110-H_circle))
               {
                   HBoundL[H]=0;
                   
               }
               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
           }
     }
     /*********************************************/
     
     /*******************障碍物********************/
    for(H=100;H>20;H--)                                 //50                    开始是100到40
    {
        /********识别左障碍物*******/
        Lbarrier_Weight=0;
        Lchange=0;                                                       //在弯道,可能上一行Rchange=1;
        for(W=HBoundM[H];W>HBoundL[H];W--)                              //检测左障碍物
        {       
            if (Image_Data[H][W]>Image_Data[H][W-1]||Lchange)
            {   
                Lchange=1;                                              //有白到黑的跳变
                if(Lchange==1&&Image_Data[H][W-1]==BLACK_VAL&&Image_Data[H][W-2]==BLACK_VAL)
                    Lbarrier_Weight++;                                  //障碍物宽度
                else
                {
                     Lbarrier_end=W;
                     Lbarrier_H=H;
                     Lchange=0;
                     break;
                } 
            }
        }
        Lbarrierstart=Lbarrier_end+Lbarrier_Weight;                     //障碍物靠近赛道中心的一端
       /****************************/
        
       /********识别右障碍物********/
       Rbarrier_Weight=0;
       Rchange=0;                                                       //在弯道,可能上一行Rchange=1;
       for(W=HBoundM[H];W<HBoundR[H];W++)                               //检测左障碍物
       {
            if (Image_Data[H][W]>Image_Data[H][W+1]||Rchange)
            {   
                Rchange=1;                                              //有白到黑的跳变
                if(Rchange==1&&Image_Data[H][W+1]==BLACK_VAL&&Image_Data[H][W+2]==BLACK_VAL)
                    Rbarrier_Weight++;                                  //障碍物宽度
                else
                {
                     Rbarrier_end=W;
                     Rbarrier_H=H;
                     Rchange=0;
                     break;
                } 
            }
            if(HBoundM[H]-HBoundM[H-1]>10)                             //由于赛道环境限制，在圆环那里，周围的赛道太靠近圆环，会误识别为障碍物，所以这个if语句用来排除周围赛道的干扰
            {
                no=0;                                                  //代表不是障碍物，而是周围赛道干扰
            }
       }
       Rbarrierstart=Rbarrier_end-Rbarrier_Weight;                     //障碍物靠近赛道中心的一端
       /***************************/
       
       if(Lbarrier_Weight>20&&HBoundM[Lbarrier_H]!=BLACK_VAL)          //排除远处干扰  ：HBoundM[Rbarrier_H]!=BLACK_VAL                                 
       {
            Lbarrier=1;
            break;
       }
       if(Rbarrier_Weight>20&&HBoundM[Rbarrier_H]!=BLACK_VAL)          //Lbarrier_Weight设为10时，起跑线会当成障碍物，所以改为20
       {
            Rbarrier=1;
            break;
       }
    }
    
    /*******************障碍物补线********************/
    if(Lbarrier==1&&no&&(Lbarrier_H>H_Min))                            //(Lbarrier_H>H_Min&&(H_Min!=H_START)):排除弯道干扰，急弯时，会误识别为障碍物
       {
          for(H=Lbarrier_H;H<H_END;H++)
          {
              HBoundL[H]=Lbarrierstart-(H-Lbarrier_H)*Lbarrierstart/(H_END-Lbarrier_H)+10;         //障碍物补线
          }
          for(H=Lbarrier_H-1;H>=H_START;H--)
          {
              HBoundL[H]=HBoundL[H+1];
          }
       }
       
       if(Rbarrier==1&&no&&(Rbarrier_H>H_Min))
       {
          for(H=Rbarrier_H;H<H_END;H++)
          {
              HBoundR[H]=Rbarrierstart+(H-Rbarrier_H)*(160-Rbarrierstart)/(H_END-Rbarrier_H)-10;
          }
          for(H=Rbarrier_H-1;H>=H_START;H--)
          {
              HBoundR[H]=HBoundR[H+1];
          }
       }
    
      for(H=H_END;H>=H_START;H--)                                     //障碍物中线规划
      {
           
           HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
          
          
      }  
    /**********************************************/
     
    /*for(H=int_min(H_MaxL,H_MaxR);H<=H_END;H++)
    {
         HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
    }*/
    return S;                                                
    
}


int Graph_FindMidLine3()                         //此函数用于模式1
{
  
    /*********障碍物变量*******/                 // L代表左边,R代表右边
    int Lbarrier_Weight=0;                      //左障碍物的宽度
    int Lbarrier_end=0;
    int Lbarrier_H=0;                           //障碍物最近一行的行号
    int Lbarrierstart=0;
    int Rbarrier_Weight=0;
    int Rbarrier_end=0;                         //障碍物靠近赛道中心的一端
    int Rbarrier_H=0;
    int Rbarrierstart=0;
    int Lbarrier=0;                             //标记障碍物的有无
    int Rbarrier=0;
    int Lchange=0;                              //记录跳变
    int Rchange=0;
    int no=1;                                   //用来排除圆环附近赛道的干扰，以防误识别为障碍物
    int White_Black=0;
    /**************************/
      
    /********找边线变量********/
    int H,W;
    int findLline=0;                            //找到左线标志           
    int findRline=0;                            //找到右线标志
    imINT16  BoundL[CAMERA_H];
    imINT16  BoundR[CAMERA_H];
    int dDir0,dDir1;                            //十字路变量
    int GetLineCntL=0;
    int GetLineCntR=0;
    int Mid;                                    //每次更新的中线
    int H1,H2,H3,H4,H5,H6;
    /**************************/
      
    /*********圆环变量*********/  
//    int num=0;                                 //用于初步判断是否是圆环                                 
    int H_circle=0;                            //代表圆环近端的H   
    int countL=0,countR=0;
    int direction=1;                          //车行驶方向控制： 0:圆环左转   1:圆环右转
    int pixel_change1=0;
    int lastL=0,nowL=0,lastR=0,nowR=0;
    int no_getLineL=0,no_getLineR=0;
    /**************************/
      
    /********起始线变量********/
    int startline=0;                          //用来判断是否是起跑线
    /**************************/
    
    /**********S路变量*********/
    int White_pixel=0;                          //图像中心白像素点个数
    int sum;                                    //60-120行的中线之和
    int S_Dir;                                  //60-120行的偏差均值
    /**************************/
    
    H_Min = H_START;
    
    
    /*******判断是否出界*******/
    if(Graph_JudgeOut()&&(!(pixel_change>6)))      
        return 0;
    else
    {
        for (H=0;H<=CAMERA_H;H++)
        {
            HBoundL[H]=MaxStart;
            HBoundR[H]=MaxEnd;
            HBoundM[H]=(MaxStart+MaxEnd)/2;
        }
         // H_Min = H_START;                      //这个语句没作用，只是为了程序的正确执行，因为如果else里没有语句，遇见圆环会死机
    }
    /**************************/
    
    dDir0=0;//根据偏差算十字拐角
    dDir1=0;//根据偏差算十字拐角
    
    num=0;
    int point_A=0,point_B=0,point_C=0,point_D=0;        //圆环 上 下 左 右 四个点
    int center=0;                                       //圆环中心
    DisBuzzer();
    Circle=0;
    for(H=110;H>30;H--)                          
    {
       if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL&&H_circle==0)           //记录图像中心线有没有跳变
       {    
           num=1;
           H_circle=120-(H-1);
           //break;
       }
       if(H>10&&H<60)
       {
           if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL&&point_A==0)
           {
              point_A=H-1;
              //EnBuzzer();
           }
           if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H+1][W_MID]==BLACK_VAL&&point_B==0)
           {
              point_B=H+1;
              //EnBuzzer();
           }
           if(point_A!=0&&point_B!=0)
              break;
       }
    }
    if(point_A-point_B>15&&(point_A>25&&point_B<25))
    {
        center=(point_A+point_B)/2;
    }
        
//        for(W=W_MID;W>H_START;H--)
//        {
//            if(Image_Data[center][W-1]==WHITE_VAL&&Image_Data[center][W]==BLACK_VAL)
//            {
//                point_C=W;
//                break;
//            }
//        }
//        
//        for(W=W_MID;W<H_END;H++)
//        {
//            if(Image_Data[center][W]==WHITE_VAL&&Image_Data[center][W-1]==BLACK_VAL)
//            {
//                point_D=W-1;
//                break;
//            }
//        }
  //  }
//    if(point_D-point_C>60)
//    {
//        EnBuzzer();
//    }
    
    S=0;
    //DisBuzzer();
    for(H=H_END;H>=H_START;H--)
    {   
        /******S路识别*******/
        if(Image_Data[H][79]==WHITE_VAL)
            White_pixel++;
        /********************/
        
        H1=int_min(H+1,H_END);
        H2=int_min(H+2,H_END);
        H3=int_min(H+3,H_END);
        H4=int_min(H+4,H_END);
        H5=int_min(H+5,H_END);
        H6=int_min(H+6,H_END);
        Mid=HBoundM[H+1];
        
        if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
        {
             Mid = W_START+(W_END>>2);
             if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
             {
                  Mid = W_END-(W_END>>2);
                  if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
                  {
                        Mid = W_START+(W_END>>4);
                        if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
                        {
                             Mid = W_END-(W_END>>4);
                              if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
                              {
                                   HBoundL[H] = HBoundL[H+1] ;
                                   HBoundR[H] = HBoundR[H+1] ;
                                   HBoundM[H] = HBoundM[H+1] ;
                                    continue;
                              }
                        }

                  }
             }
        }
        
        /***************找左线***************/
        for(W=W_START;W<Mid;W++)
        {
            if (Image_Data[H][W]<Image_Data[H][W+1])
            {

                    HBoundL[H] = W;
//                    if(int_abs(HBoundL[H]-HBoundL[H1])>5)
//                        HBoundL[H]=HBoundL[H1];
                    GetLineCntL++;
                    break;
            }
        }
        if(W==Mid)
        {
            lastL=nowL;
            nowL=1;
            if(((H>121-H_circle)&&(H<121-H_circle+20))&&(lastL==1&&nowL==1)&&num==1)
                no_getLineL++;
                
            HBoundL[H] =HBoundL[H1] ;
        }
        
        /***********************************/
        
        
        /***************找右线***************/
        for(W=W_END;W>Mid;W--)
        {
            if (Image_Data[H][W]<Image_Data[H][W-1])
            {

                    HBoundR[H] = W;
//                    if(int_abs(HBoundR[H]-HBoundR[H1])>5)
//                        HBoundR[H]=HBoundR[H1];
                    GetLineCntR++;
                    break;
            }
        }
        if(W==Mid)
        {
            lastR=nowR;
            nowR=1;
            if(((H>121-H_circle)&&(H<121-H_circle+20))&&(lastR==1&&nowR==1)&&num==1)
                no_getLineR++;
          
            HBoundR[H] =HBoundR[H1] ;
        }
        
        if(Image_Data[center][W_MID]==BLACK_VAL&&Image_Data[center][W_MID-1]==BLACK_VAL&&Image_Data[center][W_MID+1]==BLACK_VAL&&Image_Data[center][W_MID-5]==BLACK_VAL&&Image_Data[center][W_MID+5]==BLACK_VAL&&Image_Data[center][W_MID+15]==BLACK_VAL&&Image_Data[center][W_MID-15]==BLACK_VAL&&Image_Data[center][25]==WHITE_VAL&&Image_Data[center][135]==WHITE_VAL&&(no_getLineR>10)&&(no_getLineL>10))
        {
            EnBuzzer();
            Circle=1;
        }
        /***********************************/     
        
        
        /**************十字路***************/ 
        dDir0=HBoundR[H]-HBoundR[H2];
        dDir1=HBoundR[H2]-HBoundR[H6];
        
        if((dDir0*dDir1<-4 && dDir0>0)/*&&(!startline) */|| ((dDir0 > 10 && dDir1<=0)/*&&(!startline)*/))             //(!startline):在起跑线处会误识别为十字路，所以加上(!startline)
        {
            //十字路补右线
            HBoundR[H]=HBoundR[H3]+HBoundR[H3]-HBoundR[H6];
            HBoundR[H1]=HBoundR[H4]+HBoundR[H3]-HBoundR[H6];
        }
        
        dDir0=HBoundL[H]-HBoundL[H2];
        dDir1=HBoundL[H2]-HBoundL[H6];
        
        
        if((dDir0*dDir1<-4 && dDir0<0)/*&&(!startline)*/ || ((dDir0 <-10  && dDir1>=0)/*&&(!startline)*/))            
        {
            //十字路补左线
            HBoundL[H]=HBoundL[H3]+HBoundL[H3]-HBoundL[H6];
            HBoundL[H1]=HBoundL[H4]+HBoundL[H3]-HBoundL[H6];
        }
       
        HBoundM[H]= (HBoundR[H]+ HBoundL[H])/2;
        if( HBoundR[H] < W_START+LINE_WIDTH || HBoundL[H] > W_END-LINE_WIDTH || (HBoundR[H]-HBoundL[H]) <LINE_WIDTH )
        {
             H_Min = H;
             break;
        }
        /***********************************/ 

    }
    
    /******S路识别*******/
    sum=0;
    for(H=H_END;H>60;H--)
    {
        sum+=HBoundM[H];
    }
    S_Dir=sum/(H_END-60)-79;
    
    if(White_pixel>100&&((S_Dir<25&&S_Dir>1)||(S_Dir>-25&&S_Dir<-1))&&(int_abs((HBoundM[60]+HBoundM[H_END])/2-HBoundM[90])>5))
    {
        S=1;                    //S路标志位
        //EnBuzzer();             //蜂鸣器响
    }
    /********************/
    
        
     /********************圆环*********************/
     
    int countbl=0,countbr=0;
     for(W=W_START;W<W_END;W++)
     {
         if((Image_Data[121-H_circle-2][W]>Image_Data[121-H_circle-2][W+1]||Image_Data[121-H_circle-2][W]<Image_Data[121-H_circle-2][W+1])&&(W+1<=W_END))  //白变黑或者黑跳白
        {
            pixel_change1++;                              //像素点突变次数
        }
     }
    
     if(num==1&&(pixel_change1<6))
     {
         for(H=120-H_circle;H<=H_END;H++)
         {
              if(Image_Data[H][0]==WHITE_VAL)
              {
                   countL++;                                   //woxiedelalalallallalla
                if(countL>=H_circle-30)
                 break;
              }
           else    
           {   countbl++;
              if(countbl>4)
              {
                countL=0;
                countbl=0;
                break;
              }
           }
         }
         
         for(H=H_END;H>=120-H_circle;H--)
         {
              if(Image_Data[H][159]==WHITE_VAL)
              {
                   countR++;                                   //woxiedelalalallallalla
                if(countR>=H_circle-30)
                 break;
              }
             else            
             {
               countbr++;
               if(countbr>4)
               {
               countR=0;
               countbr=0;
                 break;
               }
             }
         }
     }
    //Circle=0;
//    int a,b,c,d;
//     for(H=H_END;H>0;H--)                          
//     {
//      
//        if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)           //记录图像中心线有没有跳变
//        { 
//          a=H-1;
//          break;
//        }
//     }
//    for(H=a;H>0;H--)
//    {
//       if(Image_Data[H][W_MID]==BLACK_VAL&&Image_Data[H-1][W_MID]==WHITE_VAL)
//       {
//         b=H;
//       break;
//       }
//    }
//      for(H=b;H>0;H--)
//    {
//         if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)
//       {
//         d=H;
//       break;
//       }
//    }
//    c=a-b;
    //if((countL>=H_circle-30&&(countR>=H_circle-30)/*&&Image_Data[120-H_circle-3][159]==WHITE_VAL*/&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))||((d>5)&&(c>40)&&(countL>=30)&&(countR>=30)&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))/*&&(!startline)*/)       //H_circle&&Image_Data[35][40]!=WHITE_VAL:在十字路处如果车身比较斜，十字路会与圆环出口差不多，所以加上这个这个条件
     if(Circle==1||((H>30&&H<=80)&&(countL>=H_circle-30)&&(countR>=H_circle-30)&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))||((H>30&&H<=110)&&(countL>=H_circle-5)&&(countR>=H_circle-5)&&H_circle&&Image_Data[35][40]!=WHITE_VAL)||(no_getLineL>10&&no_getLineR>10&&Image_Data[35][40]!=WHITE_VAL))       //H_circle&&Image_Data[35][40]!=WHITE_VAL:在十字路处如果车身比较斜，十字路会与圆环出口差不多，所以加上这个这个条件
     {   
       Circle=1;
       if(direction)
       {
           for(H=H_END;H>=H_START;H--)                                     //圆环补右线，即圆环左转
           {
               HBoundR[H]=160-(H-(120-H_circle))*160/(double)(H_circle);
               HBoundL[H]=159;
               if(H<(110-H_circle))
               {
                   HBoundR[H]=159;
                 
               }
               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
           }
       }
         
       else 
           for(H=H_END;H>=H_START;H--)                                     //圆环补左线
           {
               HBoundL[H]=(H-(120-H_circle))*160/(double)(H_circle);
               HBoundR[H]=0;
               if(H<(110-H_circle))
               {
                   HBoundL[H]=0;
                   
               }
               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
           }
     }
     /*********************************************/
     
     /*******************障碍物********************/
    for(H=100;H>20;H--)                                 //50                    开始是100到40
    {
        /********识别左障碍物*******/
        Lbarrier_Weight=0;
        Lchange=0;                                                       //在弯道,可能上一行Rchange=1;
        for(W=HBoundM[H];W>HBoundL[H];W--)                              //检测左障碍物
        {       
            if (Image_Data[H][W]>Image_Data[H][W-1]||Lchange)
            {   
                Lchange=1;                                              //有白到黑的跳变
                if(Lchange==1&&Image_Data[H][W-1]==BLACK_VAL&&Image_Data[H][W-2]==BLACK_VAL)
                    Lbarrier_Weight++;                                  //障碍物宽度
                else
                {
                     Lbarrier_end=W;
                     Lbarrier_H=H;
                     Lchange=0;
                     break;
                } 
            }
        }
        Lbarrierstart=Lbarrier_end+Lbarrier_Weight;                     //障碍物靠近赛道中心的一端
       /****************************/
        
       /********识别右障碍物********/
       Rbarrier_Weight=0;
       Rchange=0;                                                       //在弯道,可能上一行Rchange=1;
       for(W=HBoundM[H];W<HBoundR[H];W++)                               //检测左障碍物
       {
            if (Image_Data[H][W]>Image_Data[H][W+1]||Rchange)
            {   
                Rchange=1;                                              //有白到黑的跳变
                if(Rchange==1&&Image_Data[H][W+1]==BLACK_VAL&&Image_Data[H][W+2]==BLACK_VAL)
                    Rbarrier_Weight++;                                  //障碍物宽度
                else
                {
                     Rbarrier_end=W;
                     Rbarrier_H=H;
                     Rchange=0;
                     break;
                } 
            }
            if(HBoundM[H]-HBoundM[H-1]>10)                             //由于赛道环境限制，在圆环那里，周围的赛道太靠近圆环，会误识别为障碍物，所以这个if语句用来排除周围赛道的干扰
            {
                no=0;                                                  //代表不是障碍物，而是周围赛道干扰
            }
       }
       Rbarrierstart=Rbarrier_end-Rbarrier_Weight;                     //障碍物靠近赛道中心的一端
       /***************************/
       
       if(Lbarrier_Weight>20&&HBoundM[Lbarrier_H]!=BLACK_VAL)          //排除远处干扰  ：HBoundM[Rbarrier_H]!=BLACK_VAL                                 
       {
            Lbarrier=1;
            break;
       }
       if(Rbarrier_Weight>20&&HBoundM[Rbarrier_H]!=BLACK_VAL)          //Lbarrier_Weight设为10时，起跑线会当成障碍物，所以改为20
       {
            Rbarrier=1;
            break;
       }
    }
    
    /*******************障碍物补线********************/
    if(Lbarrier==1&&no&&(Lbarrier_H>H_Min))                            //(Lbarrier_H>H_Min&&(H_Min!=H_START)):排除弯道干扰，急弯时，会误识别为障碍物
       {
          for(H=Lbarrier_H;H<H_END;H++)
          {
              HBoundL[H]=Lbarrierstart-(H-Lbarrier_H)*Lbarrierstart/(H_END-Lbarrier_H)+10;         //障碍物补线
          }
          for(H=Lbarrier_H-1;H>=H_START;H--)
          {
              HBoundL[H]=HBoundL[H+1];
          }
       }
       
       if(Rbarrier==1&&no&&(Rbarrier_H>H_Min))
       {
          for(H=Rbarrier_H;H<H_END;H++)
          {
              HBoundR[H]=Rbarrierstart+(H-Rbarrier_H)*(160-Rbarrierstart)/(H_END-Rbarrier_H)-10;
          }
          for(H=Rbarrier_H-1;H>=H_START;H--)
          {
              HBoundR[H]=HBoundR[H+1];
          }
       }
    
      for(H=H_END;H>=H_START;H--)                                     //障碍物中线规划
      {
           
           HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
          
          
      }  
    /**********************************************/
     
    /*for(H=int_min(H_MaxL,H_MaxR);H<=H_END;H++)
    {
         HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
    }*/
    return S;                                                
    
}


//int Graph_FindMidLine1()                         //此函数用于模式1
//{
//  
//    /*********障碍物变量*******/                 // L代表左边,R代表右边
//    int Lbarrier_Weight=0;                      //左障碍物的宽度
//    int Lbarrier_end=0;
//    int Lbarrier_H=0;                           //障碍物最近一行的行号
//    int Lbarrierstart=0;
//    int Rbarrier_Weight=0;
//    int Rbarrier_end=0;                         //障碍物靠近赛道中心的一端
//    int Rbarrier_H=0;
//    int Rbarrierstart=0;
//    int Lbarrier=0;                             //标记障碍物的有无
//    int Rbarrier=0;
//    int Lchange=0;                              //记录跳变
//    int Rchange=0;
//    int no=1;                                   //用来排除圆环附近赛道的干扰，以防误识别为障碍物
//    int White_Black=0;
//    /**************************/
//      
//    /********找边线变量********/
//    int H,W;
//    int findLline=0;                            //找到左线标志           
//    int findRline=0;                            //找到右线标志
//    imINT16  BoundL[CAMERA_H];
//    imINT16  BoundR[CAMERA_H];
//    int dDir0,dDir1;                            //十字路变量
//    int GetLineCntL=0;
//    int GetLineCntR=0;
//    int Mid;                                    //每次更新的中线
//    int H1,H2,H3,H4,H5,H6;
//    /**************************/
//      
//    /*********圆环变量*********/  
////    int num=0;                                 //用于初步判断是否是圆环                                 
//    int H_circle=0;                            //代表圆环近端的H   
//    int countL=0,countR=0;
//    int direction=0;                          //车行驶方向控制： 0:圆环左转   1:圆环右转
//    int pixel_change1=0;
//    int lastL=0,nowL=0,lastR=0,nowR=0;
//    int no_getLineL=0,no_getLineR=0;
//    /**************************/
//      
//    /********起始线变量********/
//    int startline=0;                          //用来判断是否是起跑线
//    /**************************/
//    
//    /**********S路变量*********/
//    int White_pixel=0;                          //图像中心白像素点个数
//    int sum;                                    //60-120行的中线之和
//    int S_Dir;                                  //60-120行的偏差均值
//    /**************************/
//    
//    H_Min = H_START;
//    
//    
//    /*******判断是否出界*******/
//    if(Graph_JudgeOut()&&(!(pixel_change>6)))      
//        return 0;
//    else
//    {
//        for (H=0;H<=CAMERA_H;H++)
//        {
//            HBoundL[H]=MaxStart;
//            HBoundR[H]=MaxEnd;
//            HBoundM[H]=(MaxStart+MaxEnd)/2;
//        }
//         // H_Min = H_START;                      //这个语句没作用，只是为了程序的正确执行，因为如果else里没有语句，遇见圆环会死机
//    }
//    /**************************/
//    
//    dDir0=0;//根据偏差算十字拐角
//    dDir1=0;//根据偏差算十字拐角
//    
//    num=0;
//    for(H=110;H>30;H--)                          
//    {
//       if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)           //记录图像中心线有没有跳变
//       {    
//           num=1;
//           H_circle=120-(H-1);
//           break;
//       }
//    }
//    
//    S=0;
//    DisBuzzer();
//    for(H=H_END;H>=H_START;H--)
//    {   
//        /******S路识别*******/
//        if(Image_Data[H][79]==WHITE_VAL)
//            White_pixel++;
//        /********************/
//        
//        H1=int_min(H+1,H_END);
//        H2=int_min(H+2,H_END);
//        H3=int_min(H+3,H_END);
//        H4=int_min(H+4,H_END);
//        H5=int_min(H+5,H_END);
//        H6=int_min(H+6,H_END);
//        Mid=HBoundM[H+1];
//        
//        if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//        {
//             Mid = W_START+(W_END>>2);
//             if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//             {
//                  Mid = W_END-(W_END>>2);
//                  if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                  {
//                        Mid = W_START+(W_END>>4);
//                        if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                        {
//                             Mid = W_END-(W_END>>4);
//                              if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                              {
//                                   HBoundL[H] = HBoundL[H+1] ;
//                                   HBoundR[H] = HBoundR[H+1] ;
//                                   HBoundM[H] = HBoundM[H+1] ;
//                                    continue;
//                              }
//                        }
//
//                  }
//             }
//        }
//        
//        /***************找左线***************/
//        for(W=W_START;W<Mid;W++)
//        {
//            if (Image_Data[H][W]<Image_Data[H][W+1])
//            {
//
//                    HBoundL[H] = W;
////                    if(int_abs(HBoundL[H]-HBoundL[H1])>5)
////                        HBoundL[H]=HBoundL[H1];
//                    GetLineCntL++;
//                    break;
//            }
//        }
//        if(W==Mid)
//        {
//            lastL=nowL;
//            nowL=1;
//            if(((H>121-H_circle)&&(H<121-H_circle+20))&&(lastL==1&&nowL==1)&&num==1)
//                no_getLineL++;
//                
//            HBoundL[H] =HBoundL[H1] ;
//        }
//        
//        /***********************************/
//        
//        
//        /***************找右线***************/
//        for(W=W_END;W>Mid;W--)
//        {
//            if (Image_Data[H][W]<Image_Data[H][W-1])
//            {
//
//                    HBoundR[H] = W;
////                    if(int_abs(HBoundR[H]-HBoundR[H1])>5)
////                        HBoundR[H]=HBoundR[H1];
//                    GetLineCntR++;
//                    break;
//            }
//        }
//        if(W==Mid)
//        {
//            lastR=nowR;
//            nowR=1;
//            if(((H>121-H_circle)&&(H<121-H_circle+20))&&(lastR==1&&nowR==1)&&num==1)
//                no_getLineR++;
//          
//            HBoundR[H] =HBoundR[H1] ;
//        }
//        
//        /***********************************/     
//        
//        
//        /**************十字路***************/ 
//        dDir0=HBoundR[H]-HBoundR[H2];
//        dDir1=HBoundR[H2]-HBoundR[H6];
//        
//        if((dDir0*dDir1<-4 && dDir0>0)/*&&(!startline) */|| ((dDir0 > 10 && dDir1<=0)/*&&(!startline)*/))             //(!startline):在起跑线处会误识别为十字路，所以加上(!startline)
//        {
//            //十字路补右线
//            HBoundR[H]=HBoundR[H3]+HBoundR[H3]-HBoundR[H6];
//            HBoundR[H1]=HBoundR[H4]+HBoundR[H3]-HBoundR[H6];
//        }
//        
//        dDir0=HBoundL[H]-HBoundL[H2];
//        dDir1=HBoundL[H2]-HBoundL[H6];
//        
//        
//        if((dDir0*dDir1<-4 && dDir0<0)/*&&(!startline)*/ || ((dDir0 <-10  && dDir1>=0)/*&&(!startline)*/))            
//        {
//            //十字路补左线
//            HBoundL[H]=HBoundL[H3]+HBoundL[H3]-HBoundL[H6];
//            HBoundL[H1]=HBoundL[H4]+HBoundL[H3]-HBoundL[H6];
//        }
//       
//        HBoundM[H]= (HBoundR[H]+ HBoundL[H])/2;
//        if( HBoundR[H] < W_START+LINE_WIDTH || HBoundL[H] > W_END-LINE_WIDTH || (HBoundR[H]-HBoundL[H]) <LINE_WIDTH )
//        {
//             H_Min = H;
//             break;
//        }
//        /***********************************/ 
//
//    }
//    
//    /******S路识别*******/
//    sum=0;
//    for(H=H_END;H>60;H--)
//    {
//        sum+=HBoundM[H];
//    }
//    S_Dir=sum/(H_END-60)-79;
//    
//    if(White_pixel>100&&((S_Dir<25&&S_Dir>1)||(S_Dir>-25&&S_Dir<-1))&&(int_abs((HBoundM[60]+HBoundM[H_END])/2-HBoundM[90])>5))
//    {
//        S=1;                    //S路标志位
//        //EnBuzzer();             //蜂鸣器响
//    }
//    /********************/
//    
//        
//     /********************圆环*********************/
//     
//    
//     for(W=W_START;W<W_END;W++)
//     {
//         if((Image_Data[121-H_circle-2][W]>Image_Data[121-H_circle-2][W+1]||Image_Data[121-H_circle-2][W]<Image_Data[121-H_circle-2][W+1])&&(W+1<=W_END))  //白变黑或者黑跳白
//        {
//            pixel_change1++;                              //像素点突变次数
//        }
//     }
//    
//     if(num==1&&(pixel_change1<6))
//     {
//         for(H=H_END;H>=120-H_circle;H--)
//         {
//              if(Image_Data[H][0]==WHITE_VAL)
//                   countL++;
////              else
////              {
////                   countL==0;
////                   break;
////              }
//         }
//         
//         for(H=H_END;H>=120-H_circle;H--)
//         {
//              if(Image_Data[H][159]==WHITE_VAL)
//                   countR++;
////              else
////              {
////                   countR==0;
////                   break;
////              }
//         }
//     }
//    Circle=0;
//    if(((H>30&&H<=80)&&(countL>=H_circle-30)&&(countR>=H_circle-30)&&H_circle&&Image_Data[35][40]!=WHITE_VAL&&(GetLineCntR<H_circle))||((H>30&&H<=110)&&(countL>=H_circle-5)&&(countR>=H_circle-5)&&H_circle&&Image_Data[35][40]!=WHITE_VAL)||(no_getLineL>10&&no_getLineR>10&&Image_Data[35][40]!=WHITE_VAL))       //H_circle&&Image_Data[35][40]!=WHITE_VAL:在十字路处如果车身比较斜，十字路会与圆环出口差不多，所以加上这个这个条件
//     {   
//       Circle=1;
//       if(direction)
//       {
//           for(H=H_END;H>=H_START;H--)                                     //圆环补右线，即圆环左转
//           {
//               HBoundR[H]=160-(H-(120-H_circle))*160/(double)(H_circle);
//               HBoundL[H]=159;
//               if(H<(110-H_circle))
//               {
//                   HBoundR[H]=159;
//                 
//               }
//               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//           }
//       }
//         
//       else 
//           for(H=H_END;H>=H_START;H--)                                     //圆环补左线
//           {
//               HBoundL[H]=(H-(120-H_circle))*160/(double)(H_circle);
//               HBoundR[H]=0;
//               if(H<(110-H_circle))
//               {
//                   HBoundL[H]=0;
//                   
//               }
//               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//           }
//     }
//     /*********************************************/
//     
//     /*******************障碍物********************/
//    for(H=100;H>20;H--)                                 //50                    开始是100到40
//    {
//        /********识别左障碍物*******/
//        Lbarrier_Weight=0;
//        Lchange=0;                                                       //在弯道,可能上一行Rchange=1;
//        for(W=HBoundM[H];W>HBoundL[H];W--)                              //检测左障碍物
//        {       
//            if (Image_Data[H][W]>Image_Data[H][W-1]||Lchange)
//            {   
//                Lchange=1;                                              //有白到黑的跳变
//                if(Lchange==1&&Image_Data[H][W-1]==BLACK_VAL&&Image_Data[H][W-2]==BLACK_VAL)
//                    Lbarrier_Weight++;                                  //障碍物宽度
//                else
//                {
//                     Lbarrier_end=W;
//                     Lbarrier_H=H;
//                     Lchange=0;
//                     break;
//                } 
//            }
//        }
//        Lbarrierstart=Lbarrier_end+Lbarrier_Weight;                     //障碍物靠近赛道中心的一端
//       /****************************/
//        
//       /********识别右障碍物********/
//       Rbarrier_Weight=0;
//       Rchange=0;                                                       //在弯道,可能上一行Rchange=1;
//       for(W=HBoundM[H];W<HBoundR[H];W++)                               //检测左障碍物
//       {
//            if (Image_Data[H][W]>Image_Data[H][W+1]||Rchange)
//            {   
//                Rchange=1;                                              //有白到黑的跳变
//                if(Rchange==1&&Image_Data[H][W+1]==BLACK_VAL&&Image_Data[H][W+2]==BLACK_VAL)
//                    Rbarrier_Weight++;                                  //障碍物宽度
//                else
//                {
//                     Rbarrier_end=W;
//                     Rbarrier_H=H;
//                     Rchange=0;
//                     break;
//                } 
//            }
//            if(HBoundM[H]-HBoundM[H-1]>10)                             //由于赛道环境限制，在圆环那里，周围的赛道太靠近圆环，会误识别为障碍物，所以这个if语句用来排除周围赛道的干扰
//            {
//                no=0;                                                  //代表不是障碍物，而是周围赛道干扰
//            }
//       }
//       Rbarrierstart=Rbarrier_end-Rbarrier_Weight;                     //障碍物靠近赛道中心的一端
//       /***************************/
//       
//       if(Lbarrier_Weight>20&&HBoundM[Lbarrier_H]!=BLACK_VAL)          //排除远处干扰  ：HBoundM[Rbarrier_H]!=BLACK_VAL                                 
//       {
//            Lbarrier=1;
//            break;
//       }
//       if(Rbarrier_Weight>20&&HBoundM[Rbarrier_H]!=BLACK_VAL)          //Lbarrier_Weight设为10时，起跑线会当成障碍物，所以改为20
//       {
//            Rbarrier=1;
//            break;
//       }
//    }
//    
//    /*******************障碍物补线********************/
//    if(Lbarrier==1&&no&&(Lbarrier_H>H_Min))                            //(Lbarrier_H>H_Min&&(H_Min!=H_START)):排除弯道干扰，急弯时，会误识别为障碍物
//       {
//          for(H=Lbarrier_H;H<H_END;H++)
//          {
//              HBoundL[H]=Lbarrierstart-(H-Lbarrier_H)*Lbarrierstart/(H_END-Lbarrier_H)+10;         //障碍物补线
//          }
//          for(H=Lbarrier_H-1;H>=H_START;H--)
//          {
//              HBoundL[H]=HBoundL[H+1];
//          }
//       }
//       
//       if(Rbarrier==1&&no&&(Rbarrier_H>H_Min))
//       {
//          for(H=Rbarrier_H;H<H_END;H++)
//          {
//              HBoundR[H]=Rbarrierstart+(H-Rbarrier_H)*(160-Rbarrierstart)/(H_END-Rbarrier_H)-10;
//          }
//          for(H=Rbarrier_H-1;H>=H_START;H--)
//          {
//              HBoundR[H]=HBoundR[H+1];
//          }
//       }
//    
//      for(H=H_END;H>=H_START;H--)                                     //障碍物中线规划
//      {
//           
//           HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//          
//          
//      }  
//    /**********************************************/
//     
//    /*for(H=int_min(H_MaxL,H_MaxR);H<=H_END;H++)
//    {
//         HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//    }*/
//    return S;                                                
//    
//}

//int Graph_FindMidLine2()                         //此函数用于模式1
//{
//  
//    /*********障碍物变量*******/                 // L代表左边,R代表右边
//    int Lbarrier_Weight=0;                      //左障碍物的宽度
//    int Lbarrier_end=0;
//    int Lbarrier_H=0;                           //障碍物最近一行的行号
//    int Lbarrierstart=0;
//    int Rbarrier_Weight=0;
//    int Rbarrier_end=0;                         //障碍物靠近赛道中心的一端
//    int Rbarrier_H=0;
//    int Rbarrierstart=0;
//    int Lbarrier=0;                             //标记障碍物的有无
//    int Rbarrier=0;
//    int Lchange=0;                              //记录跳变
//    int Rchange=0;
//    int no=1;                                   //用来排除圆环附近赛道的干扰，以防误识别为障碍物
//    int White_Black=0;
//    /**************************/
//      
//    /********找边线变量********/
//    int H,W;
//    int findLline=0;                            //找到左线标志           
//    int findRline=0;                            //找到右线标志
//    int dDir0,dDir1;                            //十字路变量
//    int GetLineCntL=0;
//    int GetLineCntR=0;
//    int Mid;                                    //每次更新的中线
//    int H1,H2,H3,H4,H5,H6;
//    /**************************/
//      
//    /*********圆环变量*********/  
//    int num=0;                                 //用于初步判断是否是圆环                                 
//    int H_circle=0;                            //代表圆环近端的H   
//    int countL=0,countR=0;
//    int direction=0;                          //车行驶方向控制： 0:圆环左转   1:圆环右转
//    /**************************/
//      
//    /********起始线变量********/
//    int startline=0;                          //用来判断是否是起跑线
//    /**************************/
//    
//    /**********S路变量*********/
//    int White_pixel=0;                          //图像中心白像素点个数
//    int sum;                                    //60-120行的中线之和
//    int S_Dir;                                  //60-120行的偏差均值
//    /**************************/
//    
//    H_Min = H_START;
//    
//    
//    /*******判断是否出界*******/
//    if(Graph_JudgeOut())      
//        return 0;
//    else
//    {
//        for (H=0;H<=CAMERA_H;H++)
//        {
//            HBoundL[H]=MaxStart;
//            HBoundR[H]=MaxEnd;
//            HBoundM[H]=(MaxStart+MaxEnd)/2;
//        }
//         // H_Min = H_START;                      //这个语句没作用，只是为了程序的正确执行，因为如果else里没有语句，遇见圆环会死机
//    }
//    /**************************/
//    
//    dDir0=0;//根据偏差算十字拐角
//    dDir1=0;//根据偏差算十字拐角
//    
//    S=0;
//    DisBuzzer();
//    for(H=H_END;H>=H_START;H--)
//    {   
//        
//        /******S路识别*******/
//        if(Image_Data[H][79]==WHITE_VAL)
//            White_pixel++;
//        /********************/
//        
//        H1=int_min(H+1,H_END);
//        H2=int_min(H+2,H_END);
//        H3=int_min(H+3,H_END);
//        H4=int_min(H+4,H_END);
//        H5=int_min(H+5,H_END);
//        H6=int_min(H+6,H_END);
//        Mid=HBoundM[H+1];
//        
//        if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//        {
//             Mid = W_START+(W_END>>2);
//             if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//             {
//                  Mid = W_END-(W_END>>2);
//                  if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                  {
//                        Mid = W_START+(W_END>>4);
//                        if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                        {
//                             Mid = W_END-(W_END>>4);
//                              if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                              {
//                                   HBoundL[H] = HBoundL[H+1] ;
//                                   HBoundR[H] = HBoundR[H+1] ;
//                                   HBoundM[H] = HBoundM[H+1] ;
//                                    continue;
//                              }
//                        }
//
//                  }
//             }
//        }
//        
//        /***************找左线***************/
//        for(W=W_START;W<Mid;W++)
//        {
//            if (Image_Data[H][W]<Image_Data[H][W+1])
//            {
//
//                    HBoundL[H] = W;
////                    if(int_abs(HBoundL[H]-HBoundL[H1])>5)
////                        HBoundL[H]=HBoundL[H1];
//                    GetLineCntL++;
//                    break;
//            }
//        }
//        if(W==Mid)
//        {
//            HBoundL[H] =HBoundL[H1] ;
//        }
//        
//        /***********************************/
//        
//        
//        /***************找右线***************/
//        for(W=W_END;W>Mid;W--)
//        {
//            if (Image_Data[H][W]<Image_Data[H][W-1])
//            {
//
//                    HBoundR[H] = W;
////                    if(int_abs(HBoundR[H]-HBoundR[H1])>5)
////                        HBoundR[H]=HBoundR[H1];
//                    GetLineCntR++;
//                    break;
//            }
//        }
//        if(W==Mid)
//        {
//            HBoundR[H] =HBoundR[H1] ;
//        }
//        
//        /***********************************/     
//        
//        
//        /**************十字路***************/ 
//        dDir0=HBoundR[H]-HBoundR[H2];
//        dDir1=HBoundR[H2]-HBoundR[H6];
//        
//        if((dDir0*dDir1<-4 && dDir0>0)/*&&(!startline) */|| ((dDir0 > 10 && dDir1<=0)/*&&(!startline)*/))             //(!startline):在起跑线处会误识别为十字路，所以加上(!startline)
//        {
//            //十字路补右线
//            HBoundR[H]=HBoundR[H3]+HBoundR[H3]-HBoundR[H6];
//            HBoundR[H1]=HBoundR[H4]+HBoundR[H3]-HBoundR[H6];
//        }
//        
//        dDir0=HBoundL[H]-HBoundL[H2];
//        dDir1=HBoundL[H2]-HBoundL[H6];
//        
//        
//        if((dDir0*dDir1<-4 && dDir0<0)/*&&(!startline)*/ || ((dDir0 <-10  && dDir1>=0)/*&&(!startline)*/))            
//        {
//            //十字路补左线
//            HBoundL[H]=HBoundL[H3]+HBoundL[H3]-HBoundL[H6];
//            HBoundL[H1]=HBoundL[H4]+HBoundL[H3]-HBoundL[H6];
//        }
//       
//        HBoundM[H]= (HBoundR[H]+ HBoundL[H])/2;
//        if( HBoundR[H] < W_START+LINE_WIDTH || HBoundL[H] > W_END-LINE_WIDTH || (HBoundR[H]-HBoundL[H]) <LINE_WIDTH )
//        {
//             H_Min = H;
//             break;
//        }
//        /***********************************/ 
//
//    }
//    
//    /******S路识别*******/
//    sum=0;
//    for(H=H_END;H>60;H--)
//    {
//        sum+=HBoundM[H];
//    }
//    S_Dir=sum/(H_END-60)-79;
//    
//    if(White_pixel>100&&((S_Dir<25&&S_Dir>1)||(S_Dir>-25&&S_Dir<-1))&&(int_abs((HBoundM[60]+HBoundM[H_END])/2-HBoundM[90])>5))
//    {
//        S=1;                    //S路标志位
//        EnBuzzer();             //蜂鸣器响
//    }
//    /********************/
//    
//        
//     /********************圆环*********************/
//     for(H=80;H>30;H--)                          
//     {
//        if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)           //记录图像中心线有没有跳变
//        {    num=1;
//            H_circle=120-(H-1);
//            break;
//        }
//     }
//    
//     if(num==1)
//     {
//         for(H=H_END;H>=120-H_circle;H--)
//         {
//              if(Image_Data[H][0]==WHITE_VAL)
//                   countL++;
//              else
//              {
//                   countL==0;
//                   break;
//              }
//         }
//         
//         for(H=H_END;H>=120-H_circle;H--)
//         {
//              if(Image_Data[H][159]==WHITE_VAL)
//                   countR++;
//              else
//              {
//                   countR==0;
//                   break;
//              }
//         }
//     }
//     
//    if(countL>=H_circle-30&&(countR>=H_circle-30)/*&&Image_Data[120-H_circle-3][159]==WHITE_VAL*/&&H_circle&&Image_Data[35][40]!=WHITE_VAL/*&&(!startline)*/)       //H_circle&&Image_Data[35][40]!=WHITE_VAL:在十字路处如果车身比较斜，十字路会与圆环出口差不多，所以加上这个这个条件
//     {   
//       if(direction)
//       {
//           for(H=H_END;H>=H_START;H--)                                     //圆环补右线，即圆环左转
//           {
//               HBoundR[H]=160-(H-(120-H_circle))*160/(double)(H_circle);
//               HBoundL[H]=159;
//               if(H<(110-H_circle))
//               {
//                   HBoundR[H]=159;
//                 
//               }
//               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//           }
//       }
//         
//       else 
//           for(H=H_END;H>=H_START;H--)                                     //圆环补左线
//           {
//               HBoundL[H]=(H-(120-H_circle))*160/(double)(H_circle);
//               HBoundR[H]=0;
//               if(H<(110-H_circle))
//               {
//                   HBoundL[H]=0;
//                   
//               }
//               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//           }
//     }
//     /*********************************************/
//     
//     /*******************障碍物********************/
//    for(H=100;H>20;H--)                                 //50                    开始是100到40
//    {
//        /********识别左障碍物*******/
//        Lbarrier_Weight=0;
//        Lchange=0;                                                       //在弯道,可能上一行Rchange=1;
//        for(W=HBoundM[H];W>HBoundL[H];W--)                              //检测左障碍物
//        {       
//            if (Image_Data[H][W]>Image_Data[H][W-1]||Lchange)
//            {   
//                Lchange=1;                                              //有白到黑的跳变
//                if(Lchange==1&&Image_Data[H][W-1]==BLACK_VAL&&Image_Data[H][W-2]==BLACK_VAL)
//                    Lbarrier_Weight++;                                  //障碍物宽度
//                else
//                {
//                     Lbarrier_end=W;
//                     Lbarrier_H=H;
//                     Lchange=0;
//                     break;
//                } 
//            }
//        }
//        Lbarrierstart=Lbarrier_end+Lbarrier_Weight;                     //障碍物靠近赛道中心的一端
//       /****************************/
//        
//       /********识别右障碍物********/
//       Rbarrier_Weight=0;
//       Rchange=0;                                                       //在弯道,可能上一行Rchange=1;
//       for(W=HBoundM[H];W<HBoundR[H];W++)                               //检测左障碍物
//       {
//            if (Image_Data[H][W]>Image_Data[H][W+1]||Rchange)
//            {   
//                Rchange=1;                                              //有白到黑的跳变
//                if(Rchange==1&&Image_Data[H][W+1]==BLACK_VAL&&Image_Data[H][W+2]==BLACK_VAL)
//                    Rbarrier_Weight++;                                  //障碍物宽度
//                else
//                {
//                     Rbarrier_end=W;
//                     Rbarrier_H=H;
//                     Rchange=0;
//                     break;
//                } 
//            }
//            if(HBoundM[H]-HBoundM[H-1]>10)                             //由于赛道环境限制，在圆环那里，周围的赛道太靠近圆环，会误识别为障碍物，所以这个if语句用来排除周围赛道的干扰
//            {
//                no=0;                                                  //代表不是障碍物，而是周围赛道干扰
//            }
//       }
//       Rbarrierstart=Rbarrier_end-Rbarrier_Weight;                     //障碍物靠近赛道中心的一端
//       /***************************/
//       
//       if(Lbarrier_Weight>20&&HBoundM[Lbarrier_H]!=BLACK_VAL)          //排除远处干扰  ：HBoundM[Rbarrier_H]!=BLACK_VAL                                 
//       {
//            Lbarrier=1;
//            break;
//       }
//       if(Rbarrier_Weight>20&&HBoundM[Rbarrier_H]!=BLACK_VAL)          //Lbarrier_Weight设为10时，起跑线会当成障碍物，所以改为20
//       {
//            Rbarrier=1;
//            break;
//       }
//    }
//    
//    /*******************障碍物补线********************/
//    if(Lbarrier==1&&no&&(Lbarrier_H>H_Min))                            //(Lbarrier_H>H_Min&&(H_Min!=H_START)):排除弯道干扰，急弯时，会误识别为障碍物
//       {
//          for(H=Lbarrier_H;H<H_END;H++)
//          {
//              HBoundL[H]=Lbarrierstart-(H-Lbarrier_H)*Lbarrierstart/(H_END-Lbarrier_H)+10;         //障碍物补线
//          }
//          for(H=Lbarrier_H-1;H>=H_START;H--)
//          {
//              HBoundL[H]=HBoundL[H+1];
//          }
//       }
//       
//       if(Rbarrier==1&&no&&(Rbarrier_H>H_Min))
//       {
//          for(H=Rbarrier_H;H<H_END;H++)
//          {
//              HBoundR[H]=Rbarrierstart+(H-Rbarrier_H)*(160-Rbarrierstart)/(H_END-Rbarrier_H)-10;
//          }
//          for(H=Rbarrier_H-1;H>=H_START;H--)
//          {
//              HBoundR[H]=HBoundR[H+1];
//          }
//       }
//    
//      for(H=H_END;H>=H_START;H--)                                     //障碍物中线规划
//      {
//           
//           HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//          
//          
//      }  
//    /**********************************************/
//     
//    /*for(H=int_min(H_MaxL,H_MaxR);H<=H_END;H++)
//    {
//         HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//    }*/
//    return S;                                                
//    
//}




//int Graph_FindMidLine1()                          //此函数用于模式1
//{
//  
//    /*********障碍物变量*******/                   // L代表左边,R代表右边
//      int Lbarrier_Weight=0;                      //左障碍物的宽度
//      int Lbarrier_end=0;
//      int Lbarrier_H=0;                           //障碍物最近一行的行号
//      int Lbarrierstart=0;
//      int Rbarrier_Weight=0;
//      int Rbarrier_end=0;                         //障碍物靠近赛道中心的一端
//      int Rbarrier_H=0;
//      int Rbarrierstart=0;
//      int Lbarrier=0;                             //标记障碍物的有无
//      int Rbarrier=0;
//      int Lchange=0;                              //记录跳变
//      int Rchange=0;
//      int no=1;                                   //用来排除圆环附近赛道的干扰，以防误识别为障碍物
//      
//    /**************************/
//              
//    int H,W;
//    int dDir0,dDir1;
//    int GetLineCntL=0;
//    int GetLineCntR=0;
//    int Mid;
//    int H1,H2,H3,H4,H5,H6;
//    int num=0;
//    int yes=0;
//    int H_circle=0;                       //代表圆环近端的H   
//    int countL=0,countR=0;
//    int direction=0;                         //  0:圆环左转   1:圆环右转
//  //  int W_circle;                       //代表补线的一点的W
//    H_Min = H_START;
//    
//    //MorphOpen((unsigned char*)Image_Data[106],(unsigned char*)Image_DataF[106],CAMERA_W,CAMERA_H-107,9,9);
//    
//   if(Graph_JudgeOut())
//        return 0;
//   else
//   {
//      for (H=0;H<=CAMERA_H;H++)
//      {
//        HBoundL[H]=MaxStart;
//        HBoundR[H]=MaxEnd;
//        HBoundM[H]=(MaxStart+MaxEnd)/2;
//      }
//   }
//    dDir0=0;
//    dDir1=0;//根据偏差算十字拐角
//    
//    for(H=80;H>30;H--)
//    {
//        if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)           //记录图像中心线有没有突变
//        {    num=1;
//            H_circle=120-(H-1);
//            break;
//        }
//    }
//    
//    for(H=H_END;H>=H_START;H--)
//    {   
//      //if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H+1][W_MID]==BLACK_VAL)
//      //    num=1;
//        H1=int_min(H+1,H_END);
//        H2=int_min(H+2,H_END);
//        H3=int_min(H+3,H_END);
//        H4=int_min(H+4,H_END);
//        H5=int_min(H+5,H_END);
//        H6=int_min(H+6,H_END);
//        Mid=HBoundM[H+1];
//        //mexPrintf("%d %d\n",H,Mid);
//        if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//        {
//             Mid = W_START+(W_END>>2);
//             if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//             {
//                  Mid = W_END-(W_END>>2);
//                  if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                  {
//                        Mid = W_START+(W_END>>4);
//                        if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                        {
//                             Mid = W_END-(W_END>>4);
//                              if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                              {
//                                   HBoundL[H] = HBoundL[H+1] ;
//                                   HBoundR[H] = HBoundR[H+1] ;
//                                   HBoundM[H] = HBoundM[H+1] ;
//                                    continue;
//                              }
//                        }
//
//                  }
//             }
//        }
//        //Find Left Bound
//        for(W=W_START;W<Mid;W++)
//        {
//            if (Image_Data[H][W]<Image_Data[H][W+1])
//            {
//                 HBoundL[H] = W;
////                 //为了避免在起跑线找出来的边线比较乱
////                 if(int_abs(HBoundL[H]-HBoundL[H1])>5)
////                      HBoundL[H]=HBoundL[H1];
//                 GetLineCntL++;
//                 break;
//            }
//        }
//        if(W==W_START){
//             GetLineCntL=0;
//            // countL++;                  //代表没找到边线的行数，从图像最底下开始
//             
//        }
//        //Find Right Bound
//        for(W=W_END;W>Mid;W--)
//        {
//            if (Image_Data[H][W]<Image_Data[H][W-1])
//            {
//                 HBoundR[H] = W;
////                 //为了避免在起跑线找出来的边线比较乱
////                 if(int_abs(HBoundR[H]-HBoundR[H1])>5)
////                      HBoundR[H]=HBoundR[H1];
//                 GetLineCntR++;
//                 break;
//            }
//        }
//        if(W==W_END)
//        {
//             GetLineCntR=0;
//            // countR++;
//        }
//        
//        //Cal Ringt 
//        dDir0=HBoundR[H]-HBoundR[H2];
//        dDir1=HBoundR[H2]-HBoundR[H6];
//        
//        
//        if((dDir0*dDir1<-4 && dDir0>0)/*&&(!judge_startline1())*/ || ((dDir0 > 10 && dDir1<=0)/*&&(!judge_startline1())*/))
//        {
//            HBoundR[H]=HBoundR[H3]+HBoundR[H3]-HBoundR[H6];
//            HBoundR[H1]=HBoundR[H4]+HBoundR[H3]-HBoundR[H6];
//            //mexPrintf("R:%d %d\n",dDir0,dDir1);
//        }
//        //else if((num==1&&L))                                  //圆环补右线
//        //{
//        //    HBoundR[H]=HBoundR[H1]-2;
//        //}
//          
//       
//        //Cal Left
//        dDir0=HBoundL[H]-HBoundL[H2];
//        dDir1=HBoundL[H2]-HBoundL[H6];
//        
//        
//        if((dDir0*dDir1<-4 && dDir0<0)/*&&(!judge_startline1())*/ || ((dDir0 <-10  && dDir1>=0)/*&&(!judge_startline1())*/))
//        {
//    
//          HBoundL[H]=HBoundL[H3]+HBoundL[H3]-HBoundL[H6];
//            HBoundL[H1]=HBoundL[H4]+HBoundL[H3]-HBoundL[H6];
//            //mexPrintf("L:%d %d\n",dDir0,dDir1);
//        }
//       // else if((num==1&&R))                                 //圆环补左线
//        //{
//         //   HBoundL[H]=80-(H-H_circle)*80/(double)(120-H_circle);
//        //}
//        
//       // if((dDir0*dDir1<-4 && dDir0<0&&num==1) || (dDir0 < -10 && dDir1>=0&&num==1))
//         //         yes++;
//        HBoundM[H]= (HBoundR[H]+ HBoundL[H])/2;
//        if( HBoundR[H] < W_START+LINE_WIDTH || HBoundL[H] > W_END-LINE_WIDTH || (HBoundR[H]-HBoundL[H]) <LINE_WIDTH )
//        {
//             H_Min = H;
//             break;
//        }
////         if(H_MaxL==H_END && GetLineCntL>3 )
////         {
////             H_MaxL=H;
////         }
////         if(H_MaxR==H_END && GetLineCntR>3 )
////         {
////             H_MaxR=H;
////         }
//    }
////     if(H_MaxL!=H_END && H_MaxL>H_START+8)//not swap Line 
////     {
////          for(H=H_MaxL;H<=H_END;H++)
////          {
////              HBoundL[H]=int_max(HBoundL[H-7]+HBoundL[H-1]-HBoundL[H-8],W_START);
////          }
////     }
////     if(H_MaxR!=H_END && H_MaxR>H_START+8)//not swap Line 
////     {
////          for(H=H_MaxR;H<=H_END;H++)
////          {
////              HBoundR[H]=int_min(HBoundR[H-7]+HBoundR[H-1]-HBoundR[H-8],W_END);
////          }
////     }
//    
//     /********************圆环补线*********************/
//     //圆环补线
//     if(num==1)
//     {
//         for(H=H_END;H>=120-H_circle;H--)
//         {
//             // if(HBoundL[H]==0&&HBoundL[H-1]==0)
//                if(Image_Data[H][0]==WHITE_VAL&&Image_Data[H][0]==WHITE_VAL)
//                   countL++;
//              else
//              {
//                   countL==0;
//                   break;
//              }
//             //HBoundL[H]=80-(H-H_circle)*80/(double)(120-H_circle);                   //圆环补线
//             //HBoundR[H-1]=HBoundR[H]+2;
//        // return 5;
//         }
//         
//         for(H=H_END;H>=120-H_circle;H--)
//         {
//             // if(HBoundR[H]==159&&HBoundR[H-1]==159)
//              if(Image_Data[H][159]==WHITE_VAL&&Image_Data[H][159]==WHITE_VAL)
//                   countR++;
//              else
//              {
//                   countR==0;
//                   break;
//              }
//         }
//     }
//     //if(countR>=H_circle&&Image_Data[120-H_circle-3][159]==WHITE_VAL&&H_circle)
//     if(countR>=H_circle-30&&(countL>=H_circle-30)/*&&Image_Data[120-H_circle-3][159]==WHITE_VAL*/&&H_circle)
//     {   
//       if(direction)
//       {
//           for(H=H_END;H>=H_START;H--)                                     //圆环补右线
//           {
//               //HBoundR[H-1]=HBoundR[H]-2;
//               HBoundR[H]=140-(H-(120-H_circle))*140/(double)(H_circle);
//               HBoundL[H]=159;
//               if(H<(110-H_circle))
//               {
//                   HBoundR[H]=159;
//                 
//               }
//               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//           }
//       }
//         
//       else 
//           for(H=H_END;H>=H_START;H--)                                     //圆环补左线
//           {
//               //HBoundR[H-1]=HBoundR[H]-2;
//               HBoundL[H]=(H-(120-H_circle))*160/(double)(H_circle);
//               HBoundR[H]=0;
//               if(H<(110-H_circle))
//               {
//                   HBoundL[H]=0;
//                   
//               }
//               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//           }
//     }
//    
////    //圆环出口
////    else                                                                                        //如果不是圆环，再判断是否为圆环出口
////        for(H=60;H<30;H--)
////        {
////            for(W=40;W<120;W++)
////            {
////                if(Image_Data[H][W]>Image_Data[H][W+1])                                            //白变黑
////                {
////                    White_Black=1;
////                }
////                if(Image_Data[H][W]<Image_Data[H][W+1]&&White_Black==1&&Black_White==0)           //黑变白
////                {
////                    Black_White=1;
////                    H_exit=H;                                                                     //出口的高
////                    W_exit=W;                                                                     //出口的宽
////                    break;
////                }
////                    
////            }
////        }
////    
////    if(White_Black==1&&Black_White==1)                                                          //圆环出口补线
////    {
////        for(H=H_END;H>=H_exit;H--)
////        {
////            HBoundL[H]=(H-H_exit)*W_exit/(double)(H_END-H_exit);
////        }
////    }
//     /********************圆环补线*********************/
//     
//   /*******************障碍物补线********************/
//   //障碍物补线
//   for(H=115;H>20;H--)                                 //50                    开始是100到40
//   {
//       Lbarrier_Weight=0;
//       Lchange=0;                                      //在弯道,可能上一行Rchange=1;
//       for(W=HBoundM[H];W>HBoundL[H];W--)                //检测左障碍物
//       {       
//           if (Image_Data[H][W]>Image_Data[H][W-1]||Lchange)
//           {   
//               Lchange=1;                               //有白到黑的跳变
//               if(Lchange==1&&Image_Data[H][W-1]==BLACK_VAL&&Image_Data[H][W-2]==BLACK_VAL)
//                   Lbarrier_Weight++;                      //障碍物宽度
//               else
//               {
//                    Lbarrier_end=W;
//                    Lbarrier_H=H;
//                    Lchange=0;
//                    break;
//               } 
//           }
////            else 
////            {
////                 Lbarrier_end=W;
////                 Lbarrier_H=H;
////                break;
////            }
//       }
//       Lbarrierstart=Lbarrier_end+Lbarrier_Weight;       //障碍物靠近赛道中心的一端
//       
//      Rbarrier_Weight=0;
//      Rchange=0;                                        //在弯道,可能上一行Rchange=1;
//      for(W=HBoundM[H];W<HBoundR[H];W++)                //检测左障碍物
//      {
//           if (Image_Data[H][W]>Image_Data[H][W+1]||Rchange)
//           {   
//               Rchange=1;                               //有白到黑的跳变
//               if(Rchange==1&&Image_Data[H][W+1]==BLACK_VAL&&Image_Data[H][W+2]==BLACK_VAL)
//                   Rbarrier_Weight++;                      //障碍物宽度
//               else
//               {
//                    Rbarrier_end=W;
//                    Rbarrier_H=H;
//                    Rchange=0;
//                    break;
//               } 
//           }
//       if(HBoundM[H]-HBoundM[H-1]>10)                    //由于赛道环境限制，在圆环那里，周围的赛道太靠近圆环，会误识别为障碍物，所以这个if语句用来排除周围赛道的干扰
//       {
//            no=0;                                       //代表不是障碍物，而是周围赛道干扰
//       }
////            else 
////            {
////                Rbarrier_end=W;                         
////                Rbarrier_H=H;
////                break;
////            }
//      }
//       Rbarrierstart=Rbarrier_end-Rbarrier_Weight;     //障碍物靠近赛道中心的一端
//      
//      if(Lbarrier_Weight>10&&HBoundM[Lbarrier_H]!=BLACK_VAL)                    //排除远处干扰  ：HBoundM[Rbarrier_H]!=BLACK_VAL                                 
//      {
//           Lbarrier=1;
//           break;
//      }
//      if(Rbarrier_Weight>10&&HBoundM[Rbarrier_H]!=BLACK_VAL)
//      {
//           Rbarrier=1;
//           break;
//      }
//   }
//   
//   //障碍物补线
//      if(Lbarrier==1&&no)
//      {
//         for(H=Lbarrier_H;H<H_END;H++)
//         {
//             HBoundL[H]=Lbarrierstart-(H-Lbarrier_H)*Lbarrierstart/(H_END-Lbarrier_H)+15;         //障碍物补线
//         }
//         for(H=Lbarrier_H-1;H>=H_START;H--)
//         {
//             HBoundL[H]=HBoundL[H+1];
//         }
//         //HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//      }
//      
//      if(Rbarrier==1&&no)
//      {
//         for(H=Rbarrier_H;H<H_END;H++)
//         {
//             HBoundR[H]=Rbarrierstart+(H-Rbarrier_H)*(160-Rbarrierstart)/(H_END-Rbarrier_H)-15;
//         }
//         for(H=Rbarrier_H-1;H>=H_START;H--)
//         {
//             HBoundR[H]=HBoundR[H+1];
//         }
//         //HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//      }
//   
//     for(H=H_END;H>=H_START;H--)                                     //障碍物中线规划
//     {
//          HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//     }
//   
//    /*******************障碍物补线********************/
//    
//    //judge_startline1();
//    for(H=int_min(H_MaxL,H_MaxR);H<=H_END;H++)
//    {
//         HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//    }
//    //if(yes>=2)
//    //  return 1;
//    //else
//      return Lbarrier_Weight;
//    
//}
//
//
//int Graph_FindMidLine2()                          //此函数用于模式1
//{
//  
//    /*********障碍物变量*******/                   // L代表左边,R代表右边
//      int Lbarrier_Weight=0;                      //左障碍物的宽度
//      int Lbarrier_end=0;
//      int Lbarrier_H=0;                           //障碍物最近一行的行号
//      int Lbarrierstart=0;
//      int Rbarrier_Weight=0;
//      int Rbarrier_end=0;                         //障碍物靠近赛道中心的一端
//      int Rbarrier_H=0;
//      int Rbarrierstart=0;
//      int Lbarrier=0;                             //标记障碍物的有无
//      int Rbarrier=0;
//      int Lchange=0;                              //记录跳变
//      int Rchange=0;
//    /**************************/
//              
//    int H,W;
//    int dDir0,dDir1;
//    int GetLineCntL=0;
//    int GetLineCntR=0;
//    int Mid;
//    int H1,H2,H3,H4,H5,H6;
//    int num=0;
//    int yes=0;
//    int H_circle=0;                       //代表圆环近端的H   
//    int countL=0,countR=0;
//    int direction=0;                         //  0:圆环左转   1:圆环右转
//  //  int W_circle;                       //代表补线的一点的W
//    H_Min = H_START;
//    
//    //MorphOpen((unsigned char*)Image_Data[106],(unsigned char*)Image_DataF[106],CAMERA_W,CAMERA_H-107,9,9);
//    
//   if(Graph_JudgeOut())
//        return 0;
//   else
//   {
//      for (H=0;H<=CAMERA_H;H++)
//      {
//        HBoundL[H]=MaxStart;
//        HBoundR[H]=MaxEnd;
//        HBoundM[H]=(MaxStart+MaxEnd)/2;
//      }
//   }
//    dDir0=0;
//    dDir1=0;//根据偏差算十字拐角
//    
//    for(H=80;H>30;H--)
//    {
//        if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H-1][W_MID]==BLACK_VAL)           //记录图像中心线有没有突变
//        {    num=1;
//            H_circle=120-(H-1);
//            break;
//        }
//    }
//    
//    for(H=H_END;H>=H_START;H--)
//    {   
//      //if(Image_Data[H][W_MID]==WHITE_VAL&&Image_Data[H+1][W_MID]==BLACK_VAL)
//      //    num=1;
//        H1=int_min(H+1,H_END);
//        H2=int_min(H+2,H_END);
//        H3=int_min(H+3,H_END);
//        H4=int_min(H+4,H_END);
//        H5=int_min(H+5,H_END);
//        H6=int_min(H+6,H_END);
//        Mid=HBoundM[H+1];
//        //mexPrintf("%d %d\n",H,Mid);
//        if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//        {
//             Mid = W_START+(W_END>>2);
//             if(Image_Data[H][Mid]!=WHITE_VAL || Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//             {
//                  Mid = W_END-(W_END>>2);
//                  if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                  {
//                        Mid = W_START+(W_END>>4);
//                        if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                        {
//                             Mid = W_END-(W_END>>4);
//                              if(Image_Data[H][Mid]!=WHITE_VAL|| Image_Data[H][Mid-1]!=WHITE_VAL  || Image_Data[H][Mid+1]!=WHITE_VAL)
//                              {
//                                   HBoundL[H] = HBoundL[H+1] ;
//                                   HBoundR[H] = HBoundR[H+1] ;
//                                   HBoundM[H] = HBoundM[H+1] ;
//                                    continue;
//                              }
//                        }
//
//                  }
//             }
//        }
//        //Find Left Bound
//        for(W=W_START;W<Mid;W++)
//        {
//            if (Image_Data[H][W]<Image_Data[H][W+1])
//            {
//                 HBoundL[H] = W;
//                 //为了避免在起跑线找出来的边线比较乱
//                 if(int_abs(HBoundL[H]-HBoundL[H1])>10)
//                      HBoundL[H]=HBoundL[H1];
//                 GetLineCntL++;
//                 break;
//            }
//        }
//        if(W==W_START){
//             GetLineCntL=0;
//            // countL++;                  //代表没找到边线的行数，从图像最底下开始
//             
//        }
//        //Find Right Bound
//        for(W=W_END;W>Mid;W--)
//        {
//            if (Image_Data[H][W]<Image_Data[H][W-1])
//            {
//                 HBoundR[H] = W;
//                 //为了避免在起跑线找出来的边线比较乱
//                 if(int_abs(HBoundR[H]-HBoundR[H1])>10)
//                      HBoundR[H]=HBoundR[H1];
//                 GetLineCntR++;
//                 break;
//            }
//        }
//        if(W==W_END)
//        {
//             GetLineCntR=0;
//            // countR++;
//        }
//        
//        //Cal Ringt 
//        dDir0=HBoundR[H]-HBoundR[H2];
//        dDir1=HBoundR[H2]-HBoundR[H6];
//        
//        
//        if((dDir0*dDir1<-4 && dDir0>0) || (dDir0 > 10 && dDir1<=0))
//        {
//            HBoundR[H]=HBoundR[H3]+HBoundR[H3]-HBoundR[H6];
//            HBoundR[H1]=HBoundR[H4]+HBoundR[H3]-HBoundR[H6];
//            //mexPrintf("R:%d %d\n",dDir0,dDir1);
//        }
//        //else if((num==1&&L))                                  //圆环补右线
//        //{
//        //    HBoundR[H]=HBoundR[H1]-2;
//        //}
//          
//       
//        //Cal Left
//        dDir0=HBoundL[H]-HBoundL[H2];
//        dDir1=HBoundL[H2]-HBoundL[H6];
//        
//        
//        if((dDir0*dDir1<-4 && dDir0<0) || (dDir0 <-10  && dDir1>=0))
//        {
//    
//          HBoundL[H]=HBoundL[H3]+HBoundL[H3]-HBoundL[H6];
//            HBoundL[H1]=HBoundL[H4]+HBoundL[H3]-HBoundL[H6];
//            //mexPrintf("L:%d %d\n",dDir0,dDir1);
//        }
//       // else if((num==1&&R))                                 //圆环补左线
//        //{
//         //   HBoundL[H]=80-(H-H_circle)*80/(double)(120-H_circle);
//        //}
//        
//       // if((dDir0*dDir1<-4 && dDir0<0&&num==1) || (dDir0 < -10 && dDir1>=0&&num==1))
//         //         yes++;
//        HBoundM[H]= (HBoundR[H]+ HBoundL[H])/2;
//        if( HBoundR[H] < W_START+LINE_WIDTH || HBoundL[H] > W_END-LINE_WIDTH || (HBoundR[H]-HBoundL[H]) <LINE_WIDTH )
//        {
//             H_Min = H;
//             break;
//        }
////         if(H_MaxL==H_END && GetLineCntL>3 )
////         {
////             H_MaxL=H;
////         }
////         if(H_MaxR==H_END && GetLineCntR>3 )
////         {
////             H_MaxR=H;
////         }
//    }
////     if(H_MaxL!=H_END && H_MaxL>H_START+8)//not swap Line 
////     {
////          for(H=H_MaxL;H<=H_END;H++)
////          {
////              HBoundL[H]=int_max(HBoundL[H-7]+HBoundL[H-1]-HBoundL[H-8],W_START);
////          }
////     }
////     if(H_MaxR!=H_END && H_MaxR>H_START+8)//not swap Line 
////     {
////          for(H=H_MaxR;H<=H_END;H++)
////          {
////              HBoundR[H]=int_min(HBoundR[H-7]+HBoundR[H-1]-HBoundR[H-8],W_END);
////          }
////     }
//    
//     /********************圆环补线*********************/
//     //圆环补线
//     if(num==1)
//     {
//         for(H=H_END;H>=120-H_circle;H--)
//         {
//             // if(HBoundL[H]==0&&HBoundL[H-1]==0)
//                if(Image_Data[H][0]==WHITE_VAL&&Image_Data[H][0]==WHITE_VAL)
//                //if(HBoundL[H]==WHITE_VAL)
//                   countL++;
//              else
//              {
//                   countL==0;
//                   break;
//              }
//             //HBoundL[H]=80-(H-H_circle)*80/(double)(120-H_circle);                   //圆环补线
//             //HBoundR[H-1]=HBoundR[H]+2;
//        // return 5;
//         }
//         
//         for(H=H_END;H>=120-H_circle;H--)
//         {
//             // if(HBoundR[H]==159&&HBoundR[H-1]==159)
//              if(Image_Data[H][0]==WHITE_VAL&&Image_Data[H][0]==WHITE_VAL)
//           //if(HBoundL[H]==WHITE_VAL)
//                   countR++;
//              else
//              {
//                   countR==0;
//                   break;
//              }
//         }
//     }
//     //if(countR>=H_circle&&Image_Data[120-H_circle-3][159]==WHITE_VAL&&H_circle)
//     if(countR>=H_circle-30/*&&Image_Data[120-H_circle-3][159]==WHITE_VAL*/&&H_circle)
//     {   
//       if(direction)
//       {
//           for(H=H_END;H>=H_START;H--)                                     //圆环补右线
//           {
//               //HBoundR[H-1]=HBoundR[H]-2;
//               HBoundR[H]=140-(H-(120-H_circle))*140/(double)(H_circle);
//               HBoundL[H]=159;
//               if(H<(110-H_circle))
//               {
//                   HBoundR[H]=159;
//                 
//               }
//               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//           }
//       }
//         
//       else 
//           for(H=H_END;H>=H_START;H--)                                     //圆环补左线
//           {
//               //HBoundR[H-1]=HBoundR[H]-2;
//               HBoundL[H]=(H-(120-H_circle))*140/(double)(H_circle);
//               HBoundR[H]=0;
//               if(H<(110-H_circle))
//               {
//                   HBoundL[H]=0;
//                   
//               }
//               HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//           }
//     }
//     /********************圆环补线*********************/
//     
////   /*******************障碍物补线********************/
////   //障碍物补线
////   for(H=100;H>40;H--)                                 //50
////   {
////       Lbarrier_Weight=0;
////       Lchange=0;                                      //在弯道,可能上一行Rchange=1;
////       for(W=HBoundM[H];W>HBoundL[H];W--)                //检测左障碍物
////       {       
////           if (Image_Data[H][W]>Image_Data[H][W-1]||Lchange)
////           {   
////               Lchange=1;                               //有白到黑的跳变
////               if(Lchange==1&&Image_Data[H][W-1]==BLACK_VAL&&Image_Data[H][W-2]==BLACK_VAL)
////                   Lbarrier_Weight++;                      //障碍物宽度
////               else
////               {
////                    Lbarrier_end=W;
////                    Lbarrier_H=H;
////                    Lchange=0;
////                    break;
////               } 
////           }
//////            else 
//////            {
//////                 Lbarrier_end=W;
//////                 Lbarrier_H=H;
//////                break;
//////            }
////       }
////       Lbarrierstart=Lbarrier_end+Lbarrier_Weight;       //障碍物靠近赛道中心的一端
////       
////      Rbarrier_Weight=0;
////      Rchange=0;                                        //在弯道,可能上一行Rchange=1;
////      for(W=HBoundM[H];W<HBoundR[H];W++)                //检测左障碍物
////      {
////           if (Image_Data[H][W]>Image_Data[H][W+1]||Rchange)
////           {   
////               Rchange=1;                               //有白到黑的跳变
////               if(Rchange==1&&Image_Data[H][W+1]==BLACK_VAL&&Image_Data[H][W+2]==BLACK_VAL)
////                   Rbarrier_Weight++;                      //障碍物宽度
////               else
////               {
////                    Rbarrier_end=W;
////                    Rbarrier_H=H;
////                    Rchange=0;
////                    break;
////               } 
////           }
//////            else 
//////            {
//////                Rbarrier_end=W;                         
//////                Rbarrier_H=H;
//////                break;
//////            }
////      }
////       Rbarrierstart=Rbarrier_end-Rbarrier_Weight;     //障碍物靠近赛道中心的一端
////      
////      if(Lbarrier_Weight>10/*&&HBoundM[Lbarrier_H]!=BLACK_VAL*/)                    //排除远处干扰  ：HBoundM[Rbarrier_H]!=BLACK_VAL                                 
////      {
////           Lbarrier=1;
////           break;
////      }
////      if(Rbarrier_Weight>10/*&&HBoundM[Rbarrier_H]!=BLACK_VA*/)
////      {
////           Rbarrier=1;
////           break;
////      }
////   }
////   
////   //障碍物补线
////      if(Lbarrier==1)
////      {
////         for(H=Lbarrier_H;H<H_END;H++)
////         {
////             HBoundL[H]=Lbarrierstart-(H-Lbarrier_H)*Lbarrierstart/(H_END-Lbarrier_H);         //障碍物补线
////         }
////         for(H=Lbarrier_H-1;H>=H_START;H--)
////         {
////             HBoundL[H]=HBoundL[H+1];
////         }
////         //HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
////      }
////      
////      if(Rbarrier==1)
////      {
////         for(H=Rbarrier_H;H<H_END;H++)
////         {
////             HBoundR[H]=Rbarrierstart+(H-Rbarrier_H)*(160-Rbarrierstart)/(H_END-Rbarrier_H);
////         }
////         for(H=Rbarrier_H-1;H>=H_START;H--)
////         {
////             HBoundR[H]=HBoundR[H+1];
////         }
////         //HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
////      }
////   
////     for(H=H_END;H>=H_START;H--)                                     //障碍物中线规划
////     {
////          HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
////     }
////   
////    /*******************障碍物补线********************/
//    
//    for(H=int_min(H_MaxL,H_MaxR);H<=H_END;H++)
//    {
//         HBoundM[H]=(HBoundL[H]+HBoundR[H])/2;
//    }
//    //if(yes>=2)
//    //  return 1;
//    //else
//      return Lbarrier_Weight;
//    
//}
//


static void Graph_AverageMBound(void)
{
     int H;
     int i;
     int sum;
     for(H=H_END;H>H_Min;H--)
     {
         HBoundM_F[H]=HBoundM[H];
     }
     for(H=H_END-AVG_COUNT/2-1;H>H_Min+AVG_COUNT/2+1;H--)
     {
         sum=0;
         for(i=-AVG_COUNT/2;i<=AVG_COUNT/2;i++)
            sum+=HBoundM[H+i];
         HBoundM_F[H]=sum/AVG_COUNT;
     }
}

int Graph_Cam2Real(int H)
{
    int D;
    
    D=(int)(4188.0/(H+17.0)-10.0);
    //D = (int)(4153 / H - 16.11);//(young)
    D=int_max(0,D);
    D=int_min(CAM_MAX_LENGTH_CM,D);
    
    return D;
}

int Graph_Real2Cam(int D)
{
    int H;
    
    //D=(int)(4188.0/(H+17.0)-10);
    H = (int)(4188 / (D + 10.0) - 17.0);
    D=int_max(H_START,D);
    D=int_min(H_END,D);
    
    return H;
}
/*将纵向摄像头行转化为纵向厘米数*/
void Graph_Cam2Real_BoundM(void)
{
    int H;
    int D=0;
    int LastD;
    int d;
    
    for(H=H_END;H>=H_Min;H--)
    {
        LastD=D;
        D=Graph_Cam2Real(H);
        HBoundM_REAL[D]=HBoundM_F[H];
        for(d=LastD;d<D;d++)
        {
            
          HBoundM_REAL[d]=HBoundM_F[H+1]+(d-LastD)*(HBoundM_F[H]-HBoundM_F[H+1])/(D-LastD);
        }
    }
    D_Max=D;
    
    D=Graph_Cam2Real(H_END);
    for(d=D;d>=0;d--)
         HBoundM_REAL[d]=HBoundM_REAL[D];
}

void MY_Graph_Calculate_Dir(int Speed)
{
    
}

void Graph_Calculate_Dir(int Speed)
{
    int D;
    int sum;
    int DStart;
    int DEnd;
    int Dir;
    
    Graph_AverageMBound();
    Graph_Cam2Real_BoundM();

    //mexPrintf("D_Max H_Min %d %d\n",D_Max,H_Min);
    //Near Dir Calculate
    DStart = int_max(0,gParam.NearStart);
    DEnd   = int_min(gParam.NearStart+gParam.NearLen,D_Max);
    //mexPrintf("Near %d %d\n",DStart,DEnd);
    sum=0;
    for (D=DStart;D<=DEnd;D++)
    {
      sum+= HBoundM_REAL[D];
    }
    gDir_Near = sum/(DEnd-DStart+1)-W_MID;

    
    //Mid Dir Calculate
    DStart = int_min((int)(gParam.MidDirSpeedK*Speed),D_Max-10);
    DEnd   = int_min(DStart+gParam.MidLen,D_Max);
    
    //mexPrintf("Mid %d %d\n",DStart,DEnd);
    sum=0;
    for (D=DStart;D<=DEnd;D++)
    {
      sum+= HBoundM_REAL[D];
    }
     Dir= sum/(DEnd-DStart+1)-W_MID;
    gDir_Mid=int_min(Dir,W_MID);
    gDir_Mid=int_max(Dir,-W_MID);
    
    //Far Dir Calculate
    DStart = int_min(gParam.FarStart,D_Max);
    DEnd   = int_min(DStart+gParam.FarLen,D_Max);
    
    //mexPrintf("Far %d %d\n",DStart,DEnd);
    sum=0;
    for (D=DStart;D<=DEnd;D++)
    {
      sum+= HBoundM_REAL[D];
    }
    gDir_Far = sum/(DEnd-DStart+1)-W_MID;
}


int  judge_startline1()                  //新起跑线,用于在起跑线处误识别为圆环 或者十字路
{
    int White_Black=0;          //记录像素点跳变的次数
    int H,W;
    
    for(H=H_END;H>60;H--)
        {
            White_Black=0;
            for(W=W_START;W<W_END-1;W++)
            {
                if(Image_Data[H][W]>Image_Data[H][W+1]||Image_Data[H][W]<Image_Data[H][W+1])  //白变黑或者黑跳白
                {
                    White_Black+=1;
                }
            }
            if(White_Black>6)        //是起跑线,只要一行满足就退出循环
                break;
        }
     if(White_Black>6)
     {
//        for(H=H_START;H<H_END;H++)
//        {
//            HBoundM[H]=79;              //把中线赋值为图像中心，左右边线不用管
//        }
        return 1;
     }
     else 
        return 0;
}
int  judge_startline()                        //旧起跑线
{
	int left_flag=0,right_flag=0;		//左右起跑线标志标志 
	int flag=0;							//起跑线标志 
        int Startline_count=0;                                            //是起跑线的行数
        int row,col,col1;
        //行列，for循环用
	unsigned int countL,countR;			//记录左右跳变次数
	unsigned int pixel_memery2[2];		//用来判断跳变 
//	unsigned int Sline_flag;			//记录检测到的起跑线的行数 
	for(row=105;row>=60;row--)
	{	
		left_flag=0;
		right_flag=0;
		countL=0;
		countR=0;
		col1=HBoundM_F[row];
		if(Image_Data[row][col1]==WHITE_VAL)
		{
			pixel_memery2[0]=WHITE_VAL;
			pixel_memery2[1]=WHITE_VAL;
		}
		if(Image_Data[row][col1]==BLACK_VAL)
		{
			pixel_memery2[0]=BLACK_VAL;
			pixel_memery2[1]=BLACK_VAL;
		}
		for(col=HBoundM_F[row];col>HBoundL[row];col--)	//从中间检测左半边
		{
			if(Image_Data[row][col]==BLACK_VAL)
			{
				pixel_memery2[1]=pixel_memery2[0];
				pixel_memery2[0]=BLACK_VAL;
				if(pixel_memery2[0]!=pixel_memery2[1])
					countL++;
			 } 
			else if(Image_Data[row][col]==WHITE_VAL)
			{
				pixel_memery2[1]=pixel_memery2[0];
				pixel_memery2[0]=WHITE_VAL;
				if(pixel_memery2[0]!=pixel_memery2[1])
					countL++;
			 } 
		}
		if(countL>=4)
		{
			left_flag=1;
		}
		if(Image_Data[row][col1]==WHITE_VAL)
		{
			pixel_memery2[0]=WHITE_VAL;
			pixel_memery2[1]=WHITE_VAL;
		}
		if(Image_Data[row][col1]==BLACK_VAL)
		{
			pixel_memery2[0]=BLACK_VAL;
			pixel_memery2[1]=BLACK_VAL;
		}
		for(col=HBoundM_F[row];col<HBoundR[row];col++)	//从中间检测右半边
		{
			if(Image_Data[row][col]==BLACK_VAL)
			{
				pixel_memery2[1]=pixel_memery2[0];
				pixel_memery2[0]=BLACK_VAL;
				if(pixel_memery2[0]!=pixel_memery2[1])
					countR++;
			 } 
			else if(Image_Data[row][col]==WHITE_VAL)
			{
				pixel_memery2[1]=pixel_memery2[0];
				pixel_memery2[0]=WHITE_VAL;
				if(pixel_memery2[0]!=pixel_memery2[1])
					countR++;
			 } 
		} 
                if(countR>=4)
		{
			right_flag=1;
		}
		if(left_flag==1&&right_flag==1)
		{
			Startline_count++;
		}
	}
	if(Startline_count>5) 
	{
		return 1;
	}	
	else
	{
		return 0;
	}
} 

int Circle_route()                       //环形赛道识别
{
    imUINT8 Graph_MidLine_black_num=0;       //环形赛道中间黑色区域图像中心线的黑点数目
    imUINT8 Graph_MidLine_white_num1=0;      //环形赛道远处白色区域图像中心线的白点数目
    imUINT8 Graph_MidLine_white_num2=0;      //环形赛道近处白色区域图像中心线的白点数目
    imUINT8 H_start=0;
    imUINT8 H_end=20;
    imUINT8 pointA,pointB,pointC,pointD=25;    //图像中心线上的四个突变点
    imUINT8 L_exit=0;                         //左出口
    imUINT8 R_exit=0;                         //右出口
    imUINT8 countL=0,countR=0;		    //记录左右跳变次数

    int i;
    for(i=H_start;i<H_end;i++)
    {
        if(i<9)
        {
            if(Image_Data[i][W_MID]==BLACK_VAL&&Image_Data[i+1][W_MID]==WHITE_VAL)
                pointA=i;
            if(Image_Data[i+1][W_MID]==BLACK_VAL&&Image_Data[i][W_MID]==WHITE_VAL)
                pointB=i+1;
        } 
        else
        {
            if(Image_Data[i][W_MID]==BLACK_VAL&&Image_Data[i+1][W_MID]==WHITE_VAL)
                pointC=i;
        } 
        if(i>pointC)
        {
            if(Image_Data[i][W_MID]==BLACK_VAL)            //排除干扰
                return 0;
        }
        //判断左出口
        if(Image_Data[i][W_START]==BLACK_VAL&&Image_Data[i+1][W_START]==WHITE_VAL)
            countL++;
        if(countL==1&&Image_Data[i+1][W_START]==BLACK_VAL&&Image_Data[i][W_START]==WHITE_VAL)
            countL++;
        //判断右出口
        if(Image_Data[i][W_END]==BLACK_VAL&&Image_Data[i+1][W_END]==WHITE_VAL)
            countR++;
        if(countL==1&&Image_Data[i+1][W_END]==BLACK_VAL&&Image_Data[i][W_END]==WHITE_VAL)
            countR++;
    }
    
    Graph_MidLine_white_num1=pointB-pointA;
    Graph_MidLine_black_num =pointC-pointB+1;
    Graph_MidLine_white_num2=pointD-pointC;
    if(Graph_MidLine_white_num1>=2&&Graph_MidLine_black_num>=6&&Graph_MidLine_white_num2>=5)           //可能会误判，需要看其他各种图像会不会出现这种情况
    {
        if(countL==2)           //左出口
            L_exit=1;
        if(countL==2)           //右出口
            R_exit=1;
        return 1;
    }
    else
        return 0;
}



