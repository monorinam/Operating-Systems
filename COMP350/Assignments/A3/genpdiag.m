function x = genpdiag(A,b)
% genpdiag.m Gauss elimination with no pivoting, for a matrix with only
%            left and right diagonal elements.
% 
% input:  A is an n x n nonsingular matrix
%         b is an n x 1 vector
% output: x is the solution of Ax=b.
%
%% This function assumes that the matrix is of type specified in Q2-cswk3.pdf
if(mod(length(b)-1,2) == 1)
    print('This is not the right function for this matrix type!');
end
len = length(b);
n = (len-1)/2;


for k = 1:n % the diagonal entry has zeros below it due to the form of A 
  mult =  A(len-k+1,k)/A(k,k);
  % Only two entries of each row need updating since the rest are all
  % zeros.
  A(len-k+1,k) = A(len-k+1,k)-mult*A(k,k);
  A(len-k+1,len-k+1) = A(len-k+1,len-k+1) - mult*A(k,len-k+1);
  b(len-k+1) = b(len-k+1)- mult*b(k);
end

x = zeros(len,1);
for k = len:-1:1  
  %Below the diagonal
  if(k > n)
      x(k) = b(k)/A(k,k);
  else
    x(k) = (b(k)-(A(k,len-k+1)*x(len-k+1)))/A(k,k);
  end
end