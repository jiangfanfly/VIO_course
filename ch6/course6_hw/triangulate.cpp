//
// Created by hyj on 18-11-11.
//
#include <iostream>
#include <vector>
#include <random>  
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>

struct Pose
{
    Pose(Eigen::Matrix3d R, Eigen::Vector3d t):Rwc(R),qwc(R),twc(t) {};
    Eigen::Matrix3d Rwc;
    Eigen::Quaterniond qwc;
    Eigen::Vector3d twc;

    Eigen::Vector2d uv;    // 这帧图像观测到的特征坐标
};
int main()
{

    int poseNums = 10;
    double radius = 8;
    double fx = 1.;
    double fy = 1.;
    std::vector<Pose> camera_pose;
    for(int n = 0; n < poseNums; ++n ) {
        double theta = n * 2 * M_PI / ( poseNums * 4); // 1/4 圆弧
        // 绕 z轴 旋转
        Eigen::Matrix3d R;
        R = Eigen::AngleAxisd(theta, Eigen::Vector3d::UnitZ());
        Eigen::Vector3d t = Eigen::Vector3d(radius * cos(theta) - radius, radius * sin(theta), 1 * sin(2 * theta));
        camera_pose.push_back(Pose(R,t));
    }

    // 随机数生成 1 个 三维特征点
    std::default_random_engine generator;
    std::uniform_real_distribution<double> xy_rand(-4, 4.0);
    std::uniform_real_distribution<double> z_rand(8., 10.);
    double tx = xy_rand(generator);
    double ty = xy_rand(generator);
    double tz = z_rand(generator);

    Eigen::Vector3d Pw(tx, ty, tz);
    // 这个特征从第三帧相机开始被观测，i=3
    int start_frame_id = 3;
    int end_frame_id = poseNums;
    for (int i = start_frame_id; i < end_frame_id; ++i) {
        Eigen::Matrix3d Rcw = camera_pose[i].Rwc.transpose();
        Eigen::Vector3d Pc = Rcw * (Pw - camera_pose[i].twc);

        double x = Pc.x();
        double y = Pc.y();
        double z = Pc.z();

        camera_pose[i].uv = Eigen::Vector2d(x/z,y/z);  //归一化相机坐标
    }
    
    /// TODO::homework; 请完成三角化估计深度的代码
    // 遍历所有的观测数据，并三角化
    Eigen::Vector3d P_est;           // 结果保存到这个变量
    P_est.setZero();
    /* your code begin */
    const int dim = 2*(end_frame_id - start_frame_id);
    Eigen::MatrixXd D(Eigen::MatrixXd::Zero(dim, 4));
    for(int i = start_frame_id;i < poseNums; i++)
    {
        Eigen::Matrix4d T = Eigen::Matrix4d::Identity();
        T.block(0, 0, 3, 3) = camera_pose[i].Rwc.transpose();
        T.block(0, 3, 3, 1) = -camera_pose[i].Rwc.transpose()*camera_pose[i].twc;
        
        int start_row = i - start_frame_id;
        D.block(2*start_row, 0, 1, 4) = camera_pose[i].uv(0)*T.row(2) - T.row(0);
        D.block(2*start_row+1, 0, 1, 4) = camera_pose[i].uv(1)*T.row(2) - T.row(1);
    }
    //D^TD特征值分解
    Eigen::Matrix4d T= D.transpose()*D;
    Eigen::EigenSolver<Eigen::Matrix4d> es(T);
    Eigen::Matrix4cd evector = es.eigenvectors();
    Eigen::Vector4cd evalue = es.eigenvalues();
    std::cout<<"------特征值分解----------"<<std::endl<<"evector:"<<evector<<std::endl<<"evalue"<<evalue<<std::endl;

    //D^TD SVD 分解
    Eigen::MatrixXd A = D.transpose()*D;
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeFullU | Eigen::ComputeFullV); 
    Eigen::MatrixXd U,V,sigma;
    U = svd.matrixU();
    V = svd.matrixV();
    sigma = svd.singularValues();
    std::cout <<"------奇异值分解----------"<< "U:"<< U <<std::endl<<"V:"<< V<<std::endl<< "singularValues:"<< sigma<<std::endl;
    if(sigma(3)/sigma(2) > 1e-3)
        std::cout<<"三角化失败！"<<std::endl;

    Eigen::Vector4d u4 = V.col(3);
    P_est = u4.head(3)/u4(3);
    /* your code end */
    
    std::cout <<"ground truth: \n"<< Pw.transpose() <<std::endl;
    std::cout <<"your result: \n"<< P_est.transpose() <<std::endl;
    return 0;
}
