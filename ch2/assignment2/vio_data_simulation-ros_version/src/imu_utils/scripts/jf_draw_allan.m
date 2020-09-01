clear 
close all

dt = dlmread('../data/data_simulation_imu2_gyr_t.txt');         
data_x = dlmread('../data/data_simulation_imu2_gyr_x.txt'); 
data_y= dlmread('../data/data_simulation_imu2_gyr_y.txt'); 
data_z = dlmread('../data/data_simulation_imu2_gyr_z.txt'); 
data_drawavg=(data_x +data_y +data_z)/3/3600*(2.*3.141593653)/360;
data_draw=[data_x data_y data_z]/3600*(2.*3.141593653)/360 ;

data_sim_x= dlmread('../data/data_simulation_imu_sim_gyr_x.txt'); 
data_sim_y= dlmread('../data/data_simulation_imu_sim_gyr_y.txt'); 
data_sim_z= dlmread('../data/data_simulation_imu_sim_gyr_z.txt'); 
%data_sim_draw=(data_sim_x+ data_sim_y+ data_sim_z)/3/3600;
data_sim_draw=[data_sim_x data_sim_y data_sim_z]/3600*(2*3.141593653)/360 ;

figure(1)
loglog(dt, data_draw(:,1) , 'r+');
hold on;  
loglog(dt, data_draw(:,2) , 'b+');
hold on;  
loglog(dt, data_draw(:,3) , 'k+');
hold on 
loglog(dt, data_drawavg , '-');
legend('w_x','w_y','w_z','w_{avg}')
mix_where=find(dt==1);
num=dt(mix_where);
%text(dt(num),data_drawavg(num),num2str(data_drawavg(num)))
xlabel('time:sec');                
ylabel('Sigma:deg/h');   
grid on;                           
hold on;  
%loglog(dt, data_sim_draw , 'r-');



dt = dlmread('../data/data_simulation_imu_acc_t.txt');         
data_x = dlmread('../data/data_simulation_imu2_acc_x.txt'); 
data_y = dlmread('../data/data_simulation_imu2_acc_y.txt'); 
data_z = dlmread('../data/data_simulation_imu2_acc_z.txt'); 
data_draw=[data_x data_y data_z] ;
data_draw_avg=(data_x +data_y +data_z)/3 ;
data_sim_x= dlmread('../data/data_simulation_imu2_sim_acc_x.txt'); 
data_sim_y= dlmread('../data/data_simulation_imu2_sim_acc_y.txt'); 
data_sim_z= dlmread('../data/data_simulation_imu2_sim_acc_z.txt'); 
data_sim_draw=[data_sim_x data_sim_y data_sim_z] ;
figure(2)
loglog(dt, data_draw(:,1) , 'r+');
hold on
loglog(dt, data_draw(:,2) , 'b+');
hold on
loglog(dt, data_draw(:,3) , 'k+');
hold on
loglog(dt, data_draw_avg, '-');
legend('acc_x','acc_y','acc_z','acc_{avg}')
ylabel('Sigma:m/s2');   
xlabel('time:sec');   
hold on
% loglog(dt, data_sim_draw , 'b-');
grid on;