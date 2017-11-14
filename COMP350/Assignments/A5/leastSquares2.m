%%%% LEAST SQUARES METHOD %%%% 
function  c  = leastSquares2(x,y)
n  = length(x);
A  = ones(n,3);


for  (i = 1:n)
A(i,2)  = x(i)^2; A(i,3)  = x(i)^4;
end


c  = A\y ;
end