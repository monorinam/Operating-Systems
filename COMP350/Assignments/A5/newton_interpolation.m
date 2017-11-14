function [y_new,ai] = newton_interpolation(x,y,x2)
ai = zeros(length(y),1);
n = length(y);
%Calculate the a_i's
for k = 1:n %since matlab does not use 0 indexing
    ai(k) = y(k);
    for i = k+1:n
        y(i) = (y(i)-y(k))/(x(i) - x(k));
    end
end
ai(n) = y(n);

%evaluate y using the indexes
y_new(1:length(x2)) = ai(n);

for j = 1:length(x2)
    for m = n-1:-1:1
        y_new(j) = y_new(j)*(x2(j)-x(m))+ai(m);
    end
end
end