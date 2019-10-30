#include "magnet.h"

#define MATRIX_SIZE 			7

double m_matrix[MATRIX_SIZE][MATRIX_SIZE+1];
int m = MATRIX_SIZE;	
int n = MATRIX_SIZE+1;
double m_result[MATRIX_SIZE];	

void DispMatrix(void);

double MySqrt(double data)
{
	return data * data;
}

double Abs(double a)
{
	return a<0 ? -a : a;
}

u8 Equal(double a,double b)
{
	return Abs(a-b) < 1e-6;
}

void ResetMatrix(void)
{
	int row , column;
	
	for(row = 0 ; row<m ; row++){
		for(column = 0 ; column<n ; column++)
			m_matrix[row][column] = 0.0f;
	}
}
	
void CalcData_Input(double x , double y , double z)
{
	double V[MATRIX_SIZE];
	int row , column;
	
	V[0] = x*x;
	V[1] = y*y;
	V[2] = z*z;
	V[3] = x;
	V[4] = y;
	V[5] = z;
	V[6] = 1.0;
	
	//����VxVt����(VtΪV��ת��)���������ۼ�
	for(row = 0 ; row<MATRIX_SIZE ; row++){
		for(column = 0 ; column<MATRIX_SIZE ; column++){
			m_matrix[row][column] += V[row]*V[column];
		}
	}
}

void SwapRow(int row1 , int row2)
{
	int column;
	double tmp;
	
	for(column = 0 ; column<n ; column++){
		tmp = m_matrix[row1][column];
		m_matrix[row1][column] = m_matrix[row2][column];
		m_matrix[row2][column] = tmp;
	}
}

void MoveBiggestElement2Top(int s_row , int s_column)
{
	int row,column;
	
	for(row = s_row+1 ; row<m ; row++){
		if( Abs(m_matrix[s_row][s_column])<Abs(m_matrix[row][s_column])){
			SwapRow(s_row , row);
		}
	}
}

//��˹��Ԫ�������н����;���
u8 Matrix_GaussElimination(void)
{
	int row,column,i,j;
	double tmp;
	
	for(row = 0,column=0 ; row<m-1 && column<n-1 ; row++,column++){
		//����ǰ������һ��������
		MoveBiggestElement2Top(row , column);
		
		//���ж�Ϊ0
		if(Equal(m_matrix[row][column],0.0f)){
			printf("qiyi matrix:%d %d\r\n" , row , column);
			//DispMatrix();
			//return 0;
			row--;
			continue;
		}
		
		//��˹��Ԫ
		for(i = row+1 ; i<m ; i++){	
			if(Equal(m_matrix[i][column],0.0f))
				continue;	//Ϊ0�����账��
			
			tmp = m_matrix[i][column]/m_matrix[row][column];
			
			for(j = column ; j<n ; j++){
				m_matrix[i][j] -= m_matrix[row][j]*tmp;
			}
		}

		DispMatrix();
		printf("\r\n");
	}

	return 1;
}

//��������;���
int Matrix_RowSimplify(void)
{
    int c = n;//����ֵ����ʾ(������ⳣ����+1)��
    //
    int row,column,k,s,t;
    double tmp;
    //
    for(row=0,column=0;row<m && column<n;row++,column++)
    {
        if(Equal(m_matrix[row][column],0))//ƽ�ƣ��ҳ����е�һ�����㣻
        {
            row--;
            continue;
        }
        //
        c--;//��һ��������
        //
        //��a[i][j]Ϊ1��
        tmp = 1 / m_matrix[row][column];
        for(k=column;k<n;k++)//ǰ���"0"�Ͳ������ˣ�
            m_matrix[row][k] *= tmp;
        //
        //��a[s][j]Ϊ0
        for(s=0;s<row;s++)//�����0Ҳ���ô���
        {
            if(Equal(m_matrix[s][column],0))
                continue;//�Ѿ�Ϊ0��
            //
            tmp = m_matrix[s][column] / m_matrix[row][column];
            for(t=column;t<n;t++)
                m_matrix[s][t] -= m_matrix[row][t]*tmp;
            //
        }
    }
    //
    return c;
}

void Matrix_Solve(double* C , double* sol)
{
	int row,column,i;
	int any_sol[MATRIX_SIZE];

	//�ҳ�������λ��
	memset(any_sol , 0 , MATRIX_SIZE);
	for(row=0,column=0 ; row<m && column<n-1 ; row++,column++){
		if(Equal(m_matrix[row][column] , 0.0f)){
			any_sol[column] = 1;	//��¼������λ��
			row--;	//����1��
		}
	}

	//���
	row = 0;
	for(column = 0 ; column<n-1 ; column++){
		if(any_sol[column] == 1){	//�����
			sol[column] = C[column];
		}else{
			sol[column] = m_matrix[row][n-1];
			//���������
			for(i = column+1 ; i<n-1 ; i++){
				if(any_sol[i]==1 && !Equal(m_matrix[row][i],0.0f)){
					sol[column] -= m_matrix[row][i]*C[i];
				}
			}	
			row++;
		}
	}
}


void DispMatrix(void)
{
	int row,column;
	
	for(row = 0 ; row<m ; row++){
		for(column = 0 ; column<n ; column++){
			printf("%.3f	" , m_matrix[row][column]);
		}
		printf("\r\n");
	}
}

void Calc_Process(double radius)
{
	double C[MATRIX_SIZE];
	double Res[MATRIX_SIZE];
	int i;
	double k;

	ResetMatrix();

	//������������ų����������꣬�뾡����֤�������Ϸֲ�����
	CalcData_Input(7 , -7 , -2);
	CalcData_Input(-1 , -7 , -2);
	CalcData_Input(3 , 3 , -2);
	CalcData_Input(3 , -17 , -2);
	CalcData_Input(3 , -7 , 4);
	CalcData_Input(3 , -7 , -8);

	Matrix_GaussElimination();
	Matrix_RowSimplify();

    //��ֵ��������ֵ
	for(i = 0 ; i<MATRIX_SIZE ; i++){
		C[i] = 1.0f;
	}

	Matrix_Solve(C , Res);

	printf("a:%.2f b:%.2f c:%.2f d:%.2f e:%.2f f:%.2f g:%.2f\r\n" , Res[0],Res[1],Res[2],Res[3],Res[4],Res[5],Res[6]);

	k = (Res[3]*Res[3]/Res[0]+Res[4]*Res[4]/Res[1]+Res[5]*Res[5]/Res[2] - 4*Res[6])/(4*radius*radius);

//	m_result[0] = sqrt(Res[0] / k);
//	m_result[1] = sqrt(Res[1] / k);
//	m_result[2] = sqrt(Res[2] / k);
	m_result[0] = MySqrt(Res[0] / k);
	m_result[1] = MySqrt(Res[1] / k);
	m_result[2] = MySqrt(Res[2] / k);
	
	m_result[3] = Res[3] / (2 * Res[0]);
	m_result[4] = Res[4] / (2 * Res[1]);
	m_result[5] = Res[5] / (2 * Res[2]);

	printf("Xo:%f Yo:%f Zo:%f Xg:%f Yg:%f Zg:%f k:%f\r\n" , m_result[3],m_result[4],m_result[5],m_result[0],m_result[1],m_result[2],k);
}


