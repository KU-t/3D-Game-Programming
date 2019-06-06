// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN  // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// [ 따라하기3 ] 
// precompiled 
#include <string>
#include <wrl.h>

#include <d3d12.h> 
// direct3D12 API함수를 사용하기 위해 반드시 포함해야 한다.
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>



//[선미추가] timeGetTime을 위해서
#include <windows.h>
#include <timeapi.h>
#pragma comment(lib, "winmm.lib") 

using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;

// import 라이브러리를 링커 속성에서 입력하지 않아도 되도록 
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#define FRAME_BUFFER_WIDTH    640 
#define FRAME_BUFFER_HEIGHT  480
#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

extern ID3D12Resource *CreateBufferResource(ID3D12Device *pd3dDevice,
	ID3D12GraphicsCommandList *pd3dCommandList, void *pData, UINT nBytes,
	D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD,
	D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
	ID3D12Resource **ppd3dUploadBuffer = NULL);

