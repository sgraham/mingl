class Mat44
{
    public:
        Vec4 l1, l2, l3, l4;
        Mat44() {}
        Mat44(const GLfloat* f)
            : l1(f)
            , l2(f + 4)
            , l3(f + 8)
            , l4(f + 12)
        {}
        Mat44(float f0, float f1, float f2, float f3,
              float f4, float f5, float f6, float f7,
              float f8, float f9, float f10, float f11,
              float f12, float f13, float f14, float f15)
            : l1(f0, f1, f2, f3)
            , l2(f4, f5, f6, f7)
            , l3(f8, f9, f10, f11)
            , l4(f12, f13, f14, f15)
        {}
};

