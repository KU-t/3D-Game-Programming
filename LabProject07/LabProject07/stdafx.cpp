#include "stdafx.h"

/*버퍼 리소스를 생성하는 함수이다. 버퍼의 힙 유형에 따라 버퍼 리소스를 생성하고 초기화 데이터가 있으면 초기화 한다.*/ 
ID3D12Resource *CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData, UINT nBytes,
	D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource **ppd3dUploadBuffer) {

	// Resource 메모리 할당
	ID3D12Resource *pd3dBuffer = NULL;

	// Resource Heap Desc 생성
	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	// Resource Heap Desc - 초기화
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	// Resource Heap Desc - 힙의 유형
	d3dHeapPropertiesDesc.Type = d3dHeapType;
	// Resource Heap Desc - 힙에 대한 CPU 페이지 속성 (CPU가 어떻게 접근 가능한가, 불가한가)
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// Resource Heap Desc - 힙에 대한 메모리 풀( CPU vs GPU 대역폭이 어디가 더 큰가 )
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	// Resource Heap Desc - GPU 어댑터 : 0 == 1개이상 // 1 == 1개
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	// Resource Heap Desc - 다중 어댑터에서 리소스를 볼 수 있는 노드 집합 (뭔데?)
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	// Resource Desc - 생성
	D3D12_RESOURCE_DESC d3dResourceDesc;
	// Resource Desc - 초기화
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	// Resource Desc - 리소스 종류 : 버퍼 
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// Resource Desc - 리소스가 어떻게 정렬되는가? : 0 == 64KB
	d3dResourceDesc.Alignment = 0;
	// Resource Desc - 가로 크기 : 정점 버퍼의 크기
	d3dResourceDesc.Width = nBytes;
	// Resource Desc - 세로 크기
	d3dResourceDesc.Height = 1;
	// Resource Desc - 깊이 또는 배열의 크기
	d3dResourceDesc.DepthOrArraySize = 1;
	// Resource Desc - 밉맵레벨
	d3dResourceDesc.MipLevels = 1;
	// Resource Desc - 리소스 형식 : 알아서 해
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	// Resource Desc - 다중 샘플링 : 픽셀마다 샘플 개수 1개
	d3dResourceDesc.SampleDesc.Count = 1;
	// Resource Desc - 다중 샘플링 : 품질 레벨 0 == 다중 샘플링 하지 않음
	d3dResourceDesc.SampleDesc.Quality = 0;
	// Resource Desc - 다차원 리소스를 1차원 리소스로 매핑하기 위한 방법 : 텍스쳐 데이터가 행우선 순서로 저장
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// Resource Desc - 어떤 뷰에서 사용 가능한지 : 모두 가능
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// Resource State(상태) - 어떻게 사용할 것인가 : 복사 연산을 목표로 사용
	D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	// Resource Heap type == D3D12_HEAP_TYPE_UPLOAD 일때 : 업로드를 위한 CPU 최적화 (CPU 데이터로 리소스 초기화, 반복적인 업로드)
	// '업로드 힙의 시작 상태'로 변경
	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) 
		d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	// Resource Heap type == D3D12_HEAP_TYPE_READBACK 일때 : 읽기 위한 힙
	// '복사 연산을 목표로 사용'으로 변경
	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) 
		d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	// Device에 리소스 생성
	// Heap Properties Desc 주소,  힙 옵션 : 없음, Resource Desc 주소, Resource State (상태), 몰라(?), 어떤 형태로, 저장할 주소
	HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, d3dResourceInitialStates, NULL, __uuidof(ID3D12Resource), (void **)&pd3dBuffer);

	// 정점관련 데이터가 있으면
	if (pData) {

		// 생성할 리소스의 Heap type에 따라
		switch (d3dHeapType) {
		// 생성할 리소스의 Heap type이
		// 'GPU만 읽고 쓰기 가능하고, CPU는 접근 불가 (대부분의 리소스에서 적용)' 일때,
		case D3D12_HEAP_TYPE_DEFAULT:{
			// 업로드 버퍼가 있을 경우,(?)
			if (ppd3dUploadBuffer) {
				// '업로드를 위한 CPU접근에 최적화' 타입으로 변경
				d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				// 업로드 버퍼로 변경 : 리소스 상태를 업로드 힙의 시작 상태 
				pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void **)ppd3dUploadBuffer);
				
				// 업로드 버퍼를 매핑하여 초기화 데이터를 업로드 버퍼에 복사한다.

				// CPU에서 읽지 않음 : { begin, end } end <= begin이면 읽지 않음
				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8 *pBufferDataBegin = NULL;

				// 업로드 버퍼의 리소스 갱신 (업로드 버퍼 매핑) : 리소스에 대한 CPU포인터를 반환 (다중 쓰레드)
				// 서브 리소스 인덱스 or 0 == 버퍼, CPU가 접근할 메모리 영역(NULL일 경우, 전체 서브 리소스), 서브 리소스의 주소
				(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void **)&pBufferDataBegin);
				// 업로드 버퍼에 복사
				memcpy(pBufferDataBegin, pData, nBytes);
				// 서브 리소스 인덱스 or 0 == 버퍼, CPU가 수정한 메모리 영역(NULL일 경우, 전체 서브 리소스)
				(*ppd3dUploadBuffer)->Unmap(0, NULL);
				// 업로드 버퍼의 내용을 디폴트 버퍼에 복사한다.
				// GPU에서 리소스 전체를 복사 : ppd3dUploadBuffer(업로드 버퍼)에서 pd3dBuffer(새로 할당한 버퍼)로
				// 상태 : D3D12_HEAP_TYPE_UPLOAD -> D3D12_HEAP_TYPE_DEFAULT
				pd3dCommandList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);

				// Resource Barrier 생성
				D3D12_RESOURCE_BARRIER d3dResourceBarrier;
				// Resource Barrier - 초기화
				::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
				// Resource Barrier - 유형 : 전이 장벽
				d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				// Resource Barrier - 리소스 장벽을 분리 하기 위한 설정 
				d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				// Resource Barrier - 상태전이 대상 리소스 : 새로 할당한 리소스 버퍼
				d3dResourceBarrier.Transition.pResource = pd3dBuffer;
				// Resource Barrier - 이전 상태 : '복사 연산을 목표로 사용'
				d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				// Resource Barrier - 이후 상태 : 원래의 상태로
				d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
				// Resource Barrier - 서브 리소스의 인덱스 : 모든 서브 리소스
				d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				// Resource Barrier
				// 배열 원소의 개수, 배열 주소
				pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
			}
			break;
		} 
		// 생성할 리소스의 Heap type이
		// '업로드를 위한 CPU접근에 최적화' 일때,
		case D3D12_HEAP_TYPE_UPLOAD: { 

			// CPU에서 읽지 않음 : { begin, end } end <= begin이면 읽지 않음
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8 *pBufferDataBegin = NULL;

			pd3dBuffer->Map(0, &d3dReadRange, (void **)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pd3dBuffer->Unmap(0, NULL); 
			break; 
		} 
		// 생성할 리소스의 Heap type이
		// '복사 연산을 목표로 사용' 일때,
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	} 
	return(pd3dBuffer);
}