#include "overlay.hpp"
#include <dwmapi.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool Overlay::Initialize(HWND targetWindow) {
    target_ = targetWindow;
    if (!CreateOverlayWindow(targetWindow)) return false;
    if (!CreateDevice()) return false;
    
    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0;
    style.WindowRounding = 0;
    
    ImGui_ImplWin32_Init(hwnd_);
    ImGui_ImplDX11_Init(device_, context_);
    
    return true;
}

void Overlay::Shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    CleanupDevice();
    DestroyWindow(hwnd_);
    UnregisterClass(wc_.lpszClassName, wc_.hInstance);
}

void Overlay::BeginFrame() {
    // Update overlay position to match game
    RECT rect;
    GetWindowRect(target_, &rect);
    width_ = rect.right - rect.left;
    height_ = rect.bottom - rect.top;
    
    SetWindowPos(hwnd_, HWND_TOPMOST, rect.left, rect.top, width_, height_, 
        SWP_NOACTIVATE | SWP_SHOWWINDOW);
    
    // Start ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Overlay::EndFrame() {
    ImGui::EndFrame();
    
    const float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    context_->OMSetRenderTargets(1, &renderTarget_, nullptr);
    context_->ClearRenderTargetView(renderTarget_, clearColor);
    
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    
    swapChain_->Present(1, 0); // VSync on
}

bool Overlay::CreateOverlayWindow(HWND target) {
    // Get game window info
    RECT rect;
    GetWindowRect(target, &rect);
    width_ = rect.right - rect.left;
    height_ = rect.bottom - rect.top;
    
    // Create transparent layered window
    wc_ = {sizeof(wc_)};
    wc_.lpfnWndProc = WndProc;
    wc_.hInstance = GetModuleHandle(nullptr);
    wc_.lpszClassName = "ENICS2Overlay";
    wc_.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    
    RegisterClassEx(&wc_);
    
    hwnd_ = CreateWindowEx(
        WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        wc_.lpszClassName, "ENI",
        WS_POPUP,
        rect.left, rect.top, width_, height_,
        nullptr, nullptr, wc_.hInstance, nullptr
    );
    
    if (!hwnd_) return false;
    
    // Make fully transparent (color key)
    SetLayeredWindowAttributes(hwnd_, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(hwnd_, SW_SHOW);
    
    // Enable click-through
    MARGINS margins = {-1};
    DwmExtendFrameIntoClientArea(hwnd_, &margins);
    
    return true;
}

bool Overlay::CreateDevice() {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width_;
    sd.BufferDesc.Height = height_;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 144;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd_;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr, 0, D3D11_SDK_VERSION,
        &sd, &swapChain_, &device_, &featureLevel,
        &context_
    );
    
    if (FAILED(hr)) return false;
    
    // Create render target
    ID3D11Texture2D* backBuffer = nullptr;
    swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (backBuffer) {
        device_->CreateRenderTargetView(backBuffer, nullptr, &renderTarget_);
        backBuffer->Release();
    }
    
    return renderTarget_ != nullptr;
}

void Overlay::CleanupDevice() {
    if (renderTarget_) { renderTarget_->Release(); renderTarget_ = nullptr; }
    if (swapChain_) { swapChain_->Release(); swapChain_ = nullptr; }
    if (context_) { context_->Release(); context_ = nullptr; }
    if (device_) { device_->Release(); device_ = nullptr; }
}