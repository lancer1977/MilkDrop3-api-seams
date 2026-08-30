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

/* Shader compilation constants */
#define D3DXSHADER_DEBUG              (1 << 0)
#define D3DXSHADER_SKIPOPTIMIZATION   (1 << 2)
#define D3DXSHADER_NO_PRESHADER       (1 << 15)

/* Register sets */
#define D3DXRS_FLOAT4                 0
#define D3DXRS_SAMPLER                1

/* Parameter classes */
#define D3DXPC_SCALAR                 0
#define D3DXPC_VECTOR                 1
#define D3DXPC_MATRIX_ROWS            2
#define D3DXPC_MATRIX_COLUMNS         3
#define D3DXPC_OBJECT                 4
#define D3DXPC_STRUCT                 5

/* Font drawing flags */
#define D3DX_FONTS                    0

/* Forward declarations */
typedef struct ID3DXBuffer ID3DXBuffer;
typedef struct ID3DXFont ID3DXFont;
typedef struct ID3DXConstantTable ID3DXConstantTable;
typedef struct ID3DXInclude ID3DXInclude;

typedef ID3DXBuffer* LPD3DXBUFFER;
typedef ID3DXFont* LPD3DXFONT;
typedef ID3DXConstantTable* LPD3DXCONSTANTTABLE;
typedef ID3DXInclude* LPD3DXINCLUDE;

/* Shader macro definition */
typedef struct D3DXMACRO
{
    LPCSTR Name;
    LPCSTR Definition;
} D3DXMACRO;

/* Constant table constant description (simplified) */
typedef struct D3DXCONSTANT_DESC
{
    LPCSTR Name;
    UINT RegisterSet;
    UINT RegisterIndex;
    UINT RegisterCount;
    UINT Class;
    UINT Type;
    UINT Rows;
    UINT Columns;
    UINT Elements;
    UINT StructMembers;
    DWORD Flags;
    UINT Bytes;
} D3DXCONSTANT_DESC;

/* Constant table description */
typedef struct D3DXCONSTANTTABLE_DESC
{
    LPCSTR Creator;
    DWORD Version;
    UINT Constants;
} D3DXCONSTANTTABLE_DESC;

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

/* Simple texture creation (wrapper around Ex version) */
HRESULT WINAPI D3DXCreateTextureFromFileW(
    LPDIRECT3DDEVICE9 pDevice,
    LPCWSTR pSrcFile,
    LPDIRECT3DTEXTURE9* ppTexture
);

HRESULT WINAPI D3DXCreateTextureFromFileA(
    LPDIRECT3DDEVICE9 pDevice,
    LPCSTR pSrcFile,
    LPDIRECT3DTEXTURE9* ppTexture
);

#ifdef UNICODE
#define D3DXCreateTextureFromFile D3DXCreateTextureFromFileW
#else
#define D3DXCreateTextureFromFile D3DXCreateTextureFromFileA
#endif

/* Font creation and management */
HRESULT WINAPI D3DXCreateFontW(
    LPDIRECT3DDEVICE9 pDevice,
    INT Height,
    UINT Width,
    UINT Weight,
    UINT MipLevels,
    BOOL Italic,
    DWORD CharSet,
    DWORD OutputPrecision,
    DWORD Quality,
    DWORD PitchAndFamily,
    LPCWSTR pFaceName,
    LPD3DXFONT* ppFont
);

HRESULT WINAPI D3DXCreateFontA(
    LPDIRECT3DDEVICE9 pDevice,
    INT Height,
    UINT Width,
    UINT Weight,
    UINT MipLevels,
    BOOL Italic,
    DWORD CharSet,
    DWORD OutputPrecision,
    DWORD Quality,
    DWORD PitchAndFamily,
    LPCSTR pFaceName,
    LPD3DXFONT* ppFont
);

#ifdef UNICODE
#define D3DXCreateFont D3DXCreateFontW
#else
#define D3DXCreateFont D3DXCreateFontA
#endif

/* Shader compilation */
HRESULT WINAPI D3DXCompileShader(
    LPCSTR pSrcData,
    UINT SrcDataLen,
    const D3DXMACRO* pDefines,
    LPD3DXINCLUDE pInclude,
    LPCSTR pFunctionName,
    LPCSTR pProfile,
    DWORD Flags,
    LPD3DXBUFFER* ppShader,
    LPD3DXBUFFER* ppErrorMsgs,
    LPD3DXCONSTANTTABLE* ppConstantTable
);

HRESULT WINAPI D3DXCompileShaderFromFileW(
    LPCWSTR pSrcFile,
    const D3DXMACRO* pDefines,
    LPD3DXINCLUDE pInclude,
    LPCSTR pFunctionName,
    LPCSTR pProfile,
    DWORD Flags,
    LPD3DXBUFFER* ppShader,
    LPD3DXBUFFER* ppErrorMsgs,
    LPD3DXCONSTANTTABLE* ppConstantTable
);

HRESULT WINAPI D3DXCompileShaderFromFileA(
    LPCSTR pSrcFile,
    const D3DXMACRO* pDefines,
    LPD3DXINCLUDE pInclude,
    LPCSTR pFunctionName,
    LPCSTR pProfile,
    DWORD Flags,
    LPD3DXBUFFER* ppShader,
    LPD3DXBUFFER* ppErrorMsgs,
    LPD3DXCONSTANTTABLE* ppConstantTable
);

#ifdef UNICODE
#define D3DXCompileShaderFromFile D3DXCompileShaderFromFileW
#else
#define D3DXCompileShaderFromFile D3DXCompileShaderFromFileA
#endif

#ifdef __cplusplus
}
#endif

#endif /* __D3DX9_H__ */
