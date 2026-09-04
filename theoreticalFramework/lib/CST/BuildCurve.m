function [Ydata] = BuildCurve(cst,OrDer,Xdata,Yte)
Order = OrDer(1);
N1 = OrDer(2);
N2 = OrDer(3);
yi = 0;

for i = 1:length(Xdata)
    for j = 1:Order+1
        yi = cst(j)*power(Xdata(i),N1) * power(1 - Xdata(i),N2) * nchoosek(Order,j-1) * power((1 - Xdata(i)), Order+1-j) * power(Xdata(i), j-1)+yi;
    end
    Ydata(i) = yi + Xdata(i) * Yte;
    yi = 0;
end

end