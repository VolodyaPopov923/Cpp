#pragma once
#include <cmath>
#include <stdexcept>

template< typename T >
struct matrix_t
{
	T data[16];
};

template< typename T >
struct vector3_t
{
	T x, y, z;
};

template< typename T >
class Quat
{
	T m_value[4];

  public:
	explicit Quat(T a = 0, T b = 0, T c = 0, T d = 0) : m_value{ b, c, d, a } {}

	Quat(T angle, bool isRadians, vector3_t< T > axis)
	{
		angle = isRadians ? angle / 2 : angle * T(M_PI) / 360;
		T norm = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
		if (norm == 0)
			throw std::invalid_argument("Norm cannot be zero");
		T s = std::sin(angle);
		m_value[3] = std::cos(angle);
		m_value[0] = axis.x / norm * s;
		m_value[1] = axis.y / norm * s;
		m_value[2] = axis.z / norm * s;
	}
	Quat< T > operator+(const Quat< T >& q) const
	{
		Quat result = *this;
		result += q;
		return result;
	}
	Quat< T >& operator+=(const Quat< T >& q)
	{
		for (int i = 0; i < 4; i++)
		{
			m_value[i] += q.m_value[i];
		}
		return *this;
	}

	Quat< T > operator-(const Quat< T >& q) const
	{
		Quat result = *this;
		result -= q;
		return result;
	}
	Quat< T >& operator-=(const Quat< T >& q)
	{
		for (int i = 0; i < 4; i++)
		{
			m_value[i] -= q.m_value[i];
		}
		return *this;
	}

	Quat< T > operator~() const { return Quat(m_value[3], -m_value[0], -m_value[1], -m_value[2]); }

	Quat< T > operator*(const Quat< T >& q) const
	{
		return Quat(
			m_value[3] * q.m_value[3] - m_value[0] * q.m_value[0] - m_value[1] * q.m_value[1] - m_value[2] * q.m_value[2],
			m_value[3] * q.m_value[0] + m_value[0] * q.m_value[3] + m_value[1] * q.m_value[2] - m_value[2] * q.m_value[1],
			m_value[3] * q.m_value[1] - m_value[0] * q.m_value[2] + m_value[1] * q.m_value[3] + m_value[2] * q.m_value[0],
			m_value[3] * q.m_value[2] + m_value[0] * q.m_value[1] - m_value[1] * q.m_value[0] + m_value[2] * q.m_value[3]);
	}
	Quat< T > operator*(T scalar) const
	{
		return Quat(m_value[3] * scalar, m_value[0] * scalar, m_value[1] * scalar, m_value[2] * scalar);
	}
	Quat< T > operator*(const vector3_t< T >& vec) const { return *this * Quat(0, vec.x, vec.y, vec.z); }

	bool operator==(const Quat& q) const
	{
		for (int i = 0; i < 4; i++)
		{
			if (m_value[i] != q.m_value[i])
				return false;
		}
		return true;
	}

	bool operator!=(const Quat& q) const { return !(*this == q); }

	explicit operator T() const
	{
		return std::sqrt(m_value[0] * m_value[0] + m_value[1] * m_value[1] + m_value[2] * m_value[2] + m_value[3] * m_value[3]);
	}

	T angle(bool inDegrees = true) const
	{
		T radianAngle = 2 * std::acos(m_value[3]);
		return inDegrees ? radianAngle : radianAngle * 180 / T(M_PI);
	}

	matrix_t< T > rotation_matrix() const
	{
		vector3_t< T > v1 = apply({ 1, 0, 0 }), v2 = apply({ 0, 1, 0 }), v3 = apply({ 0, 0, 1 });
		return matrix_t< T >{ v1.x, v1.y, v1.z, 0, v2.x, v2.y, v2.z, 0, v3.x, v3.y, v3.z, 0, 0, 0, 0, 1 };
	};

	matrix_t< T > matrix() const
	{
		return matrix_t< T >{
			m_value[3], -m_value[0], -m_value[1], -m_value[2], m_value[0], m_value[3],	-m_value[2], m_value[1],
			m_value[1], m_value[2],	 m_value[3],  -m_value[0], m_value[2], -m_value[1], m_value[0],	 m_value[3]
		};
	}

	vector3_t< T > apply(const vector3_t< T >& vec) const
	{
		T norm = T(*this);
		if (norm == 0)
			throw std::invalid_argument("Norm cannot be zero");

		Quat< T > normalizedQuat(m_value[3] / norm, m_value[0] / norm, m_value[1] / norm, m_value[2] / norm);

		Quat< T > invQuat = ~(normalizedQuat);
		Quat< T > resQuat = normalizedQuat * Quat< T >(0, vec.x, vec.y, vec.z) * invQuat;

		return { resQuat.m_value[0], resQuat.m_value[1], resQuat.m_value[2] };
	}
	const T* data() const { return m_value; };
};
