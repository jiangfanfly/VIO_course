mudata=load('/home/jiangfan/vio_course/ch3/assisignment3/mu2_710.txt');
it=mudata(:,1);
mu=mudata(:,2);
% plot(it,mu,'r+')
semilogy(it,mu,'r+');
grid on
hold on
xlabel('迭代次数')
ylabel('值')
legend('阻尼因子')