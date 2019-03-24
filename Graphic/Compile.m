

clc;
 clear mex
mex -I"../ControlLib/Inc" ...,
    imCar.c ...,
    imProc.c ...,
    imCom.c ...,
    ../ControlLib/ControlParam.c
DIR=zeros(1080,3);
for i=83:97
    i
    try
    imfilename=strcat('C:\Users\柯锐\Desktop\摄像头\2017.6.30代码整理\CarSmart_2016寻线修改过的 - 副本-无线数据传输 - 副本改起跑线 - 副本4.8 - 副本\Graphic\S路\Imag',int2str(i),'.txt');
    svfilename=strcat('C:\Users\柯锐\Desktop\摄像头\2017.6.30代码整理\CarSmart_2016寻线修改过的 - 副本-无线数据传输 - 副本改起跑线 - 副本4.8 - 副本\Graphic\S路_Solve\Imag',int2str(i),'.bmp');
    %img=uint8(not(imread(imfilename))*255)';
    img=uint8(load(imfilename))';
    [W H]=size(img);
    if W ~=160 && H~= 120
        continue
    end
%img=img';
    [L R M  dir imOut M_F M_Real]=imCar(img);
%     DIR(i,:)=dir;
    figure(1)
    imshow(img)
    figure(2)
  imshow(imOut)
  hold on
  plot(1:1:120,L,'-r')
  hold on
  plot(1:1:120,R,'-b')
  hold on
  plot(1:1:120,M,'-k')
  hold on
  tx = text(0.18,0.05,int2str(dir)); 
  set(tx,'Color','r');
  saveas(gcf,svfilename)
   %close all
    clear mex
    catch ME
        continue
    end
end
% plot(DIR(:,1),'-r')
% hold on
% plot(DIR(:,2),'-b')
% hold on
% plot(DIR(:,3),'-k')