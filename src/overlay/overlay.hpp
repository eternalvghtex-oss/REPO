#pragma once
#include <windows.h>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

class Overlay {
public:
    bool Initialize(HWND targetWindow);
    void Shutdown();
    void BeginFrame();
    void EndFrame();
    void Render();
    
    bool IsRunning() const { return running_; }
    HWND GetWindow() const { return hwnd_; }
    
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    
private:
    bool CreateDevice();
    void CleanupDevice();
    bool CreateOverlayWindow(HWND target);
    
    HWND hwnd_ = nullptr;
    HWND target_ = nullptr;
    WNDCLASSEX wc_{};
    
    ID3D11Device* device_ = nullptr;
    ID3D11DeviceContext* context_ = nullptr;
    IDXGISwapChain* swapChain_ = nullptr;
    ID3D11RenderTargetView* renderTarget_ = nullptr;
    
    int width_ = 1920;
    int height_ = 1080;
    bool running_ = true;
};