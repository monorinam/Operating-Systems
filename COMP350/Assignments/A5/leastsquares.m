function [c,y2] = leastsquares(x,y,x2)
n = length(x);
A = ones(n,3);

%Allocate A
A(1:n,2) = x.^2;
A(1:n,3) = x.^4;
%Find the c's
c = A\y;
%Calculate the y interpolation
y2 = c(1) + c(2) *x2.^2 + c(3)*x2.^4;
end