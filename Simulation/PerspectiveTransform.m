clc;
h=33;
T= (-42)/180*pi;
d=50;
c=cos(T);
s=sin(T);

P1=[1 0 0 0
    0 1 0 0
    0 0 1 0
    0 0 -h 1];
P2=[c 0 s 0
    0 1 0 0
    -s 0 c 0
    0 0 0 1];
P3=[1 0 0 0
    0 1 0 0
    0 0 1 0
    0 0 d 1];
P4=[1 0 0 0
    0 1 0 0
    0 0 1 -1/d
    0 0 0 1];
A= P1*P2*P3*P4
tA=[c 0 s -s/d+1
    0 1 0 0
    -s 0 c -c/d
    h*s 0 -h*c+d h*c/d]
invA=inv(A);   
 
W=160;
Pw=zeros(W+1,4);

for i=0:W
        Pw(i+1,1)=i;
  Pw(i+1,2)=25;
  Pw(i+1,4)=1;
end
Pc=Pw*A;
Pc(:,1)=Pc(:,1)./Pc(:,4);
Pc(:,2)=Pc(:,2)./Pc(:,4);
Pc(:,3)=Pc(:,3)./Pc(:,4);
Pc(:,4)=1;

iPw=Pc*invA;
iPw(:,1)=iPw(:,1)./iPw(:,4);
iPw(:,2)=iPw(:,2)./iPw(:,4);
iPw(:,3)=iPw(:,3)./iPw(:,4);
iPw(:,4)=1;
figure(3)
%plot(Pc(:,1),Pc(:,2));
grid on

B= -(h*c^2 - d*c + h*s^2)/(c^2 + s^2 - d*s)*d/h;
A= (s - d + c*h)/(c^2 + s^2 - d*s)*d/h;

CLLine=ones(length(LLine),4);
CLLine(:,1)=(LLine(:,1)-40);
CLLine(:,2)=LLine(:,2)-40;
CLLine(:,3)=CLLine(:,1)*A+B;



WLLine=CLLine*invA;
WLLine(:,1)=WLLine(:,1)./WLLine(:,4);
WLLine(:,2)=WLLine(:,2)./WLLine(:,4);
WLLine(:,3)=WLLine(:,3)./WLLine(:,4);
WLLine(:,4)=1;

CRLine=ones(length(RLine),4);
CRLine(:,1)=(RLine(:,1)-40);
CRLine(:,2)=RLine(:,2)-40;
CRLine(:,3)=CRLine(:,1)*A+B;
WRLine=CRLine*invA;
WRLine(:,1)=WRLine(:,1)./WRLine(:,4);
WRLine(:,2)=WRLine(:,2)./WRLine(:,4);
WRLine(:,3)=WRLine(:,3)./WRLine(:,4);
WRLine(:,4)=1;


hold on
plot(LLine(:,1),LLine(:,2))
hold on
plot(RLine(:,1),RLine(:,2))
hold on
plot(WLLine(:,1)*1.7,WLLine(:,2),'-r')
hold on
plot(WRLine(:,1)*1.7,WRLine(:,2),'-r')
