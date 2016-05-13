#ifndef _DOUBLE
#define _DOUBLE

#include <cmath>
#include <iostream>

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
        
        Double ()
        {
        }
        
        Double operator - () const
        {
            return Double(-value);
        }
        
        Double& operator += (const Double &d)
        {
            value += d.value;
            return *this;
        }
        
        Double& operator -= (const Double &d)
        {
            value -= d.value;
            return *this;
        }
        
        operator double () const
        {
            return value;
        }
    };
    const double Double::EPS = 1e-9;
    
    Double operator * (const Double &a, const Double &b)
    {
        return Double(double(a) * double(b));
    }
    
    template<typename T>
    Double operator * (const Double &a, const T &b)
    {
        return Double(double(a) * double(b));
    }
    
    template<typename T>
    Double operator * (const T &a, const Double &b)
    {
        return Double(double(a) * double(b));
    }
    
    Double operator / (const Double &a, const Double &b)
    {
        return Double(double(a) / double(b));
    }
    
    template<typename T>
    Double operator / (const Double &a, const T &b)
    {
        return Double(double(a) / double(b));
    }
    
    template<typename T>
    Double operator / (const T &a, const Double &b)
    {
        return Double(double(a) / double(b));
    }
    
    Double operator + (const Double &a, const Double &b)
    {
        return Double(double(a) + double(b));
    }
    
    Double operator - (const Double &a, const Double &b)
    {
        return Double(double(a) - double(b));
    }
    
    bool operator == (const Double &a, const Double &b)
    {
        return fabs(a - b) < Double::EPS;
    }
    
    bool operator < (const Double &a, const Double &b)
    {
        return double(a) < double(b) - Double::EPS;
    }
    
    template<typename T>
    bool operator < (const T &a, const Double &b)
    {
        return double(a) < double(b) - Double::EPS;
    }
    
    template<typename T>
    bool operator < (const Double &a, const T &b)
    {
        return double(a) < double(b) - Double::EPS;
    }
    
    bool operator > (const Double &a, const Double &b)
    {
        return double(a) > double(b) + Double::EPS;
    }
    
    bool operator > (const Double &a, const double &b)
    {
        return double(a) > double(b) + Double::EPS;
    }
    
    bool operator > (const double &a, const Double &b)
    {
        return double(a) > double(b) + Double::EPS;
    }
    
    std::istream& operator >> (std::istream &in, Double &d)
    {
        double _d;
        in >> _d;
        d = Double(_d);
        
        return in;
    }
    
    std::ostream& operator << (std::ostream &out, const Double &d)
    {
        out << double(d);
        return out;
    }
};

#endif
