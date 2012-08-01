#ifndef _P_INT64_H
#define _P_INT64_H

///////////////////////////////////////////////////////////////////////////////
// Really big integer class for architectures without

#ifdef P_NEEDS_INT64

class PInt64__ {
  public:
    operator long()  const { return (long)low; }
    operator int()   const { return (int)low; }
    operator short() const { return (short)low; }
    operator char()  const { return (char)low; }

    operator unsigned long()  const { return (unsigned long)low; }
    operator unsigned int()   const { return (unsigned int)low; }
    operator unsigned short() const { return (unsigned short)low; }
    operator unsigned char()  const { return (unsigned char)low; }

  protected:
    PInt64__() { }
    PInt64__(unsigned long l) : low(l), high(0) { }
    PInt64__(unsigned long l, unsigned long h) : low(l), high(h) { }

    void operator=(const PInt64__ & v) { low = v.low; high = v.high; }

    void Inc() { if (++low == 0) ++high; }
    void Dec() { if (--low == 0) --high; }

    void Or (long v) { low |= v; }
    void And(long v) { low &= v; }
    void Xor(long v) { low ^= v; }

    void Add(const PInt64__ & v);
    void Sub(const PInt64__ & v);
    void Mul(const PInt64__ & v);
    void Div(const PInt64__ & v);
    void Mod(const PInt64__ & v);
    void Or (const PInt64__ & v) { low |= v.low; high |= v.high; }
    void And(const PInt64__ & v) { low &= v.low; high &= v.high; }
    void Xor(const PInt64__ & v) { low ^= v.low; high ^= v.high; }
    void ShiftLeft(int bits);
    void ShiftRight(int bits);

    BOOL Eq(unsigned long v) const { return low == v && high == 0; }
    BOOL Ne(unsigned long v) const { return low != v || high != 0; }

    BOOL Eq(const PInt64__ & v) const { return low == v.low && high == v.high; }
    BOOL Ne(const PInt64__ & v) const { return low != v.low || high != v.high; }

    unsigned long low, high;
};


#define DECL_OPS(cls, type) \
    const cls & operator=(type v) { PInt64__::operator=(cls(v)); return *this; } \
    cls operator+(type v) const { cls t = *this; t.Add(v); return t; } \
    cls operator-(type v) const { cls t = *this; t.Sub(v); return t; } \
    cls operator*(type v) const { cls t = *this; t.Mul(v); return t; } \
    cls operator/(type v) const { cls t = *this; t.Div(v); return t; } \
    cls operator%(type v) const { cls t = *this; t.Mod(v); return t; } \
    cls operator|(type v) const { cls t = *this; t.Or (v); return t; } \
    cls operator&(type v) const { cls t = *this; t.And(v); return t; } \
    cls operator^(type v) const { cls t = *this; t.Xor(v); return t; } \
    cls operator<<(type v) const { cls t = *this; t.ShiftLeft((int)v); return t; } \
    cls operator>>(type v) const { cls t = *this; t.ShiftRight((int)v); return t; } \
    const cls & operator+=(type v) { Add(v); return *this; } \
    const cls & operator-=(type v) { Sub(v); return *this; } \
    const cls & operator*=(type v) { Mul(v); return *this; } \
    const cls & operator/=(type v) { Div(v); return *this; } \
    const cls & operator|=(type v) { Or (v); return *this; } \
    const cls & operator&=(type v) { And(v); return *this; } \
    const cls & operator^=(type v) { Xor(v); return *this; } \
    const cls & operator<<=(type v) { ShiftLeft((int)v); return *this; } \
    const cls & operator>>=(type v) { ShiftRight((int)v); return *this; } \
    BOOL operator==(type v) const { return Eq(v); } \
    BOOL operator!=(type v) const { return Ne(v); } \
    BOOL operator< (type v) const { return Lt(v); } \
    BOOL operator> (type v) const { return Gt(v); } \
    BOOL operator>=(type v) const { return !Gt(v); } \
    BOOL operator<=(type v) const { return !Lt(v); } \


class PInt64 : public PInt64__ {
  public:
    PInt64() { }
    PInt64(long l) : PInt64__(l, l < 0 ? -1 : 0) { }
    PInt64(unsigned long l, long h) : PInt64__(l, h) { }
    PInt64(const PInt64__ & v) : PInt64__(v) { }

    PInt64 operator~() const { return PInt64(~low, ~high); }
    PInt64 operator-() const { return operator~()+1; }

    PInt64 operator++() { Inc(); return *this; }
    PInt64 operator--() { Dec(); return *this; }

    PInt64 operator++(int) { PInt64 t = *this; Inc(); return t; }
    PInt64 operator--(int) { PInt64 t = *this; Dec(); return t; }

    DECL_OPS(PInt64, char)
    DECL_OPS(PInt64, unsigned char)
    DECL_OPS(PInt64, short)
    DECL_OPS(PInt64, unsigned short)
    DECL_OPS(PInt64, int)
    DECL_OPS(PInt64, unsigned int)
    DECL_OPS(PInt64, long)
    DECL_OPS(PInt64, unsigned long)
    DECL_OPS(PInt64, const PInt64 &)

    friend ostream & operator<<(ostream &, const PInt64 &);
    friend istream & operator>>(istream &, PInt64 &);

  protected:
    void Add(long v) { Add(PInt64(v)); }
    void Sub(long v) { Sub(PInt64(v)); }
    void Mul(long v) { Mul(PInt64(v)); }
    void Div(long v) { Div(PInt64(v)); }
    void Mod(long v) { Mod(PInt64(v)); }
    BOOL Lt(long v) const { return Lt(PInt64(v)); }
    BOOL Gt(long v) const { return Gt(PInt64(v)); }
    BOOL Lt(const PInt64 &) const;
    BOOL Gt(const PInt64 &) const;
};


class PUInt64 : public PInt64__ {
  public:
    PUInt64() { }
    PUInt64(unsigned long l) : PInt64__(l, 0) { }
    PUInt64(unsigned long l, unsigned long h) : PInt64__(l, h) { }
    PUInt64(const PInt64__ & v) : PInt64__(v) { }

    PUInt64 operator~() const { return PUInt64(~low, ~high); }

    const PUInt64 & operator++() { Inc(); return *this; }
    const PUInt64 & operator--() { Dec(); return *this; }

    PUInt64 operator++(int) { PUInt64 t = *this; Inc(); return t; }
    PUInt64 operator--(int) { PUInt64 t = *this; Dec(); return t; }

    DECL_OPS(PUInt64, char)
    DECL_OPS(PUInt64, unsigned char)
    DECL_OPS(PUInt64, short)
    DECL_OPS(PUInt64, unsigned short)
    DECL_OPS(PUInt64, int)
    DECL_OPS(PUInt64, unsigned int)
    DECL_OPS(PUInt64, long)
    DECL_OPS(PUInt64, unsigned long)
    DECL_OPS(PUInt64, const PUInt64 &)

    friend ostream & operator<<(ostream &, const PUInt64 &);
    friend istream & operator>>(istream &, PUInt64 &);

  protected:
    void Add(long v) { Add(PUInt64(v)); }
    void Sub(long v) { Sub(PUInt64(v)); }
    void Mul(long v) { Mul(PUInt64(v)); }
    void Div(long v) { Div(PUInt64(v)); }
    void Mod(long v) { Mod(PUInt64(v)); }
    BOOL Lt(long v) const { return Lt(PUInt64(v)); }
    BOOL Gt(long v) const { return Gt(PUInt64(v)); }
    BOOL Lt(const PUInt64 &) const;
    BOOL Gt(const PUInt64 &) const;
};

#undef DECL_OPS

#endif  // P_NEEDS_INT64

#endif  // P_INT64_H

