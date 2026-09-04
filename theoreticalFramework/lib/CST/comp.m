%% step1 导入翼型
clear;
clc;
airfoilname = 'naca2412.dat';
data        = importdata(airfoilname); %Load Airfoil Coordianted from http://airfoiltools.com/airfoil/naca4digit
data = data.data;
XB = data(:,1);
YB = data(:,2);
lengthAir = length(XB);
Yte = YB(1);



%% step2 翼型分割
i = 1;
a= 0;
while (XB(i) ~= 0)
    XB1(i) = XB(i);
    YB1(i) = YB(i) + a;
    i = i+1;
end
XB1(i) = XB(i);
YB1(i) = YB(i) + a ;
XB1 = XB1';
YB1 = YB1';           % y up
XB2 = XB(i:lengthAir);
YB2 = YB(i:lengthAir) - a;% y lo
if (XB2(1)==0)
    YBF2 = flipud(YB2);
end
%% step3 求解cst参数
Order = [5,0.5,1];  %cst参数

cst1 = BuildBernstein(Order,XB1,YB1,Yte);
cst2 = BuildBernstein(Order,XB2,YB2,-Yte);

aa = (cst1+cst2) / 2
%% step4 计算并写入新翼型坐标文件
yB1 = BuildCurve(cst1,Order,XB1,Yte);
yB2 = BuildCurve(cst2,Order,XB2,-Yte);
dd = BuildCurve((cst1 + cst2) / 2,Order,XB1,0);
X = XB;
Y = [yB1';yB2'];

%airfoildata = [X,Y];
% mm = 0.04;
% pp = 0.4;
% for i = 1:length(XB1)
% 
% if(XB1(i) <= pp)
%     yy(i) = mm/pp/pp*(2*pp*XB1(i)-XB1(i)*XB1(i));
% else
%     yy(i) = mm/((1-pp)*(1-pp))*((1-2*pp)+2*pp*XB1(i) - XB1(i)*XB1(i));
% end
% end
% 
% 
% fid = fopen('j.dat', 'wt');%写入当前文件夹
% fprintf(fid, '%6.8f %12.8f\n', airfoildata');%按原先规格排列
% fclose(fid);
% %% step5 生成优化后xfoil所需的计算文本
% delete('output-j.dat');
% fid=fopen('input-j.dat','wt');
% fprintf(fid,'LOAD \n');
% fprintf(fid,'j.dat\n');
% fprintf(fid,'PANE \n');
% fprintf(fid,'OPER \n');
% fprintf(fid,'VISC %12d \n',300000);
% fprintf(fid,'M %12.3f \n',0.1);
% fprintf(fid,'ITER %12.3f \n',100);
% fprintf(fid,'PACC \n');
% fprintf(fid,'output-j.dat \n');
% fprintf(fid,' \n');
% fprintf(fid,'ASEQ %12.3f %12.3f %12.3f \n', [1.55 1.55 1]);
% %fprintf(fid,'A %f \n',2);
% fprintf(fid,'PACC \n');
% fprintf(fid,'CPWR \n');
% fprintf(fid,'j.txt \n');
% fprintf(fid,'PACC \n');
% fprintf(fid,'QUIT \n');
% %fprintf(fid,'polar.txt \n');
% fclose(fid);
% %% 生成原始翼型xfoil所需的文本
% delete('output-i.dat');
% fid=fopen('input-i.dat','wt');
% fprintf(fid,'LOAD \n');
% fprintf(fid,airfoilname);
% %fprintf(fid,'rae2822.dat\n');
% fprintf(fid,'\n PANE \n');
% fprintf(fid,'OPER \n');
% fprintf(fid,'VISC %12d \n',300000);
% fprintf(fid,'M %12.3f \n',0.1);
% fprintf(fid,'ITER %12.3f \n',100);
% fprintf(fid,'PACC \n');
% fprintf(fid,'output-i.dat \n');
% fprintf(fid,' \n');
% fprintf(fid,'ASEQ %12.3f %12.3f %12.3f \n', [1.55 1.55 1]);
% %fprintf(fid,'A %f \n',2);
% fprintf(fid,'PACC \n');
% fprintf(fid,'CPWR \n');
% fprintf(fid,'i.txt \n');
% fprintf(fid,'PACC \n');
% fprintf(fid,'QUIT \n');
% %fprintf(fid,'polar.txt \n');
% fclose(fid);
% %% step6 调用xfoil求解翼型气动力
% !xfoil<input-j.dat
% clc
% [A10,B10,C,D,E,F,G]=textread('output-j.dat','%12.5f %12.5f   %12.5f  %12.5f %12.5f %12.5f %12.5f','delimiter',',','headerlines',12);
% display('    迎角        CL         CD     CL/CD      CDp       CM      TOP_Xtr     BOT_Xtr');
% !xfoil<input-i.dat
% clc
% [A11,B11,C1,D1,E1,F1,G1]=textread('output-i.dat','%12.5f %12.5f   %12.5f  %12.5f %12.5f %12.5f %12.5f','delimiter',',','headerlines',12);
% display('    迎角        CL         CD     CL/CD      CDp       CM      TOP_Xtr     BOT_Xtr');
% %% step7 对比升阻力系数
% figure(1)
%  plot(airfoildata(:,1),airfoildata(:,2),data(:,1),data(:,2),'o');
%  hold on
%  plot(XB1,dd);
 figure(2)
 plot(XB1,YB1,'bo',XB2,YB2,'ro');

 axis off
% figure(2)
% plot(A10,B10,A11,B11);