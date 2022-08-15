/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i,j,k,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8;
    if(M == 32)
	{
		
		for(i = 0; i < N; i+=8)
		{
			for(j = 0; j < M; j+=8)
			{
				for(k = i; k <(i+8); k++)
				{
					tmp1 = A[k][j];
					tmp2 = A[k][j+1];
					tmp3 = A[k][j+2];
					tmp4 = A[k][j+3];
					tmp5 = A[k][j+4];
					tmp6 = A[k][j+5];
					tmp7 = A[k][j+6];
					tmp8 = A[k][j+7];

					B[j][k] = tmp1;
					B[j+1][k] = tmp2;
					B[j+2][k] = tmp3;
					B[j+3][k] = tmp4;
					B[j+4][k] = tmp5;
					B[j+5][k] = tmp6;
					B[j+6][k] = tmp7;
					B[j+7][k] = tmp8;
				}			
			}		
		}
	}
	else if(M == 64)
	{
		int m;
		for(i = 0; i < N; i+=8)
		{
			for(j = 0; j < M; j+=8)
			{
				for(k = i; k < i + 4; ++k)
				{
					tmp1 = A[k][j];
					tmp2 = A[k][j+1];
					tmp3 = A[k][j+2];
					tmp4 = A[k][j+3];

					tmp5 = A[k][j+4];
					tmp6 = A[k][j+5];
					tmp7 = A[k][j+6];
					tmp8 = A[k][j+7];	
	
					B[j][k] = tmp1;
					B[j+1][k] = tmp2;
					B[j+2][k] = tmp3;
					B[j+3][k] = tmp4;

					B[j][k+4] = tmp5;
					B[j+1][k+4] = tmp6;
					B[j+2][k+4] = tmp7;
					B[j+3][k+4] = tmp8;
			
				}
				for(m = j; m < j + 4; ++m)
				{
					tmp1 = A[i+4][m];
					tmp2 = A[i+5][m];
					tmp3 = A[i+6][m];
					tmp4 = A[i+7][m];
					
					tmp5 = B[m][i+4];
					tmp6 = B[m][i+5];
					tmp7 = B[m][i+6];
					tmp8 = B[m][i+7];
					
					B[m][i+4] = tmp1;
					B[m][i+5] = tmp2;
					B[m][i+6] = tmp3;
					B[m][i+7] = tmp4;

					B[m+4][i] = tmp5;
					B[m+4][i+1] = tmp6;
					B[m+4][i+2] = tmp7;
					B[m+4][i+3] = tmp8;
				}
				for(k = i + 4; k < i + 8; ++k)
				{
					tmp1 = A[k][j+4];
					tmp2 = A[k][j+5];
					tmp3 = A[k][j+6];
					tmp4 = A[k][j+7];

					B[j+4][k] = tmp1;
					B[j+5][k] = tmp2;
					B[j+6][k] = tmp3;
					B[j+7][k] = tmp4;
				}
			}		
		}
	}
	else{
		for(i = 0; i < N; i+=17)
		{
			for(j = 0; j < M; j+=17)
			{
				for (int x = i; x < N && x < i + 17; ++x)
                			for (int y = j; y < M && y < j + 17; ++y) B[y][x] = A[x][y];
			}		
		}
	}
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
//    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

