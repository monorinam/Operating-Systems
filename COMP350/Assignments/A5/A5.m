%This calls and plots the newton_interpolation,spline_interpolation and
%leastsquare.m functions 



% Array of knots
knots = -1:(2/6):1;
knots = knots';
y = f(knots);
%Evaluation points
x2 = -1:0.01:1;
y2 = f(x2);

figure(1);
hold on;
plot(x2,y2);
%Newton Interpolation
[y_newt,ai] = newton_interpolation(knots,y,x2);
plot(x2,y_newt,'-g');
%Spline Interpolation
[S,z] = spline_interpolation(knots,y,x2);
plot(x2,S,'-r');
[c,gx] = leastsquares(knots,y,x2);
plot(x2,gx,'-c');

xlabel('x');
ylabel('y');

[h,~]=legend('f(x)','p(x)','S(x)','g(x)');
%Print the results
fprintf('Coefficients of p(x) \n');
fprintf(' %10.5f \n',ai);
fprintf('Coefficients of S(x) \n');
fprintf(' %10.5f \n',z);
fprintf('Coefficients of g(x) \n');
fprintf('%10.5f \n',c);

%New nodes
knots2 = -1:(2/12):1;
knots2 = knots2';
y2_p = f(knots2);
fprintf('Num.\t     x \t             f(x) \t    f(x) - p(x)    f(x) - S(x)    f(x) - g(x) \n');
for i = 1:13
    fx = f(knots2(i));
    [px,a2] = newton_interpolation(knots,y,knots2(i));
    px = fx - px;
    [Sx,z2] = spline_interpolation(knots,y,knots2(i));
    Sx = fx - Sx;
    [c2,gx] = leastsquares(knots,y,knots2(i));
    gx = fx - gx;
    fprintf('%d\t%10.5f\t %10.5f\t %10.5f\t %10.5f\t %10.5f\t \n',i,knots2(i),fx,px,Sx,gx);
end

    
