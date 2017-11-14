function  c350A5



min  = -1;
max  = 1;
range  = max-min;
numKnots  = 7;
incr  = range/(numKnots-1);


knots  = zeros(numKnots,1);


for  i = 1:numKnots
knots(i)  = cos(pi*(2*(i-1) + 1)/(12+2));
end


x  = knots;
y  = f(x);


a  = interpNewton(x,y); c  = leastSquares2(x,y); z  = mySpline(x,y);

X  = -1:0.01:1.0; Yf  = f(X);
Yn  = evalNewt(a,x,X);
Yspl  = evalSpline(X,x,y,z); Yls  = evalLS(c,X);
hold  on;


xlabel('x')
ylabel('y')
%title(?Graph  of  various  interpolations?)
plot(X,Yf);
plot(X,Yn,'-','Color',  'r'); plot(X,Yspl,'-','Color',  'm'); plot(X,Yls,'-','Color',  'g');
%legend(?y=  f(x)?,?y=p(x)?,  ?y=S(x)?,  ?y=g(x)?,  ?Location?,?Best?);
hold  off;


min  = -1;
max  = 1;
range  = max-min;
numKnots  = 17;
incr  = range/(numKnots-1);


knots  = zeros(numKnots,1);
for  i = 1:numKnots
knots(i)  = -1+(i-1)*incr;
end


%display(sprintf(?\n$x$  &   $f(x)$  &   $f(x)-p(x)$  &   $f(x)-S(x)$  &   $f(x)-g(x)\\\\?));
for (i=1:numKnots) x2 = knots(i); fx  = f(x2);
px  = fx-evalNewt(a,x,x2);
sx = fx-evalSpline(x2,x,y,z);
gx  = fx-evalLS(c,x2);
display(sprintf('$%9f$  &   $%9f$  &   $%9f$  &   $%9f$  &   $%9f$\\\\',x2,fx,px,sx,gx));
end
end









