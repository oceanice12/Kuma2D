#pragma once



template<typename T>
struct Vector2
{
	T x, y;



	template<typename T2>
	explicit operator Vector2<T2>() const { return {static_cast<T2>(this->x), static_cast<T2>(this->y)}; }


	void operator*=(const T& s);
};



// Vector functions
template<typename T>
Vector2<T> Normalize(const Vector2<T>& v)
{
	T magnitude = sqrt(v.x * v.x + v.y * v.y);
	if (magnitude == 0)
		return {0,0};

	return {v.x / magnitude, v.y / magnitude};
}

template<typename T>
float Magnitude(const Vector2<T>& v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

template<typename T>
float Distance(const Vector2<T>& v1, const Vector2<T>& v2)
{
	return sqrtf((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

template<typename T>
float DotProduct(const Vector2<T>& v1, const Vector2<T>& v2)
{
	return (v1.x * v2.x + v1.y * v2.y);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Vector2<T>& v)
{
	out << "(" << v.x << ',';
	out << v.y << ")";
	return out;
}

// Vector Vector
template<typename T1, typename T2>
Vector2<T1> operator+=(Vector2<T1>& v1, const Vector2<T2>& v2)
{
	v1 = {v1.x + v2.x, static_cast<T1>(v1.y) + static_cast<T1>(v2.y)};
	return v1;
}

template<typename T1, typename T2>
Vector2<T1> operator-=(Vector2<T1>& v1, const Vector2<T2>& v2)
{
	v1 = {v1.x - v2.x, v1.y - v2.y};
	return v1;
}

template<typename T1, typename T2>
Vector2<T1> operator+(const Vector2<T1>& v1, const Vector2<T2>& v2)
{
	return {v1.x + v2.x, v1.y + v2.y};
}

template<typename T1, typename T2>
Vector2<T1> operator-(const Vector2<T1>& v1, const Vector2<T2>& v2)
{
	return {v1.x - v2.x, v1.y - v2.y};
}

template<typename T1, typename T2>
Vector2<T1> operator*(const Vector2<T1>& v1, const Vector2<T2>& v2)
{
	return {v1.x * v2.x, v1.y * v2.y};
}

template<typename T1, typename T2>
Vector2<T1> operator/(const Vector2<T1>& v1, const Vector2<T2>& v2)
{
	return {v1.x / static_cast<T1>(v2.x), v1.y / static_cast<T1>(v2.y)};
}



// Vector Scalar
template<typename T1, typename T2>
Vector2<T1> operator/(const Vector2<T1>& v, const T2& s)
{
	return {v.x / s, v.y / s};
}

template<typename T1, typename T2>
Vector2<T1> operator*(const Vector2<T1>& v, const T2& s)
{
	return {v.x * static_cast<T1>(s), v.y * static_cast<T1>(s)};
}

// Scalar Vector
template<typename T1, typename T2>
Vector2<T2> operator-(const T1& s, const Vector2<T2>& v)
{
	return {s - v.x, s - v.y};
}

template<typename T>
Vector2<T> operator-(const Vector2<T>& v)
{
	return {-v.x, -v.y};
}

template<typename T1, typename T2>
Vector2<T2> operator+(const T1& s, const Vector2<T2>& v)
{
	return {s + v.x, s + v.y};
}

template<typename T1, typename T2>
Vector2<T2> operator*(const T1& s, const Vector2<T2>& v)
{
	return {v.x * static_cast<T2>(s), v.y * static_cast<T2>(s)};
}

template<typename T>
void Vector2<T>::operator*=(const T& s)
{
	this->x = this->x * static_cast<T>(s);
	this->y = this->y * static_cast<T>(s);
}