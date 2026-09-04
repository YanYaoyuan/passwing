function [BMatrix] = BuildBernstein(OrDer,Xdata,Zdata,Yte)
Order = OrDer(1);
N1 = OrDer(2);
N2 = OrDer(3);
AMatrix = zeros(length(Xdata),Order);
for i = 1:length(Xdata)
    for j = 1:Order+1
        AMatrix(i,j) =power(Xdata(i),N1) * power(1 - Xdata(i),N2) * nchoosek(Order,j-1) * power(1 - Xdata(i), Order+1-j) * power(Xdata(i), j-1);
        
    end 
    Zdata(i) = Zdata(i) - Xdata(i)*Yte;
end
BMatrix = lsqr(AMatrix,Zdata);



end