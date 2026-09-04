
clc;
clear;
%二元一次插值算法
ad = importdata('1.txt');
bd = importdata('2.txt');
cd = importdata('3.txt');
dd = importdata('4.txt');
a = 3;
b = 9;
x1 = ad(:,1);
y1 = ad(:,a);
x2 = bd(:,1);
y2 = bd(:,a);
x3 = cd(:,1);
y3 = cd(:,a);
x4 = dd(:,1);
y4 = dd(:,a);
test = bd(:,3);

yy1 = ad(:,b);
yy2 = bd(:,b);
yy3 = cd(:,b);
yy4 = dd(:,b);


rpm = 2000;
rpms = [1000,3000,4000];

%%
t = 1 - (rpm - rpms(1)) / 3000;
for i = 1:30
    yn1(i) = ad(2,1) * (i - 1);
    yn2(i) = dd(2,1) * (i - 1);
    xi(i) = yn1(i) * (t) + yn2(i)*(1 - t);
    n(i) = i - 1;
end
%%





%xi = bd(:,1);
yi1 = two_point_interpolation(x1, y1, xi);
yi2 = two_point_interpolation(x2, y2, xi);
yi3 = two_point_interpolation(x3, y3, xi);
yi4 = two_point_interpolation(x4, y4, xi);



yyi1 = two_point_interpolation(x1, yy1, xi);
yyi2 = two_point_interpolation(x2, yy2, xi);
yyi3 = two_point_interpolation(x3, yy3, xi);
yyi4 = two_point_interpolation(x4, yy4, xi);




for i = 1:30
    res1(i,:) = [y1(i),y3(i),y4(i)];
    res2(i,:) = [yyi1(i),yyi3(i),yyi4(i)];
    
end
for i = 1:30
    %cy3(i) = (yi2(i) - yi1(i)) / 2000 * (rpm - 1000)  + yi1(i); 
    cy3(i) = cubicSpline(rpms, res1(i,:), rpm);
    cy4(i) = cubicSpline(rpms, res2(i,:), rpm);
    cy5(i) = cy3(i)  * xi(i) * 0.44704 / cy4(i);
end

figure(1); % 新建一个图形窗口
plot(bd(:,1), bd(:,a), 'ro','DisplayName', '实际结果'); % 绘制第一条曲线
hold on; % 保持当前图形
plot(xi, cy3, 'black', 'DisplayName', '拟合结果'); % 绘制第二条曲线
hold off; % 释放当前图形
xlabel('速度（mph）');
ylabel('效率');
title('效率曲线');
legend('show');
