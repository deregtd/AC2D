#pragma once

class cPoint3D {
public:
	cPoint3D()
	{
	}

	cPoint3D(float nx, float ny, float nz)
	{
		x = nx;
		y = ny;
		z = nz;
	}

	bool operator ==(cPoint3D p3dOther)
	{
		if (x != p3dOther.x) return false;
		if (y != p3dOther.y) return false;
		if (z != p3dOther.z) return false;
		return true;
	}

	bool operator !=(cPoint3D p3dOther)
	{
		return !((x == p3dOther.x) && (y == p3dOther.y) && (z == p3dOther.z));
	}

	cPoint3D operator *(float fOther)
	{
		return cPoint3D(x * fOther, y * fOther, z * fOther);
	}

	cPoint3D operator /(float fOther)
	{
		return cPoint3D(x / fOther, y / fOther, z / fOther);
	}

	cPoint3D operator +(cPoint3D p3dOther)
	{
		return cPoint3D(x + p3dOther.x, y + p3dOther.y, z + p3dOther.z);
	}

	cPoint3D operator -(cPoint3D p3dOther)
	{
		return cPoint3D(x - p3dOther.x, y - p3dOther.y, z - p3dOther.z);
	}

	void operator *=(float fOther)
	{
		x *= fOther;
		y *= fOther;
		z *= fOther;

		return; //true;
	}

	void operator /=(float fOther)
	{
		x /= fOther;
		y /= fOther;
		z /= fOther;

		return; //true;
	}

	void operator +=(cPoint3D p3dOther)
	{
		x += p3dOther.x;
		y += p3dOther.y;
		z += p3dOther.z;

		return; //true;
	}

	void operator -=(cPoint3D p3dOther)
	{
		x -= p3dOther.x;
		y -= p3dOther.y;
		z -= p3dOther.z;

		return; //true;
	}

	void RotateAround(cPoint3D Center, cPoint3D Rotation)
	{
		float tpx, tpy, tpz;
		
		x -= Center.x;
		y -= Center.y;
		z -= Center.z;
		
		tpy = y * (float) cos(Rotation.x) - z * (float) sin(Rotation.x);
		tpz = z * (float) cos(Rotation.x) + y * (float) sin(Rotation.x);

		z = tpz * (float) cos(Rotation.y) - x * (float) sin(Rotation.y) + Center.z;
		tpx = x * (float) cos(Rotation.y) + tpz * (float) sin(Rotation.y);
		
		x = tpx * (float) cos(Rotation.z) - tpy * (float) sin(Rotation.z) + Center.x;
		y = tpy * (float) cos(Rotation.z) + tpx * (float) sin(Rotation.z) + Center.y;
	}

	cPoint3D CrossProduct(cPoint3D p3dOther)
	{
		return cPoint3D(y*p3dOther.z - p3dOther.y*z, z*p3dOther.x - p3dOther.z*x, x*p3dOther.y - p3dOther.x*y);
	}

	float DotProduct(cPoint3D p3dOther)
	{
		return x*p3dOther.x + y*p3dOther.y + z*p3dOther.z;
	}

	void Normalize()
	{
		float fTemp = (float) Abs();
		x /= fTemp;
		y /= fTemp;
		z /= fTemp;
	}

	void CalcFromLocation(stLocation *Loc)
	{
		z = Loc->zOffset / 240.0f;

		DWORD dwBlockX = (Loc->landblock & 0xff000000) >> 24;
		DWORD dwBlockY = (Loc->landblock & 0x00ff0000) >> 16;

		if (dwBlockX < 3)
		{
			// dungeon, dwBlockX and dwBlockY remain constant - just use x and y
			x = Loc->xOffset / 240.0f;
			y = Loc->yOffset / 240.0f;
		}
		else
		{
			// outdoors
			x = (float) ((((float (dwBlockX)+1.0f) * 8.0f + (Loc->xOffset / 24.0)) - 1027.5) / 10.0f);
			y = (float) ((((float (dwBlockY)+1.0f) * 8.0f + (Loc->yOffset / 24.0)) - 1027.5) / 10.0f);
		}
	}

	float Abs()
	{
		return (float) sqrt(x*x+y*y+z*z);
	}

	float	x, y, z;
};

