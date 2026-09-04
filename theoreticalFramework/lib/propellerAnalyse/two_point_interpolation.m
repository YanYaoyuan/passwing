function yi = two_point_interpolation(x, y, xi)
% TWO_POINT_INTERPOLATION 使用两点线性插值法进行插值，并处理超出边界的情况
%
% 输入:
%   x  - 原始数据点的横坐标 (向量)
%   y  - 原始数据点的纵坐标 (向量)
%   xi - 要插值的点 (向量)
%
% 输出:
%   yi - 插值结果 (向量)

% 确保 x 和 y 是列向量
x = x(:);
y = y(:);

% 检查输入的有效性
if length(x) < 2 || length(y) < 2 || length(x) ~= length(y)
    error('原始数据点的数量必须至少为2，并且 x 和 y 的长度必须相等。');
end

% 预分配输出数组
yi = zeros(size(xi));

% 插值计算
for i = 1:length(xi)
    if xi(i) < x(1)
        % 插值点在最左边界外，使用最左两点进行插值
        x0 = x(1);
        x1 = x(2);
        y0 = y(1);
        y1 = y(2);
        yi(i) = y0 + (xi(i) - x0) * (y1 - y0) / (x1 - x0);
    elseif xi(i) > x(end)
        % 插值点在最右边界外，使用最右两点进行插值
        x0 = x(end-1);
        x1 = x(end);
        y0 = y(end-1);
        y1 = y(end);
        yi(i) = y0 + (xi(i) - x0) * (y1 - y0) / (x1 - x0);
    else
        % 插值点在数据点范围内，进行线性插值
        for j = 1:length(x)-1
            if xi(i) >= x(j) && xi(i) <= x(j+1)
                x0 = x(j);
                x1 = x(j+1);
                y0 = y(j);
                y1 = y(j+1);
                yi(i) = y0 + (xi(i) - x0) * (y1 - y0) / (x1 - x0);
                break;
            end
        end
    end

end


end