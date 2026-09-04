V=100/3.6;
omega=50*pi;
rho=0.7;%空气密度
n=2;
R=0.7;
Rh=0.06;
r=Rh:((R-Rh)/1000):R; %用来计算 K
T=120;%推力
Nj=20; %展向分段数
%% 计算 K
kp=2/pi*acos(exp(-n/2*(1-r./R)*(1+(omega*R/V)^2)^0.5));%Kp的公式
syms K
K1=K./(1+(V./(omega*r)).^2*(1+K)^2);%K1的公式
func=(K1+K1.^2).*kp.*r;%微分表达式
sum1=func*ones(length(r),1)*(r(2)-r(1));%积分
sum2=T/(rho*4*pi*V^2);
kk1=0;
kk2=1;
while abs(kk1-kk2)>0.0000001
kk=(kk1+kk2)/2;
if sum2-vpa(subs(sum1,K,kk))>0
kk1=kk;
else
kk2=kk;
end
end
K=kk