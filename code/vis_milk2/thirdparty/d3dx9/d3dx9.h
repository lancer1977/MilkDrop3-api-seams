/*
 * Minimal D3DX9 header for compiling legacy Direct3D 9 code
 * without requiring the full legacy DirectX SDK.
 *
 * This header provides minimal declarations of the most commonly
 * used D3DX9 functions and types. For a complete implementation,
 * link against the d3dx9.lib import library (from the legacy
 * DirectX SDK or equivalent compatible binary).
 *
 * Source/License: Minimal stub based on Microsoft Direct3D 9 SDK (deprecated).
 * Licensed under the same terms as the original D3D9 headers.
 */

#ifndef __D3DX9_H__
#define __D3DX9_H__

#include <d3d9.h>
#include <d3dx9math.h>

#ifdef __cplusplus
extern "C" {
#endif

/* D3DX Constants */
#define D3DX_DEFAULT ((UINT)-1)
#define D3DX_FROM_FILE ((UINT)-3)
#define D3DX_DEFAULT_NONPOW2 ((UINT)-2)

/* Image info structure */
typedef struct D3DXIMAGE_INFO
{
    UINT                 Width;
    UINT                 Height;
    UINT                 Depth;
    UINT                 MipLevels;
    D3DFORMAT            Format;
    D3DRESOURCETYPE      ResourceType;
    D3DXIMAGE_FILEFORMAT ImageFileFormat;
} D3DXIMAGE_INFO;

/* Texture creation function */
HRESULT WINAPI D3DXCreateTextureFromFileExW(
    LPDIRECT3DDEVICE9           pDevice,
    LPCWSTR                     pSrcFile,
    UINT                        Width,
    UINT                        Height,
    UINT                        MipLevels,
    DWORD                       Usage,
    D3DFORMAT                   Format,
    D3DPOOL                     Pool,
    DWORD                       Filter,
    DWORD                       MipFilter,
    D3DCOLOR                    ColorKey,
    D3DXIMAGE_INFO*             pSrcInfo,
    PALETTEENTRY*               pPalette,
    LPDIRECT3DTEXTURE9*         ppTexture
);

HRESULT WINAPI D3DXCreateTextureFromFileExA(
    LPDIRECT3DDEVICE9           pDevice,
    LPCSTR                      pSrcFile,
    UINT                        Width,
    UINT                        Height,
    UINT                        MipLevels,
    DWORD                       Usage,
    D3DFORMAT                   Format,
    D3DPOOL                     Pool,
    DWORD                       Filter,
    DWORD                       MipFilter,
    D3DCOLOR                    ColorKey,
    D3DXIMAGE_INFO*             pSrcInfo,
    PALETTEENTRY*               pPalette,
    LPDIRECT3DTEXTURE9*         ppTexture
);

#ifdef UNICODE
#define D3DXCreateTextureFromFileEx D3DXCreateTextureFromFileExW
#else
#define D3DXCreateTextureFromFileEx D3DXCreateTextureFromFileExA
#endif

#ifdef __cplusplus
}
#endif

#endif /* __D3DX9_H__ */
