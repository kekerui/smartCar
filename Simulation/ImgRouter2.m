img =C1;
[H W] =size(img);

WDiff2=[zeros(H,1) abs(diff(img,1,2))];
HDiff2=[zeros(1,W); abs(diff(img,1,1))];
H_START=3;
H_END  =60;
W_START=2;
W_END  =79;

LLine=[1 W_END];
RLine=[1 W_START];
Thred =10;


for h=H_START:H_END
    D=LLine(end,2)-RLine(end,2);
    for w= int8(LLine(end,2)-D/3):W_END
        if WDiff2(h,w)>Thred 
            LLine=[LLine;h w];
            break
        end
    end
    for w= int8(RLine(end,2)+D/3):-1:W_START
        if WDiff2(h,w)>Thred 
            RLine=[RLine;h w];
            break
        end
    end
    
end


figure(1)
imshow(img,[0 255])
figure(2)
plot(LLine(:,1),LLine(:,2),'-r')
hold on
plot(RLine(:,1),RLine(:,2),'-b')


