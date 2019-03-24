img =C3;
[L W] =size(img);
imgMax=max(max(img));
imgMin=min(min(img));
imgDiffX=diff(img,1,2);
imgDiffY=abs(diff(img,1,1));
LLine=zeros(L,1);
RLine=zeros(L,1)+W-1;
MLine=zeros(L,1);
WLine=zeros(W,1);
Thred =8;
lStep=1;
wStep=1;

for i=lStep+1:lStep:L
    D=RLine(i-lStep)-LLine(i-lStep);
    for j= int8(LLine(i-lStep)+D/2):-wStep:2
        if imgDiffX(i,j)>Thred 
            LLine(i)=j;
            break
        end
    end
    for j= int8(RLine(i-lStep)-D/2):wStep:W-2
        if imgDiffX(i,j)<-Thred 
            RLine(i)=j;
            break
        end
    end
    MLine(i)=(RLine(i)+LLine(i))/2;
end


for j= 1:wStep:W
    for i=lStep+1:lStep:L-1
       if (imgDiffY(i,j)>Thred)
           WLine(j)=i;
           break
       end
    end
end
maxW=max(WLine);

figure(1)
imshow(img,[0 255])

figure(2)
plot(LLine)
hold on
plot(RLine)
hold on
plot(MLine)
hold on
dir1=sum(MLine(11:45))/35
dir2=sum(MLine(30:45))/16

figure(3)
plot(WLine)


