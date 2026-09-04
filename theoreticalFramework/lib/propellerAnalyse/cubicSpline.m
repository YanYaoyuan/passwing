function yq = cubicSpline(x, y, xq)
    % 检查输入
    if length(x) ~= length(y) || length(x) < 2
        error('x 和 y 必须具有相同的长度，并且长度至少为 2');
    end
    n = length(x);
    
    % 计算 h
    h = diff(x);
    
    % 计算 a, b, c, d 系数
    a = y;
    b = zeros(n, 1);
    c = zeros(n, 1);
    d = zeros(n, 1);
    
    % 构建矩阵 A 和向量 B
    A = zeros(n, n);
    B = zeros(n, 1);
    
    A(1, 1) = 1;
    A(n, n) = 1;
    
    for i = 2:n-1
        A(i, i-1) = h(i-1);
        A(i, i) = 2 * (h(i-1) + h(i));
        A(i, i+1) = h(i);
        B(i) = 3 * ((y(i+1) - y(i)) / h(i) - (y(i) - y(i-1)) / h(i-1));
    end
    
    % 解线性方程组
    c = A \ B;
    
    % 计算 b 和 d
    for i = 1:n-1
        b(i) = (y(i+1) - y(i)) / h(i) - h(i) * (2 * c(i) + c(i+1)) / 3;
        d(i) = (c(i+1) - c(i)) / (3 * h(i));
    end
    
    % 插值计算
    yq = zeros(size(xq));
    for k = 1:length(xq)
        % 找到 xq 所在的区间
        i = find(x <= xq(k), 1, 'last');
        if i == length(x)
            i = length(x) - 1;
        end
        
        % 计算插值
        dx = xq(k) - x(i);
        yq(k) = a(i) + b(i) * dx + c(i) * dx^2 + d(i) * dx^3;
    end
end
