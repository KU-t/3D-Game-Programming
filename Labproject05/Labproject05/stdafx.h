// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

// Windows 헤더 파일
#include <windows.h>
#include <timeapi.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
#include <string>
#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>		// Direct#d 12 API 함수 헤더
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

// 임포트(import)라이브러리 추가
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

//주 윈도우 클라이언트 영역 크기 설정
#define FRAME_BUFFER_WIDTH 640
#define FRAME_BUFFER_HEIGHT 480

