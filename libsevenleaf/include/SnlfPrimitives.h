#ifndef _SNLF_PRIMITIVES_H
#define _SNLF_PRIMITIVES_H

#include <compositor/vector/matrix3x2_t.h>
#include <compositor/vector/matrix4x4_t.h>

// ---
// SnlfThickness
// ---
#ifdef __cplusplus
struct SnlfThickness final {
  float left, top, right, bottom;

  constexpr SnlfThickness() noexcept: left(0.F), top(0.F), right(0.F), bottom(0.F) {}
  constexpr SnlfThickness(float scalar): left(scalar), top(scalar), right(scalar), bottom(scalar) {}
  constexpr SnlfThickness(float left, float top, float right, float bottom): left(left), top(top), right(right), bottom(bottom) {}

  constexpr float Vertical() const noexcept {
    return top + bottom;
  }

  constexpr float Horizontal() const noexcept {
    return left + right;
  }

  constexpr SnlfThickness& operator+=(SnlfThickness const& rhs) noexcept {
    left += rhs.left;
    top += rhs.top;
    right += rhs.right;
    bottom += rhs.bottom;
    return *this;
  }

  constexpr SnlfThickness& operator-=(SnlfThickness const& rhs) noexcept {
    left -= rhs.left;
    top -= rhs.top;
    right -= rhs.right;
    bottom -= rhs.bottom;
    return *this;
  }

  constexpr SnlfThickness& operator*=(float scale) noexcept {
    left *= scale;
    top *= scale;
    right *= scale;
    bottom *= scale;
    return *this;
  }
};

constexpr SnlfThickness operator+(SnlfThickness const& t1, SnlfThickness const& t2) {
  return SnlfThickness(t1) += t2;
}

constexpr SnlfThickness operator-(SnlfThickness const& t1, SnlfThickness const& t2) {
  return SnlfThickness(t1) -= t2;
}

inline SnlfThickness operator*(SnlfThickness const& t, float scale) {
  return SnlfThickness(t) *= scale;
}

inline SnlfThickness operator*(float scale, SnlfThickness const& t) {
  return SnlfThickness(t) *= scale;
}
#else
typedef struct {
  float left, top, right, bottom;
} SnlfThickness;

inline SnlfThickness SnlfThicknessMake(float left, float top, float right, float bottom) {
  SnlfThickness ret;
  ret.left = left;
  ret.top = top;
  ret.right = right;
  ret.bottom = bottom;
  return ret;
}
#endif

// ---
// SnlfSizeF
// ---
#ifdef __cplusplus
struct SnlfSizeF final {
  float width, height;

  constexpr SnlfSizeF() noexcept: width(0.F), height(0.F) {}
  constexpr SnlfSizeF(float width, float height) noexcept: width(width), height(height) {}
  constexpr SnlfSizeF(CpsrSizeU32 size) noexcept: width(size.width), height(size.height) {}

  constexpr float AspectRatio() const noexcept {
    return width / height;
  }

  inline SnlfSizeF AspectFit(SnlfSizeF boundingSize, bool scaleDownOnly = false, float *scale = nullptr) const noexcept {
    float scaleX = boundingSize.width / width;
    float scaleY = boundingSize.height / height;
    if (scaleX > scaleY) {
      if (scaleDownOnly && scaleY > 1.F) {
        if (scale) {
          *scale = 1.F;
        }
        boundingSize.width = width;
        boundingSize.height = height;
      } else {
        if (scale) {
          *scale = scaleY;
        }
        boundingSize.width = scaleY * width;
      }
    } else {
      if (scaleDownOnly && scaleX > 1.F) {
        if (scale) {
          *scale = 1.F;
        }
        boundingSize.width = width;
        boundingSize.height = height;
      } else {
        if (scale) {
          *scale = scaleX;
        }
        boundingSize.height = scaleX * height;
      }
    }
    return boundingSize;
  }

  inline SnlfSizeF AspectFill(SnlfSizeF boundingSize, float *scale = nullptr) const noexcept {
    float scaleX = boundingSize.width / width;
    float scaleY = boundingSize.height / height;
    if (scaleX > scaleY) {
      if (scale) {
        *scale = scaleX;
      }
      boundingSize.height = scaleX * height;
    } else {
      if (scale) {
        *scale = scaleY;
      }
      boundingSize.width = scaleY * width;
    }
    return boundingSize;
  }

  constexpr SnlfSizeF& operator+=(SnlfSizeF const& rhs) noexcept {
    width += rhs.width;
    height += rhs.height;
    return *this;
  }

  constexpr SnlfSizeF& operator+=(SnlfThickness const& rhs) noexcept {
    width += rhs.Horizontal();
    height += rhs.Vertical();
    return *this;
  }

  constexpr SnlfSizeF& operator-=(SnlfSizeF const& rhs) noexcept {
    width -= rhs.width;
    height -= rhs.height;
    return *this;
  }

  constexpr SnlfSizeF& operator-=(SnlfThickness const& rhs) noexcept {
    width -= rhs.Horizontal();
    height -= rhs.Vertical();
    return *this;
  }

  constexpr SnlfSizeF& operator*=(float scale) noexcept {
    width *= scale;
    height *= scale;
    return *this;
  }
};

inline SnlfSizeF operator+(SnlfSizeF const& t1, SnlfSizeF const& t2) {
  return SnlfSizeF(t1) += t2;
}

inline SnlfSizeF operator+(SnlfSizeF const& t1, SnlfThickness const& t2) {
  return SnlfSizeF(t1) += t2;
}

inline SnlfSizeF operator-(SnlfSizeF const& t1, SnlfSizeF const& t2) {
  return SnlfSizeF(t1) -= t2;
}

inline SnlfSizeF operator-(SnlfSizeF const& t1, SnlfThickness const& t2) {
  return SnlfSizeF(t1) -= t2;
}

inline SnlfSizeF operator*(SnlfSizeF const& t, float scale) {
  return SnlfSizeF(t) *= scale;
}

inline SnlfSizeF operator*(float scale, SnlfSizeF const& t) {
  return SnlfSizeF(t) *= scale;
}
#else
typedef struct {
  float width, height;
} SnlfSizeF;

inline SnlfSizeF SnlfSizeFMake(float width, float height) {
  SnlfSizeF ret;
  ret.width = width;
  ret.height = height;
  return ret;
}
#endif

// ---
// SnlfPointF
// ---
#ifdef __cplusplus
struct SnlfPointF final {
  float x, y;

  SnlfPointF() noexcept: x(0.F), y(0.F) {}
  inline SnlfPointF(float x, float y) noexcept: x(x), y(y) {}
  inline SnlfPointF(float32x2_t point) noexcept: x(point.x), y(point.y) {}
  inline SnlfPointF(SnlfSizeF size) noexcept: x(size.width), y(size.height) {}
  
  inline operator float32x2_t() const noexcept {
    float32x2_t point;
    point.x = x;
    point.y = y;
    return point;
  }

  constexpr SnlfPointF& operator+=(SnlfPointF const& rhs) noexcept {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  constexpr SnlfPointF& operator-=(SnlfPointF const& rhs) noexcept {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  constexpr SnlfPointF& operator*=(float scale) noexcept {
    x *= scale;
    y *= scale;
    return *this;
  }
};

inline SnlfPointF operator+(SnlfPointF const& t1, SnlfPointF const& t2) {
  return SnlfPointF(t1) += t2;
}

inline SnlfPointF operator-(SnlfPointF const& t1, SnlfPointF const& t2) {
  return SnlfPointF(t1) -= t2;
}

inline SnlfPointF operator*(SnlfPointF const& t, float scale) {
  return SnlfPointF(t) *= scale;
}
#else
typedef float32x2_t SnlfPointF;

inline SnlfPointF SnlfPointFMake(float x, float y) {
  SnlfPointF ret;
  ret.x = x;
  ret.y = y;
  return ret;
}
#endif

// ---
// SnlfColor
// ---
#ifdef __cplusplus
struct SnlfColor final {
  SnlfColor() = default;
  inline SnlfColor(float32x4_t color) noexcept: color_(color) {}
  inline SnlfColor(float red, float green, float blue, float alpha) noexcept: color_(float32x4_initv(red, green, blue, alpha)) {}
  
  float Red() const noexcept { return float32x4_getx(color_); }
  float Green() const noexcept { return float32x4_gety(color_); }
  float Blue() const noexcept { return float32x4_getz(color_); }
  float Alpha() const noexcept { return float32x4_getw(color_); }
  
  void SetRed(float red) noexcept { color_ = float32x4_setx(color_, red); }
  void SetGreen(float green) noexcept { color_ = float32x4_sety(color_, green); }
  void SetBlue(float blue) noexcept { color_ = float32x4_setz(color_, blue); }
  void SetAlpha(float alpha) noexcept { color_ = float32x4_setw(color_, alpha); }
  
  constexpr operator float32x4_t() const noexcept { return color_; }

  inline SnlfColor& operator+=(SnlfColor const& rhs) noexcept {
    color_ = float32x4_add(color_, rhs.color_);
    return *this;
  }

  inline SnlfColor& operator-=(SnlfColor const& rhs) noexcept {
    color_ = float32x4_sub(color_, rhs.color_);
    return *this;
  }

  inline SnlfColor& operator*=(float scale) noexcept {
    color_ = float32x4_mul(color_, float32x4_inits(scale));
    return *this;
  }
  
  static inline SnlfColor RedColor() noexcept { return SnlfColor(1.F, 0.F, 0.F, 1.F); }
  static inline SnlfColor GreenColor() noexcept { return SnlfColor(0.F, 1.F, 0.F, 1.F); }
  static inline SnlfColor BlueColor() noexcept { return SnlfColor(0.F, 0.F, 1.F, 1.F); }
  static inline SnlfColor YellowColor() noexcept { return SnlfColor(1.F, 1.F, 0.F, 1.F); }
  static inline SnlfColor MagentaColor() noexcept { return SnlfColor(1.F, 0.F, 1.F, 1.F); }
  static inline SnlfColor CyanColor() noexcept { return SnlfColor(0.F, 1.F, 1.F, 1.F); }
  
private:
  float32x4_t color_;
};

inline SnlfColor operator+(SnlfColor const& t1, SnlfColor const& t2) {
  return SnlfColor(t1) += t2;
}

inline SnlfColor operator-(SnlfColor const& t1, SnlfColor const& t2) {
  return SnlfColor(t1) -= t2;
}

inline SnlfColor operator*(SnlfColor const& t, float scale) {
  return SnlfColor(t) *= scale;
}
#else
typedef float32x4_t SnlfColor;

inline SnlfColor SnlfColorMake(float red, float green, float blue, float alpha) {
  SnlfColor ret;
  ret = float32x4_initv(red, green, blue, alpha);
  return ret;
}

inline float SnlfColorGetRed(SnlfColor color) { return float32x4_getx(color); }
inline float SnlfColorGetGreen(SnlfColor color) { return float32x4_gety(color); }
inline float SnlfColorGetBlue(SnlfColor color) { return float32x4_getz(color); }
inline float SnlfColorGetAlpha(SnlfColor color) { return float32x4_getw(color); }
#endif

// ---
// SnlfBox
// ---
typedef enum {
  SNLF_BOX_EMPTY = 0,
  SNLF_BOX_BOOL = 1,

  SNLF_BOX_INT,
  SNLF_BOX_INT8X2,
  SNLF_BOX_INT8X3,
  SNLF_BOX_INT16X2,
  SNLF_BOX_INT16X3,
  SNLF_BOX_INT32X2,
  SNLF_BOX_INT32X3,
  SNLF_BOX_INT64X2,
  SNLF_BOX_INT64X3,

  SNLF_BOX_UINT,
  SNLF_BOX_UINT8X2,
  SNLF_BOX_UINT8X3,
  SNLF_BOX_UINT16X2,
  SNLF_BOX_UINT16X3,
  SNLF_BOX_UINT32X2,
  SNLF_BOX_UINT32X3,
  SNLF_BOX_UINT64X2,
  SNLF_BOX_UINT64X3,

  SNLF_BOX_FLOAT32,
  SNLF_BOX_FLOAT32X2,
  SNLF_BOX_FLOAT32X3,

  SNLF_BOX_FLOAT64,
  SNLF_BOX_FLOAT64X2,
  SNLF_BOX_FLOAT64X3,

  SNLF_BOX_MATRIX3X2,
  SNLF_BOX_MATRIX4X4,

  SNLF_BOX_POINT,
  SNLF_BOX_SIZE,
  SNLF_BOX_THICKNESS,
} SnlfBoxType;

typedef struct _SnlfBox {
  SnlfBoxType type;
  union {
    bool boolean;

    int8_t int8;
    int8x2_t int8x2;
    int8x3_t int8x3;

    int16_t int16;
    int16x2_t int16x2;
    int16x3_t int16x3;

    int32_t int32;
    int32x2_t int32x2;
    int32x3_t int32x3;

    int64_t int64;
    int64x2_t int64x2;
    int64x3_t int64x3;

    uint8_t uint8;
    uint8x2_t uint8x2;
    uint8x3_t uint8x3;

    uint16_t uint16;
    uint16x2_t uint16x2;
    uint16x3_t uint16x3;

    uint32_t uint32;
    uint32x2_t uint32x2;
    uint32x3_t uint32x3;

    uint64_t uint64;
    uint64x2_t uint64x2;
    uint64x3_t uint64x3;

    float float32;
    float32x2_t float32x2;
    float32x3_t float32x3;

    double float64;
    float64x2_t float64x2;
    float64x3_t float64x3;

    matrix3x2_t matrix3x2;
    matrix4x4_t matrix4x4;

    SnlfPointF point;
    SnlfSizeF size;
    SnlfThickness thickness;
  };
} SnlfBox;
typedef struct _SnlfBox *SnlfBoxRef;

#endif  // _SNLF_PRIMITIVES_H
