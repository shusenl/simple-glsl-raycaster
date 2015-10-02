#include<cmath>
#include<cstdio> 

extern "C"  float** jet(int m)   
{   
    int i=0;   
    //n = ceil(m/4);   
    int n = int(ceil(m/4.0));   
    //u = [(1:1:n)/n ones(1,n-1) (n:-1:1)/n]';   
    int u_len = n*3-1;   
    float* u = new float[u_len];   
    for(i=0;i<n;i++){   
        u[u_len-i-1] =  u[i] = float(i+1)/n;   
        u[n+i] = 1;   
    }   

	//for(int ii=0; ii<n ; ii++)
	//{
	//	printf("%f, count: %d\n", u[ii], ii);
	//}

    //J = zeros(m,3);   
    float** J = new float*[m];   
    for(i=0;i<m;i++){   
        J[i] = new float[3];   
        for(int j=0;j<3;j++){   
            J[i][j] = 0;   
        }   
    }   

 //   for(i=0;i<m;i++)
	//{   
	//	printf("<%05lf, %05lf, %05lf> count:%d\n",J[i][0], J[i][1], J[i][2],i);
 //   }   

    int t = int(ceil(n/2.0) - (m % 4 == 1));   
    int g_len = m-t;   
    for(i=0;i<g_len;i++){   
        J[t+i][1] = u[i];   
    }   
    int r_len = m-t-n;   
    for(i=0;i<r_len;i++){   
        J[t+i+n][0] = u[i];   
    }   
    int b_start = n-t;   
    for(i=b_start;i<u_len;i++){   
        J[t+i-n][2] = u[i];   
    }   
    delete[] u;   
    return J;   
}   