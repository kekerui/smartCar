%Cam1
Cam1_H_Dis =[10 20 40 60 90];
Cam1_H_Row =[0 17 37 53 60];
Cam1_H_RowC =[5 15 20 25 30 35 40 45 50 60];

Cam1_H_DisC=Cam1_H_RowC.^2/50 + 10;
figure(1)
plot(Cam1_H_Row,Cam1_H_Dis,'-r')
hold on
plot(Cam1_H_RowC,Cam1_H_DisC,'-k')
grid on
axis([0 60 0 90])

%Cam2
Cam2_H_Dis =[15 20 40 60 80 130];
Cam2_H_Row =[2 7 29 43 50 60];
Cam2_H_RowC =[1 15 20 25 30 35 40 45 50 60];

Cam2_H_DisC=Cam1_H_RowC.^2/35 + 16;
figure(2)
plot(Cam2_H_Row,Cam2_H_Dis,'-r')
hold on
plot(Cam2_H_RowC,Cam2_H_DisC,'-k')
grid on
axis([0 60 0 150])

