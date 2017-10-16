%% This is the tester for GENP and GEPP diagonal methods
%% First generate matrices

n = 9;
A = zeros(2*n+1);
for i = 1:(2*n+1)
    %generate d_i's
    if(i>1)
        A(i,i) = randn(1); % generate the left diagonal element
    else
        A(1,1) = 10^-15;
    end
    
    if(i ~= n+1)
        %generate a_i's
        if(i == 2*n+1)
            A(2*n+1,1) = 10^-8;
        else
            A(i, 2*n+2 - i) = randn(1);
        end
        %find b_i's
        b(i) = A(i,i) + A(i,2*n+2-i);
    else
        b(i) = A(i,i);
    end
    
end

%test
x_np = genpdiag(A,b);
x_pp = geppdiag(A,b);
x = A\b';
%Calculate the norms
xnp_norm = norm(x_np-x)/norm(x);
npp_norm = norm(x_pp-x)/norm(x);
epsilon  = eps('double')*cond(A,2);
fprintf('xnp norm %ld xpp_norm %ld epsilon %ld \n',xnp_norm,xpp_norm,epsilon);