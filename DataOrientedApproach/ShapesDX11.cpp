#include "ShapesDX11.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Shape.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

ShapesDX11::ShapesDX11(UINT bufferCount, string name, LONG width, LONG height) : GraphicsDX11{ bufferCount, name, width, height }
{
	createConstantBuffer();
	createVertexShaderAndInputLayout();
	createPixelShader();
	createRasterizerState();
	createDepthStencilState();

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	context->IASetInputLayout(inputLayout.Get());
	context->VSSetShader(vertexShader.Get(), nullptr, 0);
	context->PSSetShader(pixelShader.Get(), nullptr, 0);
	context->RSSetViewports(1, &viewport);
	context->RSSetState(rasterizerState.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ShapesDX11::render(UINT numVertices)
{
	UINT presentCount;
	swapChain->GetLastPresentCount(&presentCount);

	static const FLOAT clearColor[]{ 0.2f, 0.2f, 0.2f, 1.0f };
	ID3D11RenderTargetView* currRtv{ renderTargetViews[presentCount % bufferCount].Get() };
	context->OMSetRenderTargets(1, &currRtv, nullptr);
	context->ClearRenderTargetView(currRtv, clearColor);
	context->Draw(numVertices, 0);
	swapChain->Present(0, 0);
}

void ShapesDX11::createVertexBuffer(UINT vertexBufferSize)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = vertexBufferSize;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&desc, nullptr, vertexBuffer.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating vertex buffer." });
	}

	UINT stride{ sizeof(ShapeShaderData) };
	UINT offset{ 0 };
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
}

void ShapesDX11::setVertices(const vector<float>& data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		throw(runtime_error{ "Error mapping vertex buffer" });
	}

	memcpy(mappedResource.pData, data.data(), data.size() * sizeof(float));

	context->Unmap(vertexBuffer.Get(), 0);
}

void ShapesDX11::createConstantBuffer()
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(XMFLOAT4X4);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	XMFLOAT4X4 matrixProjection;
	XMMATRIX matrixProjectionDX(XMMatrixOrthographicOffCenterLH(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 0.0f, 1.0f));
	XMStoreFloat4x4(&matrixProjection, matrixProjectionDX);

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = &matrixProjection;
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&desc, &initialData, constantBuffer.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating constant buffer." });
	}

	context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
}

void ShapesDX11::createVertexShaderAndInputLayout()
{
	ComPtr<ID3DBlob> blob;
	if (FAILED(D3DReadFileToBlob(L"VertexShader.cso", blob.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error reading vertex shader." });
	}

	if (FAILED(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, vertexShader.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating vertex shader." });
	}

	vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescs;
	inputLayoutDescs.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
	inputLayoutDescs.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 });

	if (FAILED(device->CreateInputLayout(inputLayoutDescs.data(), static_cast<UINT>(inputLayoutDescs.size()), blob->GetBufferPointer(), blob->GetBufferSize(), inputLayout.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating input layout." });
	}
}

void ShapesDX11::createPixelShader()
{
	ComPtr<ID3DBlob> blob;
	if (FAILED(D3DReadFileToBlob(L"PixelShader.cso", blob.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error reading pixel shader." });
	}

	if (FAILED(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pixelShader.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating pixel shader." });
	}
}

void ShapesDX11::createRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = FALSE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	if (FAILED(device->CreateRasterizerState(&desc, rasterizerState.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating rasterizer state." });
	}
}

void ShapesDX11::createDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	desc.StencilEnable = false;
	desc.StencilReadMask = 0;
	desc.StencilWriteMask = 0;

	if (FAILED(device->CreateDepthStencilState(&desc, depthStencilState.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating rasterizer state." });
	}
}
