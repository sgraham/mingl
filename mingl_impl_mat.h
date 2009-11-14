class Mat44
{
    public:
        typedef const Mat44& Arg;

        Vec4 c1, c2, c3, c4;

        Mat44() {}
        Mat44(const GLfloat* f)
            : c1(f[0], f[4], f[8], f[12])
            , c2(f[1], f[5], f[9], f[13])
            , c3(f[2], f[6], f[10], f[14])
            , c4(f[3], f[7], f[11], f[15])
        {}
        Mat44(float f0, float f1, float f2, float f3,
              float f4, float f5, float f6, float f7,
              float f8, float f9, float f10, float f11,
              float f12, float f13, float f14, float f15)
            : c1(f0, f4, f8, f12)
            , c2(f1, f5, f9, f13)
            , c3(f2, f6, f10, f14)
            , c4(f3, f7, f11, f15)
        {}
        Mat44(Vec4::Arg v1,
              Vec4::Arg v2,
              Vec4::Arg v3,
              Vec4::Arg v4)
            : c1(v1)
            , c2(v2)
            , c3(v3)
            , c4(v4)
        {}
};

inline Vec4 operator*(Mat44::Arg m, Vec4::Arg v)
{
    NativeVectorType x, y, z, w, ret;

    MINGL_VECTOR_SPLAT_I(x, v.v, 0);
    MINGL_VECTOR_SPLAT_I(y, v.v, 1);
    MINGL_VECTOR_SPLAT_I(z, v.v, 2);
    MINGL_VECTOR_SPLAT_I(w, v.v, 3);

    MINGL_VECTOR_MULT(ret, m.c1.v, x);
    MINGL_VECTOR_MADD(ret, m.c2.v, y, ret);
    MINGL_VECTOR_MADD(ret, m.c3.v, z, ret);
    MINGL_VECTOR_MADD(ret, m.c4.v, w, ret);

    return Vec4(ret);
}

inline Mat44& operator*=(Mat44& a, Mat44::Arg b)
{
    a = Mat44(a * b.c1,
              a * b.c2,
              a * b.c3,
              a * b.c4);
    return a;
}
