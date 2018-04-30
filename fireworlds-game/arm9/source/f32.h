#include<nds.h>
#include<cstdlib>

#ifndef _F32_H
#define _F32_H


using namespace std;

class f32;

f32 fsqrt(const f32& f);
f32 fabs(const f32& f);

class f32
{
    private:
        int val;
        
    public:
    
    friend f32 fsqrt(const f32& f);
    friend f32 fabs(const f32& f);
    friend f32 frand(const f32& max);
    friend f32 fsin(int ang);
    friend f32 fcos(int ang);
    
    inline f32()
    {
    }
    
    inline f32(int n)
    {
        val = n<<12;
    }
    
    inline f32(float f)
    {
        val = (int)(f*(1<<12));
    }
    inline f32(double d)
    {
        val = (int)(d*(1<<12));
    }
    
    inline f32 operator+(const f32& b) const
    {
        f32 res;
        res.val = val+b.val;
        return res;
    }
    inline f32 operator-(const f32& b) const
    {
        f32 res;
        res.val = val-b.val;
        return res;
    }
    inline f32 operator*(const f32& b) const 
    {
        f32 res;
        res.val = mulf32(val,b.val);
        return res;
    }
    inline f32 operator/(const f32& b) const
    {
        f32 res;
        res.val = divf32(val,b.val);
        return res;
    }
    
    inline f32 operator*(const int& b) const 
    {
        f32 res;
        res.val = val*b;
        return res;
    }
    inline f32 operator/(const int& b) const
    {
        f32 res;
        res.val = val/b;
        return res;
    }
    
    
    
    inline void operator+=(const f32& b) 
    {
        val += b.val;
    }
    inline void  operator-=(const f32& b) 
    {
        val -= b.val;
    }
    inline void  operator*=(const f32& b)  
    {
        val = mulf32(val,b.val);
    }
    inline void  operator/=(const f32& b) 
    {
        val = divf32(val,b.val);
    }
    
    inline void  operator*=(const int& b)  
    {
        val = val*b;
    }
    inline void  operator/=(const int& b) 
    {
        val = val/b;
    }
    
    inline f32 operator-() const
    {
        f32 res;
        res.val = -val;
        return res;
    }
    
    inline bool operator==(const f32& b) const
    {
        return val == b.val;
    }
    inline bool operator!=(const f32& b) const
    {
        return val != b.val;
    }
    inline bool operator<(const f32& b) const
    {
        return val < b.val;
    }
    inline bool operator>(const f32& b) const
    {
        return val > b.val;
    }
    inline bool operator<=(const f32& b) const
    {
        return val <= b.val;
    }
    inline bool operator>=(const f32& b) const
    {
        return val >= b.val;
    }
    
    inline int toint() const
    {
        return val>>12;
    }
    inline int tof5() const
    {
        return val>>7;
    }
    inline int raw() const
    {
        return val;
    }
};

inline f32 fsin(int ang)
{
    f32 res;
    res.val = sinLerp(ang);
    return res;
}

inline f32 fcos(int ang)
{
    f32 res;
    res.val = cosLerp(ang);
    return res;
}
    

inline f32 fsqrt(const f32& n)
{
    f32 res;
    res.val = sqrtf32(n.val);
    return res;
}

inline f32 fabs(const f32& n)
{
    f32 res;
    if(n.val < 0) res.val = -n.val;
    else res.val = n.val;
    return res;
}

inline void glVertex3f32(f32 x, f32 y, f32 z)
{
    glVertex3v16(x.tof5(), y.tof5(), z.tof5());
}

inline f32 frand(const f32& max)
{
    if(max ==  0) return 0;
    f32 res;
    res.val = rand() % (max.raw() * 2) - max.raw();
    return res;
}

inline void vecProject(f32& cx, f32& cy, f32 ax, f32 ay)
{ 
    f32 r_numerator = cx*ax + cy*ay;
    f32 r_denomenator = ax*ax + ay*ay;
    f32 r = r_numerator / r_denomenator;

    cx = r*ax;
    cy = r*ay;
}


inline f32 vecProjectionLen(f32 cx, f32 cy, f32 ax, f32 ay)
{
    f32 r_numerator = cx*ax + cy*ay;
    f32 r_denomenator = ax*ax + ay*ay;
    f32 r = r_numerator / r_denomenator;

    return r;
}

inline void vecNormalize(f32& x, f32& y)
{
    f32 len = fsqrt(x*x+y*y);
    x /= len;
    y /= len;
}


inline int irand(int n)
{
    if(n ==  0) return 0;
    return rand()%(2*n) - n;
}
#endif
