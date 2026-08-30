/*
 * Minimal D3DX9 math header for compiling legacy Direct3D 9 code
 * without requiring the full legacy DirectX SDK.
 *
 * This header provides minimal declarations of D3DX9 math types
 * and functions used by the MilkDrop 3 visualizer.
 *
 * Source/License: Minimal stub based on Microsoft Direct3D 9 SDK (deprecated).
 * Licensed under the same terms as the original D3D9 headers.
 */

#ifndef __D3DX9MATH_H__
#define __D3DX9MATH_H__

#include <d3d9.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/* D3DXVECTOR3 - 3D vector */
typedef struct D3DXVECTOR3 {
    float x, y, z;
} D3DXVECTOR3;

/* D3DXVECTOR4 - 4D vector */
typedef struct D3DXVECTOR4 {
    float x, y, z, w;
} D3DXVECTOR4;

/* D3DXMATRIX - 4x4 matrix */
typedef struct D3DXMATRIX {
    float m[4][4];
} D3DXMATRIX;

/* Matrix functions */
D3DXMATRIX* WINAPI D3DXMatrixIdentity(D3DXMATRIX* pOut);
D3DXMATRIX* WINAPI D3DXMatrixLookAtLH(
    D3DXMATRIX* pOut,
    const D3DXVECTOR3* pEye,
    const D3DXVECTOR3* pAt,
    const D3DXVECTOR3* pUp
);

D3DXMATRIX* WINAPI D3DXMatrixOrthoLH(
    D3DXMATRIX* pOut,
    float w,
    float h,
    float zn,
    float zf
);

D3DXMATRIX* WINAPI D3DXMatrixPerspectiveFovLH(
    D3DXMATRIX* pOut,
    float fovy,
    float aspect,
    float zn,
    float zf
);

D3DXMATRIX* WINAPI D3DXMatrixRotationX(D3DXMATRIX* pOut, float angle);
D3DXMATRIX* WINAPI D3DXMatrixRotationY(D3DXMATRIX* pOut, float angle);
D3DXMATRIX* WINAPI D3DXMatrixRotationZ(D3DXMATRIX* pOut, float angle);

D3DXMATRIX* WINAPI D3DXMatrixRotationAxis(
    D3DXMATRIX* pOut,
    const D3DXVECTOR3* pV,
    float angle
);

D3DXMATRIX* WINAPI D3DXMatrixScaling(
    D3DXMATRIX* pOut,
    float sx,
    float sy,
    float sz
);

D3DXMATRIX* WINAPI D3DXMatrixTranslation(
    D3DXMATRIX* pOut,
    float x,
    float y,
    float z
);

D3DXMATRIX* WINAPI D3DXMatrixMultiply(
    D3DXMATRIX* pOut,
    const D3DXMATRIX* pM1,
    const D3DXMATRIX* pM2
);

D3DXMATRIX* WINAPI D3DXMatrixInverse(
    D3DXMATRIX* pOut,
    float* pDeterminant,
    const D3DXMATRIX* pM
);

D3DXMATRIX* WINAPI D3DXMatrixTranspose(
    D3DXMATRIX* pOut,
    const D3DXMATRIX* pM
);

/* Vector functions */
D3DXVECTOR3* WINAPI D3DXVec3Normalize(
    D3DXVECTOR3* pOut,
    const D3DXVECTOR3* pV
);

float WINAPI D3DXVec3Length(const D3DXVECTOR3* pV);
float WINAPI D3DXVec3LengthSq(const D3DXVECTOR3* pV);
float WINAPI D3DXVec3Dot(const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2);

D3DXVECTOR3* WINAPI D3DXVec3Cross(
    D3DXVECTOR3* pOut,
    const D3DXVECTOR3* pV1,
    const D3DXVECTOR3* pV2
);

/* Transform functions */
D3DXVECTOR3* WINAPI D3DXVec3TransformCoord(
    D3DXVECTOR3* pOut,
    const D3DXVECTOR3* pV,
    const D3DXMATRIX* pM
);

D3DXVECTOR3* WINAPI D3DXVec3TransformNormal(
    D3DXVECTOR3* pOut,
    const D3DXVECTOR3* pV,
    const D3DXMATRIX* pM
);

D3DXVECTOR4* WINAPI D3DXVec3Transform(
    D3DXVECTOR4* pOut,
    const D3DXVECTOR3* pV,
    const D3DXMATRIX* pM
);

#ifdef __cplusplus
}
#endif

#endif /* __D3DX9MATH_H__ */
