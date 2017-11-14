%Cubic spline method
function [S,z] = spline_interpolation(t,y, x2)
n = length(t);
len_k = length(x2);
% All starting indexes are shifted by 1 since there
% is no zero indexing in matlab
for i= 1:n-1
    h(i) = t(i+1) - t(i);
    b(i) = (y(i+1) - y(i))/h(i);
end
%Forward elimination
u(2) = 2*(h(2) + h(1));
v(2) = 6*(b(2) - b(1));
for i = 3:n-1
    u(i) = 2*(h(i-1) + h(i)) - ((h(i-1))^2) / u(i-1);
    v(i) = 6*(b(i) - b(i-1)) - (h(i-1)*v(i-1))/u(i-1);
end
%Back substitution
z(n) = 0;

for i = n-1:-1:2
    z(i) = (v(i) - h(i)*z(i+1))/u(i);
end
z(1) = 0;

for k = 1:length(x2)
    for i = 1:n-1
        if((x2(k) - t(i+1)) <= 0 )
            break;
        end
    end
    h2 = t(i+1) - t(i);
    B = -h2*z(i+1)/6 - h2*z(i)/3 + (y(i+1) - y(i))/h2;
    D = (1/(6*h2)) * (z(i+1) - z(i));
    S(k) = y(i) + (x2(k) - t(i))*(B+ (x2(k)-t(i))*(z(i)/2 + (x2(k) - t(i))*D));
end
end