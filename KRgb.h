#pragma once

#include <math.h>
#include <iostream>
#include <objidl.h>
#include <gdiplus.h>
#include <windows.h>

inline bool approx_eq(float a, float b)
{
    return fabs(a - b) < 1.0E-8;
}

class KRgb
{
    float rgb[3]; // each r,g, and b values are range from 0 to 1.

public:

    KRgb() { rgb[0] = 0.0f; rgb[1] = 0.0f; rgb[2] = 0.0f; }
    KRgb(float r, float g, float b) { rgb[0] = r; rgb[1] = g; rgb[2] = b; }
    KRgb(const KRgb& rhs_) { *this = rhs_; }
    KRgb(float* pvalue_) { rgb[0] = pvalue_[0]; rgb[1] = pvalue_[1]; rgb[2] = pvalue_[2]; }

    Gdiplus::Color GetGdiColor()
    {
        Gdiplus::Color temp(BYTE(rgb[0]*255.f), BYTE(rgb[1]*255.f), BYTE(rgb[2]*255.f));
        return temp;
    }

    COLORREF GetColor() const { return RGB(rgb[0] * 255.f, rgb[1] * 255.f, rgb[2] * 255.f); }

    KRgb& operator=(const KRgb& rhs_) { rgb[0] = rhs_.rgb[0]; rgb[1] = rhs_.rgb[1]; rgb[2] = rhs_.rgb[2]; return *this; }

    bool operator==(const KRgb& rhs_) const { return approx_eq(rgb[0], rhs_.rgb[0]) && approx_eq(rgb[1], rhs_.rgb[1]) && approx_eq(rgb[2], rhs_.rgb[2]); }
    bool operator!=(const KRgb& rhs_) const { return !this->operator==(rhs_); }
    KRgb operator+(const KRgb& rhs_) const
    {
        KRgb temp;
        temp.rgb[0] = rgb[0] + rhs_.rgb[0];
        temp.rgb[1] = rgb[1] + rhs_.rgb[1];
        temp.rgb[2] = rgb[2] + rhs_.rgb[2];
        return temp;
    }
    KRgb operator-(const KRgb& rhs_) const
    {
        KRgb temp;
        temp.rgb[0] = rgb[0] - rhs_.rgb[0];
        temp.rgb[1] = rgb[1] - rhs_.rgb[1];
        temp.rgb[2] = rgb[2] - rhs_.rgb[2];
        return temp;
    }
    KRgb operator*(float scale) const
    {
        KRgb temp;
        temp.rgb[0] = rgb[0] * scale;
        temp.rgb[1] = rgb[1] * scale;
        temp.rgb[2] = rgb[2] * scale;
        return temp;
    }
    KRgb operator*(const KRgb& rhs) const
    {
        KRgb temp;
        temp.rgb[0] = rgb[0] * rhs.rgb[0];
        temp.rgb[1] = rgb[1] * rhs.rgb[1];
        temp.rgb[2] = rgb[2] * rhs.rgb[2];
        return temp;
    }
    KRgb operator/(float scale) const
    {
        KRgb temp;
        temp.rgb[0] = rgb[0] / scale;
        temp.rgb[1] = rgb[1] / scale;
        temp.rgb[2] = rgb[2] / scale;
        return temp;
    }
    KRgb operator+=(const KRgb& rhs_)
    {
        rgb[0] += rhs_.rgb[0];
        rgb[1] += rhs_.rgb[1];
        rgb[2] += rhs_.rgb[2];
        return *this;
    }
    KRgb operator-=(const KRgb& rhs_)
    {
        rgb[0] -= rhs_.rgb[0];
        rgb[1] -= rhs_.rgb[1];
        rgb[2] -= rhs_.rgb[2];
        return *this;
    }
    KRgb operator/=(float scale)
    {
        rgb[0] /= scale;
        rgb[1] /= scale;
        rgb[2] /= scale;
        return *this;
    }
    KRgb operator*=(float scale)
    {
        rgb[0] *= scale;
        rgb[1] *= scale;
        rgb[2] *= scale;
        return *this;
    }
    float& operator[](int index)
    {
        return rgb[index];
    }
    const float& operator[](int index) const
    {
        return rgb[index];
    }

    friend std::ostream& operator<<(std::ostream& os, KRgb const& P)       { return os << "rgb=(" << P.rgb[0] << ", " << P.rgb[1] << ", " << P.rgb[2] << ")"; }
};

struct ScannedResult
{
    ScannedResult(int X, int Y, KRgb const& Col) : x(X), y(Y), col(Col) {}

    bool operator<(ScannedResult const& rhs) const { return (y < rhs.y) || ((y == rhs.y) && (x < rhs.x)); }

    int x, y;
    KRgb col;
};

/*

{
    KRgb col1;
    KRgb col2(1.0f);
    float ptr[3] = { 1, 1, 1 };
    KRgb col3(ptr);
    KRgb col4(col3);

    assert(col1 == Black);
    assert(col2 == Red);
    assert(col3 == White);
    assert(col4 == White);

    try
    {
        std::cout << "col1 is " << col1 << std::endl;
        std::cout << "col2 is " << col2 << std::endl;
        std::cout << "col3 is " << col3 << std::endl;
        std::cout << "col4 is " << col4 << std::endl;


        KRgb col5 = col2 * 1.0;// + 1.0 * col3;
        col5 /= 2.0;
        std::cout << "col 5 is " << col5 << std::endl;

        col5 = col2 * col3;
        assert(col5 == Red);
        std::cout << "col 5 is " << col5 << std::endl;

        assert(col5 != col1);

        assert(White - Blue == Yellow);

        for (int i = 0; i<3; i++)
        col1[i] = col4[i];
    }
    catch (const char* msg)
    {
        std::cerr << msg << std::endl;
        exit(0);
    }
}

*/