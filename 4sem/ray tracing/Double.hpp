#ifndef _DOUBLE
#define _DOUBLE

#include <cmath>

namespace NDouble
{
    class Double
    {
        double value;
    public:
        static const double EPS;
        
        Double (const double &value)
        : value(value)
        {
        }
        
        Double operator - () const
        {
            return Double(-value);
        }
        
        operator double () const
        {
            return value;
        }
    };
    const double Double::EPS = 1e-9;
    
    bool operator == (const Double &a, const Double &b)
    {
        return fabs(a - b) < Double::EPS;
    }
    
    bool operator < (const Double &a, const Double &b)
    {
        return double(a) < double(b) - Double::EPS;
    }
    
    bool operator > (const Double &a, const Double &b)
    {
        return double(a) > double(b) + Double::EPS;
    }
};

#endif
