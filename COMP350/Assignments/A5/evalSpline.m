function  S = evalSpline(X,t,y,z)
n  = length(z);
m   = length(X);
S = zeros(m,1);
for  j = 1:m


for  i = 1:n-1
if (X(j)  - t(i+1))  <= 0 
    break;

end
end


x  = X(j);
h  = t(i+1)-t(i);
B  = -h*z(i+1)/6-h*z(i)/3+(y(i+1)-y(i))/h; D  = (z(i+1)-z(i))/(6*h);
S(j)  = y(i)+(x-t(i))*(B+(x-t(i))*(z(i)/2+(x-t(i))*D));





end
end