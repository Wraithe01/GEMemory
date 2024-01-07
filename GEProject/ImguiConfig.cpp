#include "Includes.h"

#include <raylib.h>
#include <rlgl.h>
#include "rlImGui.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_win32.h>

#include <unordered_map>


//~ Statics

static ImGuiMouseCursor                          g_cursorCurrent = ImGuiMouseCursor_COUNT;
static MouseCursor                               g_cursorTable[ImGuiMouseCursor_COUNT];
static std::unordered_map<KeyboardKey, ImGuiKey> g_keyMap;

static ImFont* g_font;
static ImFont* g_fontBold;

static const char* ImguiGetClipText(void*) { return GetClipboardText(); }

static void ImguiSetClipText(void*, const char* text) { SetClipboardText(text); }


//~ Definitions

#define MCOL(col, scal) (ImVec4(col.x * scal, col.y * scal, col.z * scal, col.w))

void ImguiInit()
{
    // Context
    // rlImGuiSetup(true);
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable /* | ImGuiConfigFlags_ViewportsEnable*/;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.SetClipboardTextFn = ImguiSetClipText;
    io.GetClipboardTextFn = ImguiGetClipText;

    // Font
    g_font     = io.Fonts->AddFontFromFileTTF("Assets\\Fonts\\ubuntu.ttf", 20.0f);
    g_fontBold = io.Fonts->AddFontFromFileTTF("Assets\\Fonts\\ubuntu.ttf", 20.0f);
    io.Fonts->Build();


    // Style

    ImVec4 mainColor = ImVec4(1.0f, 0.4f, 0.05f, 1.0f);

    ImGuiStyle& s                         = ImGui::GetStyle();
    s.ChildRounding                       = 3.0f;
    s.FrameRounding                       = 0.0f;
    s.PopupRounding                       = 0.0f;
    s.TabRounding                         = 0.0f;
    s.WindowRounding                      = 0.0f;
    s.Colors[ImGuiCol_Button]             = mainColor;
    s.Colors[ImGuiCol_ButtonHovered]      = MCOL(mainColor, 0.8f);
    s.Colors[ImGuiCol_ButtonActive]       = MCOL(mainColor, 0.6f);
    s.Colors[ImGuiCol_DockingPreview]     = MCOL(mainColor, 0.6f);
    s.Colors[ImGuiCol_ResizeGrip]         = MCOL(mainColor, 0.8f);
    s.Colors[ImGuiCol_ResizeGripHovered]  = MCOL(mainColor, 0.65f);
    s.Colors[ImGuiCol_ResizeGripActive]   = MCOL(mainColor, 0.5f);
    s.Colors[ImGuiCol_SeparatorHovered]   = mainColor;
    s.Colors[ImGuiCol_SeparatorActive]    = MCOL(mainColor, 0.8f);
    s.Colors[ImGuiCol_Header]             = mainColor;
    s.Colors[ImGuiCol_HeaderHovered]      = MCOL(mainColor, 0.8f);
    s.Colors[ImGuiCol_HeaderActive]       = MCOL(mainColor, 0.6f);
    s.Colors[ImGuiCol_CheckMark]          = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    s.Colors[ImGuiCol_FrameBg]            = MCOL(mainColor, 0.6f);
    s.Colors[ImGuiCol_FrameBgHovered]     = MCOL(mainColor, 0.5f);
    s.Colors[ImGuiCol_FrameBgActive]      = MCOL(mainColor, 0.45f);
    s.Colors[ImGuiCol_SliderGrab]         = mainColor;
    s.Colors[ImGuiCol_SliderGrabActive]   = MCOL(mainColor, 1.2f);
    s.Colors[ImGuiCol_Tab]                = MCOL(mainColor, 0.6f);
    s.Colors[ImGuiCol_TabHovered]         = MCOL(mainColor, 0.8f);
    s.Colors[ImGuiCol_TabActive]          = mainColor;
    s.Colors[ImGuiCol_TabUnfocused]       = MCOL(mainColor, 0.6f);
    s.Colors[ImGuiCol_TabUnfocusedActive] = mainColor;
    s.Colors[ImGuiCol_TitleBg]            = MCOL(mainColor, 0.2f);
    s.Colors[ImGuiCol_TitleBgActive]      = MCOL(mainColor, 0.2f);
    s.Colors[ImGuiCol_TitleBgCollapsed]   = MCOL(mainColor, 0.2f);


    // Maps and tables

    g_cursorTable[ImGuiMouseCursor_Arrow]      = MOUSE_CURSOR_ARROW;
    g_cursorTable[ImGuiMouseCursor_TextInput]  = MOUSE_CURSOR_IBEAM;
    g_cursorTable[ImGuiMouseCursor_Hand]       = MOUSE_CURSOR_POINTING_HAND;
    g_cursorTable[ImGuiMouseCursor_ResizeAll]  = MOUSE_CURSOR_RESIZE_ALL;
    g_cursorTable[ImGuiMouseCursor_ResizeEW]   = MOUSE_CURSOR_RESIZE_EW;
    g_cursorTable[ImGuiMouseCursor_ResizeNESW] = MOUSE_CURSOR_RESIZE_NESW;
    g_cursorTable[ImGuiMouseCursor_ResizeNS]   = MOUSE_CURSOR_RESIZE_NS;
    g_cursorTable[ImGuiMouseCursor_ResizeNWSE] = MOUSE_CURSOR_RESIZE_NWSE;
    g_cursorTable[ImGuiMouseCursor_NotAllowed] = MOUSE_CURSOR_NOT_ALLOWED;

    g_keyMap[KEY_APOSTROPHE]    = ImGuiKey_Apostrophe;
    g_keyMap[KEY_COMMA]         = ImGuiKey_Comma;
    g_keyMap[KEY_MINUS]         = ImGuiKey_Minus;
    g_keyMap[KEY_PERIOD]        = ImGuiKey_Period;
    g_keyMap[KEY_SLASH]         = ImGuiKey_Slash;
    g_keyMap[KEY_ZERO]          = ImGuiKey_0;
    g_keyMap[KEY_ONE]           = ImGuiKey_1;
    g_keyMap[KEY_TWO]           = ImGuiKey_2;
    g_keyMap[KEY_THREE]         = ImGuiKey_3;
    g_keyMap[KEY_FOUR]          = ImGuiKey_4;
    g_keyMap[KEY_FIVE]          = ImGuiKey_5;
    g_keyMap[KEY_SIX]           = ImGuiKey_6;
    g_keyMap[KEY_SEVEN]         = ImGuiKey_7;
    g_keyMap[KEY_EIGHT]         = ImGuiKey_8;
    g_keyMap[KEY_NINE]          = ImGuiKey_9;
    g_keyMap[KEY_SEMICOLON]     = ImGuiKey_Semicolon;
    g_keyMap[KEY_EQUAL]         = ImGuiKey_Equal;
    g_keyMap[KEY_A]             = ImGuiKey_A;
    g_keyMap[KEY_B]             = ImGuiKey_B;
    g_keyMap[KEY_C]             = ImGuiKey_C;
    g_keyMap[KEY_D]             = ImGuiKey_D;
    g_keyMap[KEY_E]             = ImGuiKey_E;
    g_keyMap[KEY_F]             = ImGuiKey_F;
    g_keyMap[KEY_G]             = ImGuiKey_G;
    g_keyMap[KEY_H]             = ImGuiKey_H;
    g_keyMap[KEY_I]             = ImGuiKey_I;
    g_keyMap[KEY_J]             = ImGuiKey_J;
    g_keyMap[KEY_K]             = ImGuiKey_K;
    g_keyMap[KEY_L]             = ImGuiKey_L;
    g_keyMap[KEY_M]             = ImGuiKey_M;
    g_keyMap[KEY_N]             = ImGuiKey_N;
    g_keyMap[KEY_O]             = ImGuiKey_O;
    g_keyMap[KEY_P]             = ImGuiKey_P;
    g_keyMap[KEY_Q]             = ImGuiKey_Q;
    g_keyMap[KEY_R]             = ImGuiKey_R;
    g_keyMap[KEY_S]             = ImGuiKey_S;
    g_keyMap[KEY_T]             = ImGuiKey_T;
    g_keyMap[KEY_U]             = ImGuiKey_U;
    g_keyMap[KEY_V]             = ImGuiKey_V;
    g_keyMap[KEY_W]             = ImGuiKey_W;
    g_keyMap[KEY_X]             = ImGuiKey_X;
    g_keyMap[KEY_Y]             = ImGuiKey_Y;
    g_keyMap[KEY_Z]             = ImGuiKey_Z;
    g_keyMap[KEY_SPACE]         = ImGuiKey_Space;
    g_keyMap[KEY_ESCAPE]        = ImGuiKey_Escape;
    g_keyMap[KEY_ENTER]         = ImGuiKey_Enter;
    g_keyMap[KEY_TAB]           = ImGuiKey_Tab;
    g_keyMap[KEY_BACKSPACE]     = ImGuiKey_Backspace;
    g_keyMap[KEY_INSERT]        = ImGuiKey_Insert;
    g_keyMap[KEY_DELETE]        = ImGuiKey_Delete;
    g_keyMap[KEY_RIGHT]         = ImGuiKey_RightArrow;
    g_keyMap[KEY_LEFT]          = ImGuiKey_LeftArrow;
    g_keyMap[KEY_DOWN]          = ImGuiKey_DownArrow;
    g_keyMap[KEY_UP]            = ImGuiKey_UpArrow;
    g_keyMap[KEY_PAGE_UP]       = ImGuiKey_PageUp;
    g_keyMap[KEY_PAGE_DOWN]     = ImGuiKey_PageDown;
    g_keyMap[KEY_HOME]          = ImGuiKey_Home;
    g_keyMap[KEY_END]           = ImGuiKey_End;
    g_keyMap[KEY_CAPS_LOCK]     = ImGuiKey_CapsLock;
    g_keyMap[KEY_SCROLL_LOCK]   = ImGuiKey_ScrollLock;
    g_keyMap[KEY_NUM_LOCK]      = ImGuiKey_NumLock;
    g_keyMap[KEY_PRINT_SCREEN]  = ImGuiKey_PrintScreen;
    g_keyMap[KEY_PAUSE]         = ImGuiKey_Pause;
    g_keyMap[KEY_F1]            = ImGuiKey_F1;
    g_keyMap[KEY_F2]            = ImGuiKey_F2;
    g_keyMap[KEY_F3]            = ImGuiKey_F3;
    g_keyMap[KEY_F4]            = ImGuiKey_F4;
    g_keyMap[KEY_F5]            = ImGuiKey_F5;
    g_keyMap[KEY_F6]            = ImGuiKey_F6;
    g_keyMap[KEY_F7]            = ImGuiKey_F7;
    g_keyMap[KEY_F8]            = ImGuiKey_F8;
    g_keyMap[KEY_F9]            = ImGuiKey_F9;
    g_keyMap[KEY_F10]           = ImGuiKey_F10;
    g_keyMap[KEY_F11]           = ImGuiKey_F11;
    g_keyMap[KEY_F12]           = ImGuiKey_F12;
    g_keyMap[KEY_LEFT_SHIFT]    = ImGuiKey_LeftShift;
    g_keyMap[KEY_LEFT_CONTROL]  = ImGuiKey_LeftCtrl;
    g_keyMap[KEY_LEFT_ALT]      = ImGuiKey_LeftAlt;
    g_keyMap[KEY_LEFT_SUPER]    = ImGuiKey_LeftSuper;
    g_keyMap[KEY_RIGHT_SHIFT]   = ImGuiKey_RightShift;
    g_keyMap[KEY_RIGHT_CONTROL] = ImGuiKey_RightCtrl;
    g_keyMap[KEY_RIGHT_ALT]     = ImGuiKey_RightAlt;
    g_keyMap[KEY_RIGHT_SUPER]   = ImGuiKey_RightSuper;
    g_keyMap[KEY_KB_MENU]       = ImGuiKey_Menu;
    g_keyMap[KEY_LEFT_BRACKET]  = ImGuiKey_LeftBracket;
    g_keyMap[KEY_BACKSLASH]     = ImGuiKey_Backslash;
    g_keyMap[KEY_RIGHT_BRACKET] = ImGuiKey_RightBracket;
    g_keyMap[KEY_GRAVE]         = ImGuiKey_GraveAccent;
    g_keyMap[KEY_KP_0]          = ImGuiKey_Keypad0;
    g_keyMap[KEY_KP_1]          = ImGuiKey_Keypad1;
    g_keyMap[KEY_KP_2]          = ImGuiKey_Keypad2;
    g_keyMap[KEY_KP_3]          = ImGuiKey_Keypad3;
    g_keyMap[KEY_KP_4]          = ImGuiKey_Keypad4;
    g_keyMap[KEY_KP_5]          = ImGuiKey_Keypad5;
    g_keyMap[KEY_KP_6]          = ImGuiKey_Keypad6;
    g_keyMap[KEY_KP_7]          = ImGuiKey_Keypad7;
    g_keyMap[KEY_KP_8]          = ImGuiKey_Keypad8;
    g_keyMap[KEY_KP_9]          = ImGuiKey_Keypad9;
    g_keyMap[KEY_KP_DECIMAL]    = ImGuiKey_KeypadDecimal;
    g_keyMap[KEY_KP_DIVIDE]     = ImGuiKey_KeypadDivide;
    g_keyMap[KEY_KP_MULTIPLY]   = ImGuiKey_KeypadMultiply;
    g_keyMap[KEY_KP_SUBTRACT]   = ImGuiKey_KeypadSubtract;
    g_keyMap[KEY_KP_ADD]        = ImGuiKey_KeypadAdd;
    g_keyMap[KEY_KP_ENTER]      = ImGuiKey_KeypadEnter;
    g_keyMap[KEY_KP_EQUAL]      = ImGuiKey_KeypadEqual;


    // Framework initialization

    ImGui_ImplWin32_Init(GetWindowHandle());
    ImGui_ImplOpenGL3_Init();
}

void ImguiDeInit()
{
    // rlImGuiShutdown();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void ImguiBegin()
{
    ImGuiIO& io  = ImGui::GetIO();
    io.DeltaTime = GetFrameTime();


    // I/O

    io.MouseDown[0] = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    io.MouseDown[1] = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
    io.MouseDown[2] = IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);

    if (GetMouseWheelMove() > 0)
        io.MouseWheel += 1;
    else if (GetMouseWheelMove() < 0)
        io.MouseWheel -= 1;

    if (io.WantSetMousePos)
    {
        SetMousePosition((int) io.MousePos.x, (int) io.MousePos.y);
    }
    else
    {
        io.MousePos.x = (float) GetMouseX();
        io.MousePos.y = (float) GetMouseY();
    }


    // Flags and events

    static bool lastFrameFocused   = false;
    static bool lastControlPressed = false;
    static bool lastShiftPressed   = false;
    static bool lastAltPressed     = false;
    static bool lastSuperPressed   = false;

    bool focused = IsWindowFocused();
    if (focused != lastFrameFocused)
        io.AddFocusEvent(focused);
    lastFrameFocused = focused;

    bool ctrlDown = IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_CONTROL);
    if (ctrlDown != lastControlPressed)
        io.AddKeyEvent(ImGuiMod_Ctrl, ctrlDown);
    lastControlPressed = ctrlDown;

    bool shiftDown = IsKeyDown(KEY_RIGHT_SHIFT) || IsKeyDown(KEY_LEFT_SHIFT);
    if (shiftDown != lastShiftPressed)
        io.AddKeyEvent(ImGuiMod_Shift, ctrlDown);
    lastShiftPressed = shiftDown;

    bool altDown = IsKeyDown(KEY_RIGHT_ALT) || IsKeyDown(KEY_LEFT_ALT);
    if (altDown != lastAltPressed)
        io.AddKeyEvent(ImGuiMod_Alt, altDown);
    lastAltPressed = altDown;

    int key = 0;
    while ((key = GetKeyPressed()) != 0)
    {
        auto pair = g_keyMap.find(KeyboardKey(key));
        if (pair != g_keyMap.end())
        {
            io.AddKeyEvent(pair->second, true);
        }
    }

    for (auto keyItr : g_keyMap)
    {
        io.KeysData[keyItr.second].Down = IsKeyDown(keyItr.first);
        if (IsKeyReleased(keyItr.first))
        {
            io.AddKeyEvent(keyItr.second, false);
        }
    }

    unsigned int character = 0;
    while ((character = GetCharPressed()) != 0)
    {
        io.AddInputCharacter(character);
    }


    // Cursor

    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
    {
        ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
        if (cursor != g_cursorCurrent || io.MouseDrawCursor)
        {
            g_cursorCurrent = cursor;
            if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None)
            {
                HideCursor();
            }
            else
            {
                ShowCursor();

                if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange))
                {
                    SetMouseCursor((cursor > -1 && cursor < ImGuiMouseCursor_COUNT)
                                       ? g_cursorTable[cursor]
                                       : MOUSE_CURSOR_DEFAULT);
                }
            }
        }
    }


    // Frame initialization

    // rlImGuiBegin();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(nullptr,
                                 ImGuiDockNodeFlags_NoDockingInCentralNode
                                     | ImGuiDockNodeFlags_PassthruCentralNode);
}

void ImguiEnd()
{
    // rlImGuiEnd();
    ImGui::Render();

    rlDrawRenderBatchActive();
    rlDisableBackfaceCulling();

    rlBegin(RL_TRIANGLES);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    rlEnd();

    rlSetTexture(0);
    rlDisableScissorTest();
    rlEnableBackfaceCulling();

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void ImguiPushFont(bool bold) { ImGui::PushFont(bold ? g_fontBold : g_font); }

void ImguiPopFont() { ImGui::PopFont(); }
