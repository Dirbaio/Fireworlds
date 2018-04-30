#include<cstdlib>
#include<cmath>

#define SIZE ((int)128)

int p [SIZE];
float gx [SIZE];
float gy [SIZE];

void perlin_init()
{
    int i, j, nSwap;
    
    // Initialize the permutation table
    for(i = 0; i < SIZE; i++)
        p[i] = i;
    for(i = 0; i < SIZE; i++)
    {
        j = rand() % SIZE;
        nSwap = p[i];
        p[i]  = p[j];
        p[j]  = nSwap;
    }
    // Generate the gradient look-up tables
    for(i = 0; i < SIZE; i++)
    {
        gx[i] = float(rand())/(RAND_MAX/2) - 1.0f; 
        gy[i] = float(rand())/(RAND_MAX/2) - 1.0f;
    }
}

float perlin_noise(float x, float y)
{
    // Compute the integer positions of the four surrounding points
    int qx0 = (int)floorf(x);
    int qx1 = qx0 + 1;
    int qy0 = (int)floorf(y);
    int qy1 = qy0 + 1;
    // Permutate values to get indices to use with the gradient look-up tables
    int q00 = p[(qy0 + p[qx0 % SIZE]) % SIZE];
    int q01 = p[(qy0 + p[qx1 % SIZE]) % SIZE];
    int q10 = p[(qy1 + p[qx0 % SIZE]) % SIZE];
    int q11 = p[(qy1 + p[qx1 % SIZE]) % SIZE];
    
    // Computing vectors from the four points to the input point
    float tx0 = x - floorf(x);
    float tx1 = tx0 - 1;
    float ty0 = y - floorf(y);
    float ty1 = ty0 - 1;
    // Compute the dot-product between the vectors and the gradients
    float v00 = gx[q00]*tx0 + gy[q00]*ty0;
    float v01 = gx[q01]*tx1 + gy[q01]*ty0;
    float v10 = gx[q10]*tx0 + gy[q10]*ty1;
    float v11 = gx[q11]*tx1 + gy[q11]*ty1;
    
    
    // Do the bi-cubic interpolation to get the final value
    float wx = (3 - 2*tx0)*tx0*tx0;
    float v0 = v00 - wx*(v00 - v01);
    float v1 = v10 - wx*(v10 - v11);
    float wy = (3 - 2*ty0)*ty0*ty0;
    float v = v0 - wy*(v0 - v1);


    return v;
}