#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

int main()
{
    Eigen::Matrix3d R0=Eigen::AngleAxisd(M_PI/4,Eigen::Vector3d(1,0,0)).toRotationMatrix()
                    *Eigen::AngleAxisd(M_PI/4,Eigen::Vector3d(0,1,0)).toRotationMatrix()
                    *Eigen::AngleAxisd(M_PI/4,Eigen::Vector3d(0,0,1)).toRotationMatrix();
    cout<<"original R:"<<endl<<R0<<endl<<endl;
    Eigen::Vector3d omiga;
    omiga<<0.01,0.02,0.03;

    // update rotation matrix
    // compute exp(w_hat)
    double theta=sqrt(omiga(0)*omiga(0)+omiga(1)*omiga(1)+omiga(2)*omiga(2));
    Eigen::AngleAxisd w(theta,omiga/theta);
    Eigen::Matrix3d deltaR=w.toRotationMatrix();
    // Eigen::Matrix3d w_hat;
    // w_hat<<0,-omiga(2),omiga(1),
    //         omiga(2),0,-omiga(0),
    //         -omiga(1),omiga(0),0;
    // Eigen::Matrix3d R_w;
    // cout<<"R_w:"<<w_hat.exp()<<endl;
    // cout<<"w_R:"<<deltaR<<endl;
    Eigen::Matrix3d Rnew;
    Rnew=R0*deltaR;
    cout<<"R uptaded from R=R*exp(w^):"<<endl<<Rnew<<endl;

    cout<<endl;

    // update quanternion
    Eigen::Quaterniond q(R0);
    Eigen::Quaterniond deltaq(1,0.5*omiga(0),0.5*omiga(1),0.5*omiga(2));
    //cout<<"deltaq:"<<deltaq.coeffs()<<endl;
    deltaq=deltaq.normalized();
    //cout<<"deltaq:"<<deltaq.coeffs()<<endl;
    //cout<<"q_R:"<<deltaq.toRotationMatrix()<<endl;
    Eigen::Quaterniond qnew;
    qnew=q*deltaq;
    qnew.normalized();
    cout<<"R uptaded from q=q x [1,1/2*w]:"<<endl<<qnew.toRotationMatrix()<<endl;
    
    return 0;
}
