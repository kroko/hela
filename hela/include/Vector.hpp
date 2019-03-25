#ifndef Vector_hpp
#define Vector_hpp

namespace hela { namespace vec {

  // Do not use GLM in Hela / Nucleus, be indifferent
  // We only use vec here and there and only for storage
  // Any operator overload just on demand
  // You shall not use hela::vec<T> in user code, it is here just for helping out internal representations

  template<typename T = float>
  struct vec3 {
    T x;
    T y;
    T z;
    vec3(): x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)) {}
    vec3(T x_, T y_, T z_): x(x_), y(y_), z(z_) {}
  };

  template <typename T = float>
  struct vec2 {
    T x;
    T y;
    vec2(): x(static_cast<T>(0)), y(static_cast<T>(0)) {}
    vec2(T x_, T y_): x(x_), y(y_) {}
    vec2 operator+(const vec2& v) const { return vec2(v.x+x, v.y+y); }
    vec2 operator-(const vec2& v) const { return vec2(v.x-x, v.y-y); }
  };

} }

#endif // Vector_hpp
