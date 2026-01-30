#include "pch.h"

cpu_mesh::cpu_mesh()
{
	Clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cpu_mesh::Clear()
{
	triangles.clear();
	radius = 0.0f;
	aabb.Zero();
}

void cpu_mesh::AddMesh(cpu_mesh& mesh)
{
	for ( cpu_triangle& tri : mesh.triangles )
		triangles.push_back(tri);
}

void cpu_mesh::AddTriangle(cpu_triangle& tri)
{
	triangles.push_back(tri);
}

void cpu_mesh::AddTriangle(XMFLOAT3& a, XMFLOAT3& b, XMFLOAT3& c, XMFLOAT3& color)
{
	cpu_triangle t;
	t.v[0].pos = a;
	t.v[0].color = color;
	t.v[1].pos = b;
	t.v[1].color = color;
	t.v[2].pos = c;
	t.v[2].color = color;
	triangles.push_back(t);
}

void cpu_mesh::AddTriangle(XMFLOAT3& a, XMFLOAT3& b, XMFLOAT3& c, XMFLOAT2& auv, XMFLOAT2& buv, XMFLOAT2& cuv, XMFLOAT3& color)
{
	cpu_triangle t;
	t.v[0].pos = a;
	t.v[0].color = color;
	t.v[0].uv = auv;
	t.v[1].pos = b;
	t.v[1].color = color;
	t.v[1].uv = buv;
	t.v[2].pos = c;
	t.v[2].color = color;
	t.v[2].uv = cuv;
	triangles.push_back(t);
}

void cpu_mesh::AddFace(XMFLOAT3& a, XMFLOAT3& b, XMFLOAT3& c, XMFLOAT3& d, XMFLOAT3& color)
{
	AddTriangle(a, b, c, color);
	AddTriangle(a, c, d, color);
}

void cpu_mesh::AddFace(XMFLOAT3& a, XMFLOAT3& b, XMFLOAT3& c, XMFLOAT3& d, XMFLOAT2& auv, XMFLOAT2& buv, XMFLOAT2& cuv, XMFLOAT2& duv, XMFLOAT3& color)
{
	AddTriangle(a, b, c, auv, buv, cuv, color);
	AddTriangle(a, c, d, auv, cuv, duv, color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cpu_mesh::Optimize()
{
	CalculateNormals();
	CalculateBox();
}

void cpu_mesh::CalculateNormals()
{
	std::map<XMFLOAT3, XMVECTOR, cpu_vec3_cmp> normalAccumulator;
	for ( cpu_triangle& t : triangles )
	{
		XMVECTOR p0 = XMLoadFloat3(&t.v[0].pos);
		XMVECTOR p1 = XMLoadFloat3(&t.v[1].pos);
		XMVECTOR p2 = XMLoadFloat3(&t.v[2].pos);

		XMVECTOR edge1 = XMVectorSubtract(p1, p0);
		XMVECTOR edge2 = XMVectorSubtract(p2, p0);
		XMVECTOR faceNormal = XMVector3Cross(edge1, edge2);
		
		if ( normalAccumulator.count(t.v[0].pos)==0 )
			normalAccumulator[t.v[0].pos] = XMVectorZero();
		if ( normalAccumulator.count(t.v[1].pos)==0 )
			normalAccumulator[t.v[1].pos] = XMVectorZero();
		if ( normalAccumulator.count(t.v[2].pos)==0 )
			normalAccumulator[t.v[2].pos] = XMVectorZero();

		normalAccumulator[t.v[0].pos] = XMVectorAdd(normalAccumulator[t.v[0].pos], faceNormal);
		normalAccumulator[t.v[1].pos] = XMVectorAdd(normalAccumulator[t.v[1].pos], faceNormal);
		normalAccumulator[t.v[2].pos] = XMVectorAdd(normalAccumulator[t.v[2].pos], faceNormal);
	}
	for ( cpu_triangle& t : triangles )
	{
		for ( int i=0 ; i<3 ; i++ )
		{
			XMVECTOR sumNormal = normalAccumulator[t.v[i].pos];
			sumNormal = XMVector3Normalize(sumNormal);
			XMStoreFloat3(&t.v[i].normal, sumNormal);
		}
	}
}

void cpu_mesh::CalculateBox()
{
	aabb.min.x = FLT_MAX;
	aabb.min.y = FLT_MAX;
	aabb.min.z = FLT_MAX;
	aabb.max.x = -FLT_MAX;
	aabb.max.y = -FLT_MAX;
	aabb.max.z = -FLT_MAX;

	for ( cpu_triangle& t : triangles )
	{
		for ( int i=0 ; i<3 ; i++ )
		{
			if ( t.v[i].pos.x<aabb.min.x )
				aabb.min.x = t.v[i].pos.x;
			if ( t.v[i].pos.y<aabb.min.y )
				aabb.min.y = t.v[i].pos.y;
			if ( t.v[i].pos.z<aabb.min.z )
				aabb.min.z = t.v[i].pos.z;
				
			if ( t.v[i].pos.x>aabb.max.x )
				aabb.max.x = t.v[i].pos.x;
			if ( t.v[i].pos.y>aabb.max.y )
				aabb.max.y = t.v[i].pos.y;
			if ( t.v[i].pos.z>aabb.max.z )
				aabb.max.z = t.v[i].pos.z;
		}
	}

	float fx = std::max(fabsf(aabb.min.x), fabsf(aabb.max.x));
	float fy = std::max(fabsf(aabb.min.y), fabsf(aabb.max.y));
	float fz = std::max(fabsf(aabb.min.z), fabsf(aabb.max.z));
	float r2 = fx*fx + fy*fy + fz*fz;
	radius = sqrtf(r2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cpu_mesh::CreatePlane(float width, float height, XMFLOAT3 color)
{
	Clear();
	const float w = width*0.5f;
	const float h = height*0.5f;
	XMFLOAT3 p0 = { -w, -h, 0.0f };
	XMFLOAT3 p1 = {  w, -h, 0.0f };
	XMFLOAT3 p2 = {  w,  h, 0.0f };
	XMFLOAT3 p3 = { -w,  h, 0.0f };
	XMFLOAT2 t0 = { 0.0f, 0.0f };
	XMFLOAT2 t1 = { 1.0f, 0.0f };
	XMFLOAT2 t2 = { 1.0f, 1.0f };
	XMFLOAT2 t3 = { 0.0f, 1.0f };
	AddFace(p0, p1, p2, p3, t0, t1, t2, t3, color);
	Optimize();
}

void cpu_mesh::CreateCube(float halfSize, XMFLOAT3 color)
{
	Clear();
	const float s = halfSize; 
	XMFLOAT3 p0 = { -s, -s, -s };
	XMFLOAT3 p1 = {  s, -s, -s };
	XMFLOAT3 p2 = {  s,  s, -s };
	XMFLOAT3 p3 = { -s,  s, -s };
	XMFLOAT3 p4 = { -s, -s,  s };
	XMFLOAT3 p5 = {  s, -s,  s };
	XMFLOAT3 p6 = {  s,  s,  s };
	XMFLOAT3 p7 = { -s,  s,  s };
	XMFLOAT2 tl = { 0.0f, 0.0f };					// top-left
	XMFLOAT2 tr = { 1.0f, 0.0f };					// top-right
	XMFLOAT2 br = { 1.0f, 1.0f };					// bottom-right
	XMFLOAT2 bl = { 0.0f, 1.0f };					// bottom-left
	AddFace(p0, p1, p2, p3, bl, br, tr, tl, color);	// -Z (back)
	AddFace(p4, p7, p6, p5, bl, tl, tr, br, color);	// +Z (front)
	AddFace(p1, p5, p6, p2, bl, br, tr, tl, color);	// +X (droite)
	AddFace(p4, p0, p3, p7, bl, br, tr, tl, color);	// -X (gauche)
	AddFace(p3, p2, p6, p7, bl, br, tr, tl, color);	// +Y (haut)
	AddFace(p4, p5, p1, p0, bl, br, tr, tl, color);	// -Y (bas)
	Optimize();
}

void cpu_mesh::CreateSkyBox(float halfSize, XMFLOAT3 color)
{
	Clear();
	const float s = halfSize;
	XMFLOAT3 p0 = { -s, -s, -s };
	XMFLOAT3 p1 = {  s, -s, -s };
	XMFLOAT3 p2 = {  s,  s, -s };
	XMFLOAT3 p3 = { -s,  s, -s };
	XMFLOAT3 p4 = { -s, -s,  s };
	XMFLOAT3 p5 = {  s, -s,  s };
	XMFLOAT3 p6 = {  s,  s,  s };
	XMFLOAT3 p7 = { -s,  s,  s };
	XMFLOAT2 tl = { 0.0f, 0.0f };					// top-left
	XMFLOAT2 tr = { 1.0f, 0.0f };					// top-right
	XMFLOAT2 br = { 1.0f, 1.0f };					// bottom-right
	XMFLOAT2 bl = { 0.0f, 1.0f };					// bottom-left
	AddFace(p3, p2, p1, p0, tl, tr, br, bl, color); // -Z
	AddFace(p5, p6, p7, p4, br, tr, tl, bl, color); // +Z
	AddFace(p2, p6, p5, p1, tl, tr, br, bl, color); // +X
	AddFace(p7, p3, p0, p4, tl, tr, br, bl, color); // -X
	AddFace(p7, p6, p2, p3, tl, tr, br, bl, color); // +Y
	AddFace(p0, p1, p5, p4, tl, tr, br, bl, color); // -Y
	Optimize();
}

void cpu_mesh::CreateCircle(float radius, int count, XMFLOAT3 color)
{
	if ( count<3 )
		return;

	Clear();
	float radius2 = radius * 2.0f;
	float step = XM_2PI / count;
	float angle = 0.0f;
	XMFLOAT3 p1, p2, p3;
	p1.x = 0.0f;
	p1.y = 0.0f;
	p1.z = 0.0f;
	p2.y = 0.0f;
	p3.y = 0.0f;
	XMFLOAT2 t1, t2, t3;
	t1.x = 0.5f;
	t1.y = 0.5f;
	for ( int i=0 ; i<count ; i++ )
	{
		p2.x = cosf(angle) * radius;
		p2.z = sinf(angle) * radius;
		p3.x = cosf(angle+step) * radius;
		p3.z = sinf(angle+step) * radius;
		t2.x = 0.5f + (p2.x / radius2);
		t2.y = 0.5f - (p2.z / radius2);
		t3.x = 0.5f + (p3.x / radius2);
		t3.y = 0.5f - (p3.z / radius2);
		AddTriangle(p1, p2, p3, t1, t2, t3, color);
		angle += step;
	}
	Optimize();
}

void cpu_mesh::CreateSphere(float radius, int stacks, int slices, XMFLOAT3 color1, XMFLOAT3 color2)
{
	Clear();
	if ( stacks<2 ) stacks = 2; // minimum pour avoir un haut et un bas
	if ( slices<3 ) slices = 3; // minimum pour fermer correctement
	for ( int i=0 ; i<stacks ; ++i )
	{
		// theta0/theta1 d�limitent une bande (du haut vers le bas)
		const float v0 = (float)i / (float)stacks;
		const float v1 = (float)(i + 1) / (float)stacks;
		const float theta0 = v0 * XM_PI;
		const float theta1 = v1 * XM_PI;
		for ( int j=0,k=0 ; j<slices ; ++j )
		{
			const float u0 = (float)j / (float)slices;
			const float u1 = (float)(j + 1) / (float)slices;
			const float phi0 = u0 * XM_2PI;
			const float phi1 = u1 * XM_2PI;

			// 4 coins du quad (bande i, secteur j)
			// p00 = (theta0, phi0)
			// p01 = (theta0, phi1)
			// p10 = (theta1, phi0)
			// p11 = (theta1, phi1)
			XMFLOAT3 p00 = cpu::SphericalPoint(radius, theta0, phi0);
			XMFLOAT3 p01 = cpu::SphericalPoint(radius, theta0, phi1);
			XMFLOAT3 p10 = cpu::SphericalPoint(radius, theta1, phi0);
			XMFLOAT3 p11 = cpu::SphericalPoint(radius, theta1, phi1);
			XMFLOAT3& color = k==0 ? color1 : color2;

			XMFLOAT2 uv00 = { u0, v0 };
			XMFLOAT2 uv01 = { u1, v0 };
			XMFLOAT2 uv10 = { u0, v1 };
			XMFLOAT2 uv11 = { u1, v1 };

			// Triangles d�g�n�r�s (theta = 0 ou PI)
			const bool topBand = i==0;
			const bool bottomBand = i==stacks-1;
			if ( topBand )
			{
				// Au p�le nord, p00 et p01 sont quasiment identiques (theta0=0).
				// Triangle orient� vers l'ext�rieur (CCW vu de l'ext�rieur).
				// On utilise: p00 (sommet), p10 (bas gauche), p11 (bas droite)
				AddTriangle(p00, p10, p11, uv00, uv10, uv11, color);
			}
			else if ( bottomBand )
			{
				// Au p�le sud, p10 et p11 sont quasiment identiques (theta1=PI).
				// On utilise: p10 (sommet bas), p01 (haut droite), p00 (haut gauche)
				AddTriangle(p10, p01, p00, uv10, uv01, uv00, color);
			}
			else
			{
				// Bande interm�diaire : 2 triangles pour le quad
				// Winding CCW (vu depuis l'ext�rieur)
				AddTriangle(p00, p10, p11, uv00, uv10, uv11, const_cast<XMFLOAT3&>(color));
				AddTriangle(p00, p11, p01, uv00, uv11, uv01, const_cast<XMFLOAT3&>(color));
			}

			if ( ++k==2 )
				k = 0;
		}
	}
	Optimize();
}

void cpu_mesh::CreateSpaceship()
{
	Clear();
	const float width = 2.0f;
	XMFLOAT3 nose = { 0.0f, 0.0f, 1.5f };
	XMFLOAT3 rTop = { 0.0f, 0.5f, -1.0f };
	XMFLOAT3 rBot = { 0.0f, -0.3f, -1.0f };
	XMFLOAT3 wLeft = { -width*0.5f, 0.0f, -1.0f };
	XMFLOAT3 wRight = { width*0.5f, 0.0f, -1.0f };

	XMFLOAT2 noseUV		= { 0.5f, 0.0f };
	XMFLOAT2 rTopUV		= { 0.5f, 0.4f };
	XMFLOAT2 rBotUV		= { 0.5f, 0.8f };
	XMFLOAT2 wLeftUV	= { 0.0f, 0.6f };
	XMFLOAT2 wRightUV	= { 1.0f, 0.6f };

	XMFLOAT3 c1 = cpu::ToColor(208, 208, 208);
	XMFLOAT3 c2 = cpu::ToColor(192, 192, 192);
	XMFLOAT3 c3 = cpu::ToColor(112, 112, 112);
	XMFLOAT3 c4 = cpu::ToColor(96, 96, 96);
	XMFLOAT3 c5 = cpu::ToColor(255, 255, 255);
	XMFLOAT3 c6 = cpu::ToColor(255, 255, 255);

	AddTriangle(nose, wLeft, rTop, noseUV, wLeftUV, rTopUV, c1);		// Avant gauche haut
	AddTriangle(nose, rTop, wRight, noseUV, rTopUV, wRightUV, c2);		// Avant droit haut
	AddTriangle(nose, rBot, wLeft, noseUV, rBotUV, wLeftUV, c3);		// Avant gauche bas
	AddTriangle(nose, wRight, rBot, noseUV, wRightUV, rBotUV, c4);		// Avant droit bas
	AddTriangle(wLeft, rBot, rTop, wLeftUV, rBotUV, rTopUV, c5);		// Moteur gauche
	AddTriangle(wRight, rTop, rBot, wRightUV, rTopUV, rBotUV, c6);		// Moteur droit
	Optimize();
}

void cpu_mesh::CreateBoatHull(float length, float width, float height, XMFLOAT3 color)
{
	Clear();

	const float L = length;
	const float W = width;
	const float H = height;

	const float zFront = L * 0.5f;
	const float zBack = -L * 0.5f;
	const float zMidF = L * 0.25f;
	const float zMidB = -L * 0.25f;

	// ================= BLOC CENTRAL =================

	XMFLOAT3 ftl = { -W * 0.5f, H, zMidF };
	XMFLOAT3 ftr = { W * 0.5f, H, zMidF };
	XMFLOAT3 fbl = { -W * 0.5f, 0, zMidF };
	XMFLOAT3 fbr = { W * 0.5f, 0, zMidF };

	XMFLOAT3 btl = { -W * 0.5f, H, zMidB };
	XMFLOAT3 btr = { W * 0.5f, H, zMidB };
	XMFLOAT3 bbl = { -W * 0.5f, 0, zMidB };
	XMFLOAT3 bbr = { W * 0.5f, 0, zMidB };

	AddFace(ftl, btl, btr, ftr, color);
	AddFace(fbl, fbr, bbr, bbl, color);
	AddFace(btl, ftl, fbl, bbl, color);
	AddFace(ftr, btr, bbr, fbr, color);

	// ================= PROUE =================

	XMFLOAT3 bowTop = { 0, H, zFront };
	XMFLOAT3 bowBottom = { 0, H * 0.25f, zFront };

	AddTriangle(ftl, ftr, bowTop, color);
	AddTriangle(fbl, bowBottom, fbr, color);
	AddTriangle(ftl, bowTop, bowBottom, color);
	AddTriangle(ftl, bowBottom, fbl, color);
	AddTriangle(ftr, fbr, bowBottom, color);
	AddTriangle(ftr, bowBottom, bowTop, color);

	// ================= POUPE =================

	XMFLOAT3 sternTop = { 0, H, zBack };
	XMFLOAT3 sternBottom = { 0, H * 0.25f, zBack };

	AddTriangle(btr, btl, sternTop, color);
	AddTriangle(bbl, bbr, sternBottom, color);
	AddTriangle(btl, sternBottom, sternTop, color);
	AddTriangle(btl, bbl, sternBottom, color);
	AddTriangle(btr, sternTop, sternBottom, color);
	AddTriangle(btr, sternBottom, bbr, color);

	Optimize();
}

void cpu_mesh::CreateCannon(float barrelLength, float barrelSize, float offsetY, XMFLOAT3 color)
{
	Clear();

	const float L = barrelLength;
	const float S = barrelSize;
	const float r = S * 0.5f;

	const float zFront = L * 0.8f;
	const float zBack = -L * 0.19f;

	// ================= TUBE =================

	XMFLOAT3 ftl = { -r,  r + offsetY, zFront };
	XMFLOAT3 ftr = { r,  r + offsetY, zFront };
	XMFLOAT3 fbl = { -r, -r + offsetY, zFront };
	XMFLOAT3 fbr = { r, -r + offsetY, zFront };

	XMFLOAT3 btl = { -r,  r + offsetY, zBack };
	XMFLOAT3 btr = { r,  r + offsetY, zBack };
	XMFLOAT3 bbl = { -r, -r + offsetY, zBack };
	XMFLOAT3 bbr = { r, -r + offsetY, zBack };

	AddFace(btl, btr, ftr, ftl, color);
	AddFace(bbr, bbl, fbl, fbr, color);
	AddFace(ftl, fbl, bbl, btl, color);
	AddFace(fbr, ftr, btr, bbr, color);
	AddFace(fbl, ftl, ftr, fbr, color);
	AddFace(bbr, btr, btl, bbl, color);

	// ================= BASE =================

	float baseW = S * 2.5f;
	float baseH = S;
	float baseL = L * 0.4f;

	float bzF = -baseL * 0.5f;
	float bzB = baseL * 0.5f;

	XMFLOAT3 s0 = { -baseW * 0.5f, -baseH + offsetY, bzF };
	XMFLOAT3 s1 = { baseW * 0.5f, -baseH + offsetY, bzF };
	XMFLOAT3 s2 = { baseW * 0.5f, -baseH + offsetY, bzB };
	XMFLOAT3 s3 = { -baseW * 0.5f, -baseH + offsetY, bzB };

	XMFLOAT3 s4 = { -baseW * 0.5f,  offsetY, bzF };
	XMFLOAT3 s5 = { baseW * 0.5f,  offsetY, bzF };
	XMFLOAT3 s6 = { baseW * 0.5f,  offsetY, bzB };
	XMFLOAT3 s7 = { -baseW * 0.5f,  offsetY, bzB };
	
	AddFace(s4, s5, s6, s7, color);
	AddFace(s0, s3, s2, s1, color);
	AddFace(s0, s4, s7, s3, color);
	AddFace(s5, s1, s2, s6, color);
	AddFace(s4, s0, s1, s5, color);
	AddFace(s7, s6, s2, s3, color);
	
	Optimize();
}

void cpu_mesh::CreateIcebergBlock(float length, float width, float height, XMFLOAT3 color)
{
	Clear();

	float L = length;
	float W = width;
	float H = height;

	float zF = L * 0.5f;
	float zB = -L * 0.5f;

	float topW = W * 0.6f;
	float topOffsetX = W * 0.15f;
	float topOffsetZ = L * 0.1f;

	// ===== BASE (ligne d�eau, irr�guli�re) =====
	XMFLOAT3 b0 = { -W * 0.5f, 0, zF };
	XMFLOAT3 b1 = { W * 0.5f, 0, zF };
	XMFLOAT3 b2 = { W * 0.5f, 0, zB };
	XMFLOAT3 b3 = { -W * 0.5f, 0, zB };

	// ===== SOMMET =====
	XMFLOAT3 t0 = { -topW * 0.5f + topOffsetX, H, zF - topOffsetZ };
	XMFLOAT3 t1 = { topW * 0.5f + topOffsetX, H * 0.9f, zF };
	XMFLOAT3 t2 = { topW * 0.5f - topOffsetX, H * 1.1f, zB + topOffsetZ };
	XMFLOAT3 t3 = { -topW * 0.5f - topOffsetX, H * 0.95f, zB };

	AddFace(t1, t0, t3, t2, color);
	AddFace(t0, t1, b1, b0, color);
	AddFace(t2, t3, b3, b2, color);
	AddFace(t3, t0, b0, b3, color);
	AddFace(t1, t2, b2, b1, color);
	AddFace(b3, b0, b1, b2, color);

	Optimize();
}
