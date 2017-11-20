%   adapt_simpson.m
function  numI  = adapt_simpson(f,a,b,epsilon,level,level_max,fa,fb,fc)
h  = b-a;
I1  = h*(fa+4*fc+fb)/6;
fprintf('fa %f\n',fa);
fprintf('fb %f\n',fb);
fprintf('fc %f\n',fc);
level  = level+1;
c  = (a+b)/2; d  = (a+c)/2; e  = (c+b)/2;
fd  = feval(f,d);
fe = feval(f,e);
I2  = h*(fa+4*fd+2*fc+4*fe+fb)/12;
global  count;
count  = count  + 2;
if level>=level_max 
    numI  = I2;
else

    if abs(I2-I1)<=15*epsilon 
    numI  = I2;

    else

    numI  = adapt_simpson(f,a,c,epsilon/2,level,level_max,fa,fc,fd)+adapt_simpson(f,c,b,epsilon/2,level,level_max,fc,fb,fe);

    end;
end;



