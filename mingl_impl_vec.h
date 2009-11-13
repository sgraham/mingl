union VecUnion
{
    NativeVectorType v;
    float asFloat[4];
};

template <int I>
class VecFloatRef
{
    public:
        typedef const VecFloatRef& Arg;

        VecFloatRef(const NativeVectorType& v) : rv(v) {}

        operator float() const
        {
            VecUnion tmp;
            tmp.v = rv;
            return tmp.asFloat[I];
        }

        enum { Index = I };
        const NativeVectorType& rv;
};

class VecFloat
{
    public:
        typedef const VecFloat& Arg;
        VecFloat() {}

        explicit VecFloat(const GLfloat f) { MINGL_VECTOR_SPLAT_1(v, f); }

        explicit VecFloat(const NativeVectorType v) : v(v) {}

        template <int I>
        VecFloat(VecFloatRef<I> r)
        {
            MINGL_VECTOR_SPLAT_I(v, r.rv, I);
        }
        operator float() const
        {
            VecUnion u;
            u.v = v;
            return u.asFloat[0];
        }

        NativeVectorType v;

};

class Vec4
{
    public:
        typedef const Vec4& Arg;
        Vec4() {}
        Vec4(const GLfloat* f)
        {
            MINGL_VECTOR_LOAD_UNALIGNED(v, f);
        }
        Vec4(float a, float b, float c, float d)
        {
            MINGL_VECTOR_SET_FROM_FLOATS(v, a, b, c, d);
        }
        Vec4(const NativeVectorType& v) : v(v) {}

        const VecFloatRef<0> X() const { return VecFloatRef<0>(v); }
        const VecFloatRef<1> Y() const { return VecFloatRef<1>(v); }
        const VecFloatRef<2> Z() const { return VecFloatRef<2>(v); }
        const VecFloatRef<3> W() const { return VecFloatRef<3>(v); }

        void SetX(float x)
        {
            VecUnion tmp;
            tmp.v = v;
            tmp.asFloat[0] = x;
            v = tmp.v;
        }
        void SetY(float y)
        {
            VecUnion tmp;
            tmp.v = v;
            tmp.asFloat[1] = y;
            v = tmp.v;
        }
        void SetZ(float z)
        {
            VecUnion tmp;
            tmp.v = v;
            tmp.asFloat[2] = z;
            v = tmp.v;
        }
        void SetW(float w)
        {
            VecUnion tmp;
            tmp.v = v;
            tmp.asFloat[3] = w;
            v = tmp.v;
        }

        NativeVectorType v;
};

inline VecFloat Ceil(VecFloat::Arg a)
{
    NativeVectorType ret;
    MINGL_VECTOR_CEIL(ret, a.v);
    return VecFloat(ret);
}

template <int I>
inline VecFloat Ceil(VecFloatRef<I> a)
{
    NativeVectorType ret;
    MINGL_VECTOR_CEIL(ret, a.rv);
    return VecFloat(ret);
}

// BINARY OP +
inline VecFloat operator+(VecFloat::Arg a, VecFloat::Arg b)
{
    NativeVectorType ret;
    MINGL_VECTOR_ADD(ret, a.v, b.v);
    return VecFloat(ret);
}

template <int I1, int I2>
inline VecFloat operator+(VecFloatRef<I1> a, VecFloatRef<I2> b)
{
    NativeVectorType ret;
    NativeVectorType aa;
    NativeVectorType bb;
    MINGL_VECTOR_SPLAT_I(aa, a.rv, I1);
    MINGL_VECTOR_SPLAT_I(bb, b.rv, I2);
    MINGL_VECTOR_ADD(ret, aa, bb);
    return VecFloat(ret);
}

// BINARY OP -
inline VecFloat operator-(VecFloat::Arg a, VecFloat::Arg b)
{
    NativeVectorType ret;
    MINGL_VECTOR_SUBTRACT(ret, a.v, b.v);
    return VecFloat(ret);
}

template <int I1, int I2>
inline VecFloat operator-(VecFloatRef<I1> a, VecFloatRef<I2> b)
{
    NativeVectorType ret;
    NativeVectorType aa;
    NativeVectorType bb;
    MINGL_VECTOR_SPLAT_I(aa, a.rv, I1);
    MINGL_VECTOR_SPLAT_I(bb, b.rv, I2);
    MINGL_VECTOR_SUBTRACT(ret, aa, bb);
    return VecFloat(ret);
}

// OP *
template <int I>
inline VecFloat operator*(VecFloat::Arg a, VecFloat::Arg b)
{
    NativeVectorType ret;
    MINGL_VECTOR_MULTIPLY(ret, a.v, b.v);
    return VecFloat(ret);
}

template <int I>
inline VecFloat operator*(VecFloat::Arg a, VecFloatRef<I> b)
{
    NativeVectorType ret;
    NativeVectorType bb;
    MINGL_VECTOR_SPLAT_I(bb, b.rv, I);
    MINGL_VECTOR_MULTIPLY(ret, a.v, bb);
    return VecFloat(ret);
}

template <int I>
inline VecFloat operator*(VecFloatRef<I> a, VecFloat b)
{
    NativeVectorType ret;
    NativeVectorType aa;
    MINGL_VECTOR_SPLAT_I(aa, a.rv, I);
    MINGL_VECTOR_MULTIPLY(ret, aa, b.v);
    return VecFloat(ret);
}

template <int I1, int I2>
inline VecFloat operator*(VecFloatRef<I1> a, VecFloatRef<I2> b)
{
    NativeVectorType ret;
    NativeVectorType aa;
    NativeVectorType bb;
    MINGL_VECTOR_SPLAT_I(aa, a.rv, I1);
    MINGL_VECTOR_SPLAT_I(bb, b.rv, I2);
    MINGL_VECTOR_MULTIPLY(ret, aa, bb);
    return VecFloat(ret);
}

// OP /
template <int I>
inline VecFloat operator/(VecFloat::Arg a, VecFloat::Arg b)
{
    NativeVectorType ret;
    MINGL_VECTOR_DIVIDE(ret, a.v, b.v);
    return VecFloat(ret);
}

template <int I>
inline VecFloat operator/(VecFloat::Arg a, VecFloatRef<I> b)
{
    NativeVectorType ret;
    NativeVectorType bb;
    MINGL_VECTOR_SPLAT_I(bb, b.rv, I);
    MINGL_VECTOR_DIVIDE(ret, a.v, bb);
    return VecFloat(ret);
}

template <int I>
inline VecFloat operator/(VecFloatRef<I> a, VecFloat b)
{
    NativeVectorType ret;
    NativeVectorType aa;
    MINGL_VECTOR_SPLAT_I(aa, a.rv, I);
    MINGL_VECTOR_DIVIDE(ret, aa, b.v);
    return VecFloat(ret);
}

template <int I1, int I2>
inline VecFloat operator/(VecFloatRef<I1> a, VecFloatRef<I2> b)
{
    NativeVectorType ret;
    NativeVectorType aa;
    NativeVectorType bb;
    MINGL_VECTOR_SPLAT_I(aa, a.rv, I1);
    MINGL_VECTOR_SPLAT_I(bb, b.rv, I2);
    MINGL_VECTOR_DIVIDE(ret, aa, bb);
    return VecFloat(ret);
}

// OP >
template <int I>
inline bool operator>(VecFloatRef<I> a, VecFloat::Arg b)
{
    bool ret;
    NativeVectorType aa;
    MINGL_VECTOR_SPLAT_I(aa, a.rv, I);
    MINGL_VECTOR_SCALAR_IS_GREATER(ret, aa, b.v);
    return ret;
}

template <int I1, int I2>
inline bool operator>(VecFloatRef<I1> a, VecFloatRef<I2> b)
{
    bool ret;
    NativeVectorType aa;
    NativeVectorType bb;
    MINGL_VECTOR_SPLAT_I(aa, a.rv, I1);
    MINGL_VECTOR_SPLAT_I(bb, b.rv, I2);
    MINGL_VECTOR_SCALAR_IS_GREATER(ret, aa, bb);
    return ret;
}
