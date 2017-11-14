function  y  = evalNewt(a,x,X)
n  = length(a);
m   = length(X);
y  = a(n)*ones(m,1);


for  j=1:m
for  (i = n-1:-1:1)
y(j)  = y(j)*(X(j)-x(i))+a(i);


end
end
end