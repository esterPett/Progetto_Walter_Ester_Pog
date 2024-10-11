#include "Generale.h"

Mat4F::Mat4F()
{
	memset(mat4f, 0.f, sizeof(float) * 16);
	mat4f[0] = mat4f[5] = mat4f[10] = mat4f[15] = 1.f;
}

Mat4F::Mat4F(float val)
{
	memset(mat4f, 0.f, sizeof(float) * 16);
	mat4f[0] = mat4f[5] = mat4f[10] = mat4f[15] = val;
}

Mat4F::Mat4F(float v[16]) : mat4f(v) {}

Mat4F::Mat4F(float val0, float val1, float val2, float val3,
	float val4, float val5, float val6, float val7,
	float val8, float val9, float val10, float val11,
	float val12, float val13, float val14, float val15)
{
	mat4f[0] = val0;	mat4f[1] = val4;	mat4f[2] = val8;	mat4f[3] = val12;
	mat4f[4] = val1;	mat4f[5] = val5;	mat4f[6] = val9;	mat4f[7] = val13;
	mat4f[8] = val2;	mat4f[9] = val6;	mat4f[10] = val10;	mat4f[11] = val14;
	mat4f[12] = val3;	mat4f[13] = val7;	mat4f[14] = val11;	mat4f[15] = val15;
}

Mat4F::Mat4F(const Vec3F& u, const Vec3F& v, const Vec3F& h, const Vec3F& k)
{
	mat4f[0] = u.x;		mat4f[1] = u.y;		mat4f[2] = u.z;		mat4f[3] = 0.f;
	mat4f[4] = v.x;		mat4f[5] = v.y;		mat4f[6] = v.z;		mat4f[7] = 0.f;
	mat4f[8] = h.x;		mat4f[9] = h.y;		mat4f[10] = h.z;	mat4f[11] = 0.f;
	mat4f[12] = k.x;	mat4f[13] = k.y;	mat4f[14] = k.z;	mat4f[15] = 1.f;
}

Mat4F::Mat4F(const Vec4F& u, const Vec4F& v, const Vec4F& h, const Vec4F& k)
{
	mat4f[0] = u.x;		mat4f[1] = u.y;		mat4f[2] = u.z;		mat4f[3] = u.w;
	mat4f[4] = v.x;		mat4f[5] = v.y;		mat4f[6] = v.z;		mat4f[7] = v.w;
	mat4f[8] = h.x;		mat4f[9] = h.y;		mat4f[10] = h.z;	mat4f[11] = h.w;
	mat4f[12] = k.x;	mat4f[13] = k.y;	mat4f[14] = k.z;	mat4f[15] = k.w;
}

Mat4F::Mat4F(const Mat4F& otherMat4f)
{
	memcpy(mat4f, otherMat4f.mat4f, sizeof(float) * 16);
}

Mat4F::Mat4F(Mat4F&& otherMat4f)noexcept
{
	delete[] mat4f;

	mat4f = otherMat4f.mat4f;
	otherMat4f.mat4f = nullptr;
}

Mat4F::~Mat4F()
{
	delete[] mat4f;
}
//Passa da gradi a radianti (Metterla su un file.h per operazioni matematiche)
float toRad(float deg)
{
	return deg * M_PI / 180.0f;
}

Mat4F Mat4F::operator*(const Mat4F& otherMat4f)const
{
	Mat4F tmp;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tmp(j, i) = otherMat4f.mat4f[4 * i] * mat4f[j] + otherMat4f.mat4f[4 * i + 1] * mat4f[j + 4] +
				otherMat4f.mat4f[4 * i + 2] * mat4f[j + 8] + otherMat4f.mat4f[4 * i + 3] * mat4f[j + 12];
		}
	}
	return tmp;
}

Mat4F& Mat4F::operator*=(const Mat4F& otherMat4f)
{
	Mat4F tmp;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tmp(j, i) = otherMat4f.mat4f[4 * i] * mat4f[j] + otherMat4f.mat4f[4 * i + 1] * mat4f[j + 4] +
				otherMat4f.mat4f[4 * i + 2] * mat4f[j + 8] + otherMat4f.mat4f[4 * i + 3] * mat4f[j + 12];
		}
	}
	return *this = tmp;
}

Vec4F Mat4F::operator*(const Vec4F& vec4f)const
{
	Vec4F tmp(mat4f[0] * vec4f.x + mat4f[4] * vec4f.y + mat4f[8] * vec4f.z + mat4f[12] * vec4f.w,
		mat4f[1] * vec4f.x + mat4f[5] * vec4f.y + mat4f[9] * vec4f.z + mat4f[13] * vec4f.w,
		mat4f[2] * vec4f.x + mat4f[6] * vec4f.y + mat4f[10] * vec4f.z + mat4f[14] * vec4f.w,
		mat4f[3] * vec4f.x + mat4f[7] * vec4f.y + mat4f[11] * vec4f.z + mat4f[15] * vec4f.w);

	return tmp;
}

Mat4F& Mat4F::operator=(const Mat4F& otherMat4f)
{
	memcpy(mat4f, otherMat4f.mat4f, sizeof(float) * 16);

	return *this;
}

Mat4F& Mat4F::operator=(Mat4F&& otherMat4f)noexcept
{
	delete[] mat4f; mat4f = otherMat4f.mat4f;

	otherMat4f.mat4f = nullptr;

	return *this;
}

float& Mat4F::operator()(int row, int col)
{
	return mat4f[col * 4 + row];
}


Vec4F Mat4F::row(int r)const
{
	return Vec4F(mat4f[r], mat4f[r + 4], mat4f[r + 8], mat4f[r + 12]);
}

Vec4F Mat4F::col(int c)const
{
	return Vec4F(mat4f[c * 4], mat4f[c * 4 + 1], mat4f[c * 4 + 2], mat4f[c * 4 + 3]);
}

Mat4F Mat4F::transpose(const Mat4F& otherMat4f)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < i; j++) {
			float temp = otherMat4f.mat4f[i + j * 4];
			otherMat4f.mat4f[i + j * 4] = otherMat4f.mat4f[j + i * 4];
			otherMat4f.mat4f[j + i * 4] = temp;
		}
	}

	return Mat4F(otherMat4f);
}

Mat4F Mat4F::inverse(const Mat4F& otherMat4f) //Utiliziamo il metodo di inversione di Gauss
{
	int i, j, k;

	float a[32] = { otherMat4f.mat4f[0], otherMat4f.mat4f[4], otherMat4f.mat4f[8], otherMat4f.mat4f[12], 1.f , 0.f , 0.f , 0.f
				   ,otherMat4f.mat4f[1], otherMat4f.mat4f[5], otherMat4f.mat4f[9], otherMat4f.mat4f[13], 0.f , 1.f , 0.f , 0.f
				   ,otherMat4f.mat4f[2], otherMat4f.mat4f[6], otherMat4f.mat4f[10], otherMat4f.mat4f[14], 0.f , 0.f , 1.f , 0.f
				   ,otherMat4f.mat4f[3], otherMat4f.mat4f[7], otherMat4f.mat4f[11], otherMat4f.mat4f[15], 0.f , 0.f , 0.f , 1.f };

	float d;

	/************************************************************************************/

	for (i = 24; i >= 8; i -= 8)
	{
		if (a[i - 8] < a[i])
		{
			for (j = i; j < i + 8; j++)
			{
				swap(a[j], a[j - 8]);
			}
		}
	}

	/************************************************************************************/

	for (k = 0, i = 0; i <= 24; i += 8, k++)
	{
		for (j = 0; j <= 24; j += 8)
		{
			if (j != i)
			{
				d = a[j + k] / a[i + k];
				for (int l = i, k = j; k < j + 8; k++, l++)
				{
					a[k] -= a[l] * d;
				}
			}
		}
	}

	/************************************************************************************/

	for (k = 0, i = 0; i <= 24; i += 8, k++)
	{
		d = 1.f / a[i + k];
		for (j = i; j < i + 8; j++)
		{
			a[j] *= d;
		}
	}

	return *this = Mat4F(a[4], a[5], a[6], a[7]
		, a[12], a[13], a[14], a[15]
		, a[20], a[21], a[22], a[23]
		, a[28], a[29], a[30], a[31]);
}

Mat4F Mat4F::translation(Mat4F&& m, float a, float b, float c)
{

	Mat4F trasl = Mat4F(1,0,0,a,
						0,1,0,b,
						0,0,1,c,
						0,0,0,1);

	Mat4F calc = m * trasl;

	return calc;
}

Mat4F Mat4F::scaling(Mat4F&& m, float scale)
{
	Mat4F scal = Mat4F(scale, 0, 0, 0,
						0, scale, 0, 0,
						0, 0, scale, 0,
						0, 0, 0, 1);

	Mat4F calc = m * scal;

	return calc;
}

Mat4F Mat4F::rotationX(Mat4F m, float deg)
{
	float sinT = sin(toRad(deg));
	float cosT = cos(toRad(deg));

	Mat4F rotX = Mat4F(1.f, 0.f, 0.f, 0.f
					, 0.f, cosT, -sinT, 0.f
					, 0.f, sinT, cosT, 0.f
					, 0.f, 0.f, 0.f, 1.f);

	Mat4F calc = m * rotX;

	return calc;
}

Mat4F Mat4F::rotationY(Mat4F m,float deg)
{
	float sinT = sin(toRad(deg));
	float cosT = cos(toRad(deg));

	Mat4F rotY = Mat4F(cosT, 0.f, -sinT, 0.f
					, 0.f, 1.f, 0.f, 0.f
					, sinT, 0.f, cosT, 0.f
					, 0.f, 0.f, 0.f, 1.f);

	Mat4F calc = m * rotY;

	return calc;
}

Mat4F Mat4F::rotationZ(Mat4F m,float deg)
{
	float sinT = sin(toRad(deg));
	float cosT = cos(toRad(deg));

	Mat4F rotZ = Mat4F(cosT, -sinT, 0.f, 0.f
					, sinT, cosT, 0.f, 0.f
					, 0.f, 0.f,1.f, 0.f
					, 0.f, 0.f, 0.f, 1.f);

	Mat4F calc = m * rotZ;

	return calc;
}


Mat4F Mat4F::rotateAllAxis(Mat4F&& model, float angle, Vec3F&& axis) {
	float radians = toRad(angle);
	float cosA = cos(radians);
	float sinA = sin(radians);
	float oneMinusCosA = 1.0f - cosA;

	// Crea la matrice di rotazione
	Mat4F rotationMatrix;

	// Calcola gli elementi della matrice di rotazione
	rotationMatrix(0, 0) = cosA + (axis.x * axis.x) * oneMinusCosA;
	rotationMatrix(0, 1) = axis.x * axis.y * oneMinusCosA - axis.z * sinA;
	rotationMatrix(0, 2) = axis.x * axis.z * oneMinusCosA + axis.y * sinA;

	rotationMatrix(1, 0) = axis.y * axis.x * oneMinusCosA + axis.z * sinA;
	rotationMatrix(1, 1) = cosA + (axis.y * axis.y) * oneMinusCosA;
	rotationMatrix(1, 2) = axis.y * axis.z * oneMinusCosA - axis.x * sinA;

	rotationMatrix(2, 0) = axis.z * axis.x * oneMinusCosA - axis.y * sinA;
	rotationMatrix(2, 1) = axis.z * axis.y * oneMinusCosA + axis.x * sinA;
	rotationMatrix(2, 2) = cosA + (axis.z * axis.z) * oneMinusCosA;

	// Ora moltiplica la matrice di rotazione per la matrice model
	Mat4F result = model * rotationMatrix;

	return result;
}

Mat4F Mat4F::projectionMat4F(const float& angleOfView, const float& near, const float& far)
{
	float S = 1 / tan((angleOfView / 2) * (M_PI / 180));

	return Mat4F::Mat4F(S, 0, 0, 0,
						0,S*near,0,0,
						0,0,- (far / (far-near)), -1,
						0,0,- (far * near) / (far - near), 0);
}

Mat4F Mat4F::lookat(Vec3F&& from,Vec3F&& to,Vec3F&& up)
{
	Vec3F forward = (from - to).normalize();
	Vec3F right = (up ^ forward).normalize();
	Vec3F newup = (forward ^ right).normalize();

	return Mat4F(
				right.x,	right.y,	 right.z,    -(right * from),
				newup.x,	newup.y,	 newup.z,    -(newup * from),
				forward.x,	forward.y,   forward.z,  -(forward * from),
					0,		   0,			0,		    	1
	);
}

ostream& operator<<(ostream& output, const Mat4F& mat4f)
{
	if (mat4f.mat4f) {
		output << "Matrix: \n";
		for (int i = 0; i < 4; i++)
		{
			output << "|" << mat4f.mat4f[i] << "\t" << mat4f.mat4f[i + 4] << "\t"
				<< mat4f.mat4f[i + 8] << "\t" << mat4f.mat4f[i + 12] << "|" << endl;
		}
	}
	else
	{
		output << "Matrice vuota" << endl;
	}

	return output;
}


