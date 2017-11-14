
%%%%   CUBIC  SPLINE  METHOD  %%%%
function  z  = mySpline(t,y)


n  = length(t);
h  = zeros(n,1);
b  = zeros(n,1); u  = zeros(n,1); v  = zeros(n,1); z  = zeros(n,1);

for  i = 1  : n-1
h(i)  = t(i+1)-t(i)  ;
b(i)  = (y(i+1)-y(i))/h(i);
end


u(2)  = 2*(h(1)+h(2));
v(2)  = 6*(b(2)-b(1));


for  i = 3:n-1
u(i)  = 2*(h(i-1)+h(i))-h(i-1)^2/u(i-1);
v(i)  = 6*(b(i)-b(i-1))-h(i-1)*v(i-1)/u(i-1);
end


z(n)  = 0;


for  i = n-1:-1:2
z(i)  = (v(i)-h(i)*z(i+1))/u(i);
end


z(1)  = 0;


end

