

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
    imfilename=strcat('C:\Users\����\Desktop\����ͷ\2017.6.30��������\CarSmart_2016Ѱ���޸Ĺ��� - ����-�������ݴ��� - ������������ - ����4.8 - ����\Graphic\S·\Imag',int2str(i),'.txt');
    svfilename=strcat('C:\Users\����\Desktop\����ͷ\2017.6.30��������\CarSmart_2016Ѱ���޸Ĺ��� - ����-�������ݴ��� - ������������ - ����4.8 - ����\Graphic\S·_Solve\Imag',int2str(i),'.bmp');
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