function num_count_array = adaptive_simpson(a,b,c,fa, fb, fc,ftol,level,level_max, count)

h = b - a;
%c = (a+b)/2;
I1 = h*(fa + 4*fc + fb)/6;
level = level + 1;
d = (a + c)/2;
e = (c + b)/2;
I2 = h*(fa + 4*erf_func(d) + 2*fc+ 4*erf_func(e)+ fb)/12;

count  = count + 2;
if (level >= level_max)
    numI = I2;
    num_count_array = [numI,count];
else
    if(abs(I2 - I1) <= 15*ftol)
        numI = I2 + (1/15)*(I2 - I1);
        num_count_array = [numI,count];
    else
        numArr1 = adaptive_simpson(a,c,d,fa, fc, erf_func(d),ftol/2,level,level_max, count) ;
        numArr2 = adaptive_simpson(c,b,e,fc,fb,erf_func(e),ftol,level,level_max, numArr1(2));
        numI = numArr1(1) + numArr2(1);
        count = numArr2(2); %+ numArr2(1);
        num_count_array = [numI,count];
    end
end
end