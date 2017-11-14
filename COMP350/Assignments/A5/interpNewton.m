%%%%   NEWTON  APPROACH  %%%% 
function  a  = interpNewton(x,y) 
n  = length(y);
a  = zeros(n,1);


for  k  = 1  :n 
a(k)  = y(k);
for  i = k+1 : n
y(i)  = (y(i)-y(k))/(x(i)-x(k));


end
end
a(n)  = y(n);
end