global  count;
a  = 0;
b  = 1;
fa = feval('erf_func',a);

c = (a+b)/2;
fb = feval('erf_func',b); fc = feval('erf_func',c); 
count  = 3;
adapt_simpson('erf_func',a,b,10^-6,1,100,fa,fb,fc);
count


