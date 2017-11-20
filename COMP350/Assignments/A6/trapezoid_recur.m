%evaluate erf using the recursive trapezoid rule
function trapezoid_recur(ftol,a,b)
%Calculate the first two differences I_2 - I_1 first, to get the first
%error
% then use a loop
%initial n
n = 0;
h = (b - a);
It = (1/2) *h* (erf_func(a) + erf_func(b));
count  = 0;
diff = ftol + 1; %so the loop executes once
count = count + 2 ; %first evaluation, evaluates at erf(a) and erf(b),so=2
while(diff > ftol)
    n = n + 1;
    h = h/2;
    temp = 0;% (1/2)*It;
    for i = 1:2^(n-1)
        temp = temp + erf_func(a+(2*i - 1)*h);
        count = count + 1;
    end
    It_new = temp*h + (1/2)*It;
    %It = temp;
    diff = abs(It_new - It);
    It = It_new;
end
fprintf('The number of evaluations is %d \n', count);
fprintf('The final result is: %f \n', It);
fprintf('There are 2^(%d) intervals\n', n);
fprintf('The final error is %.8f \n', diff);
end