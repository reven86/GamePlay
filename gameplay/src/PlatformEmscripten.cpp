#ifndef GP_NO_PLATFORM
#ifdef EMSCRIPTEN

#include "Base.h"
#include "Platform.h"
#include "FileSystem.h"
#include "Game.h"
#include "Form.h"
#include "ScriptController.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <sys/time.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#define TOUCH_COUNT_MAX     4
#define MAX_GAMEPADS 4

using namespace std;

int __argc = 0;
char** __argv = 0;

enum GamepadAxisInfoFlags
{
    GP_AXIS_SKIP = 0x1,
    GP_AXIS_IS_DPAD = 0x2,
    GP_AXIS_IS_NEG = 0x4,
    GP_AXIS_IS_XAXIS = 0x8,
    GP_AXIS_IS_TRIGGER = 0x10
};

enum GamepadAxisInfoNormalizeFunction
{
    NEG_TO_POS,
    ZERO_TO_POS
};

struct GamepadJoystickAxisInfo
{
    int axisIndex;
    unsigned int joystickIndex;
    unsigned long flags;
    int mappedPosArg;
    int mappedNegArg;
    float deadZone;
    GamepadAxisInfoNormalizeFunction mapFunc;
};

struct GamepadInfoEntry
{
    unsigned int vendorId;
    unsigned int productId;
    const char* productName;
    unsigned int numberOfJS;
    unsigned int numberOfAxes;
    unsigned int numberOfButtons;
    unsigned int numberOfTriggers;

    GamepadJoystickAxisInfo* axes;
    long* buttons;
};

struct ConnectedGamepadDevInfo
{
    dev_t deviceId;
    gameplay::GamepadHandle fd;
    const GamepadInfoEntry& gamepadInfo;
};

struct timespec __timespec;
static double __timeStart;
static double __timeAbsolute;
static bool __vsync = WINDOW_VSYNC;
static bool __mouseCaptured = false;
static int __mouseCapturePointX = 0;
static int __mouseCapturePointY = 0;
static bool __multiSampling = false;
static bool __cursorVisible = true;
static bool __leftMouseButtonPressed = false;
static EGLDisplay __eglDisplay = EGL_NO_DISPLAY;
static EGLContext __eglContext = EGL_NO_CONTEXT;
static EGLSurface __eglSurface = EGL_NO_SURFACE;
static EGLConfig __eglConfig = 0;
static const char* __glExtensions;
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray = NULL;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays = NULL;
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays = NULL;
PFNGLISVERTEXARRAYOESPROC glIsVertexArray = NULL;
static int __windowSize[2];
static list<ConnectedGamepadDevInfo> __connectedGamepads;

static EGLenum checkErrorEGL(const char* msg)
{
    GP_ASSERT(msg);
    static const char* errmsg[] =
    {
        "EGL function succeeded",
        "EGL is not initialized, or could not be initialized, for the specified display",
        "EGL cannot access a requested resource",
        "EGL failed to allocate resources for the requested operation",
        "EGL fail to access an unrecognized attribute or attribute value was passed in an attribute list",
        "EGLConfig argument does not name a valid EGLConfig",
        "EGLContext argument does not name a valid EGLContext",
        "EGL current surface of the calling thread is no longer valid",
        "EGLDisplay argument does not name a valid EGLDisplay",
        "EGL arguments are inconsistent",
        "EGLNativePixmapType argument does not refer to a valid native pixmap",
        "EGLNativeWindowType argument does not refer to a valid native window",
        "EGL one or more argument values are invalid",
        "EGLSurface argument does not name a valid surface configured for rendering",
        "EGL power management event has occurred",
    };
    EGLenum error = eglGetError();
    printf("%s: %s.", msg, errmsg[error - EGL_SUCCESS]);
    return error;
}

// Gets the gameplay::Keyboard::Key enumeration constant that corresponds to the given X11 key symbol.
static gameplay::Keyboard::Key getKey(KeySym sym)
{
    switch (sym)
    {
        case XK_Sys_Req:
            return gameplay::Keyboard::KEY_SYSREQ;
        case XK_Break:
            return gameplay::Keyboard::KEY_BREAK;
        case XK_Menu :
            return gameplay::Keyboard::KEY_MENU;
        case XK_KP_Enter:
            return gameplay::Keyboard::KEY_KP_ENTER;
        case XK_Pause:
            return gameplay::Keyboard::KEY_PAUSE;
        case XK_Scroll_Lock:
            return gameplay::Keyboard::KEY_SCROLL_LOCK;
        case XK_Print:
            return gameplay::Keyboard::KEY_PRINT;
        case XK_Escape:
            return gameplay::Keyboard::KEY_ESCAPE;
        case XK_BackSpace:
            return gameplay::Keyboard::KEY_BACKSPACE;
        case XK_Tab:
            return gameplay::Keyboard::KEY_TAB;
        case XK_Return:
            return gameplay::Keyboard::KEY_RETURN;
        case XK_Caps_Lock:
            return gameplay::Keyboard::KEY_CAPS_LOCK;
        case XK_Shift_L:
        case XK_Shift_R:
            return gameplay::Keyboard::KEY_SHIFT;
        case XK_Control_L:
        case XK_Control_R:
            return gameplay::Keyboard::KEY_CTRL;
        case XK_Alt_L:
        case XK_Alt_R:
            return gameplay::Keyboard::KEY_ALT;
        case XK_Hyper_L:
        case XK_Hyper_R:
            return gameplay::Keyboard::KEY_HYPER;
        case XK_Insert:
            return gameplay::Keyboard::KEY_INSERT;
        case XK_Home:
            return gameplay::Keyboard::KEY_HOME;
        case XK_Page_Up:
            return gameplay::Keyboard::KEY_PG_UP;
        case XK_Delete:
            return gameplay::Keyboard::KEY_DELETE;
        case XK_End:
            return gameplay::Keyboard::KEY_END;
        case XK_Page_Down:
            return gameplay::Keyboard::KEY_PG_DOWN;
        case XK_Left:
            return gameplay::Keyboard::KEY_LEFT_ARROW;
        case XK_Right:
            return gameplay::Keyboard::KEY_RIGHT_ARROW;
        case XK_Up:
            return gameplay::Keyboard::KEY_UP_ARROW;
        case XK_Down:
            return gameplay::Keyboard::KEY_DOWN_ARROW;
        case XK_Num_Lock:
            return gameplay::Keyboard::KEY_NUM_LOCK;
        case XK_KP_Add:
            return gameplay::Keyboard::KEY_KP_PLUS;
        case XK_KP_Subtract:
            return gameplay::Keyboard::KEY_KP_MINUS;
        case XK_KP_Multiply:
            return gameplay::Keyboard::KEY_KP_MULTIPLY;
        case XK_KP_Divide:
            return gameplay::Keyboard::KEY_KP_DIVIDE;
        case XK_KP_Home:
            return gameplay::Keyboard::KEY_KP_HOME;
        case XK_KP_Up:
            return gameplay::Keyboard::KEY_KP_UP;
        case XK_KP_Page_Up:
            return gameplay::Keyboard::KEY_KP_PG_UP;
        case XK_KP_Left:
            return gameplay::Keyboard::KEY_KP_LEFT;
        case XK_KP_5:
            return gameplay::Keyboard::KEY_KP_FIVE;
        case XK_KP_Right:
            return gameplay::Keyboard::KEY_KP_RIGHT;
        case XK_KP_End:
            return gameplay::Keyboard::KEY_KP_END;
        case XK_KP_Down:
            return gameplay::Keyboard::KEY_KP_DOWN;
        case XK_KP_Page_Down:
            return gameplay::Keyboard::KEY_KP_PG_DOWN;
        case XK_KP_Insert:
            return gameplay::Keyboard::KEY_KP_INSERT;
        case XK_KP_Delete:
            return gameplay::Keyboard::KEY_KP_DELETE;
        case XK_F1:
            return gameplay::Keyboard::KEY_F1;
        case XK_F2:
            return gameplay::Keyboard::KEY_F2;
        case XK_F3:
            return gameplay::Keyboard::KEY_F3;
        case XK_F4:
            return gameplay::Keyboard::KEY_F4;
        case XK_F5:
            return gameplay::Keyboard::KEY_F5;
        case XK_F6:
            return gameplay::Keyboard::KEY_F6;
        case XK_F7:
            return gameplay::Keyboard::KEY_F7;
        case XK_F8:
            return gameplay::Keyboard::KEY_F8;
        case XK_F9:
            return gameplay::Keyboard::KEY_F9;
        case XK_F10:
            return gameplay::Keyboard::KEY_F10;
        case XK_F11:
            return gameplay::Keyboard::KEY_F11;
        case XK_F12:
            return gameplay::Keyboard::KEY_F12;
        case XK_KP_Space:
        case XK_space:
            return gameplay::Keyboard::KEY_SPACE;
        case XK_parenright:
            return gameplay::Keyboard::KEY_RIGHT_PARENTHESIS;
        case XK_0:
            return gameplay::Keyboard::KEY_ZERO;
        case XK_exclam:
            return gameplay::Keyboard::KEY_EXCLAM;
        case XK_1:
            return gameplay::Keyboard::KEY_ONE;
        case XK_at:
            return gameplay::Keyboard::KEY_AT;
        case XK_2:
            return gameplay::Keyboard::KEY_TWO;
        case XK_numbersign:
            return gameplay::Keyboard::KEY_NUMBER;
        case XK_3:
            return gameplay::Keyboard::KEY_THREE;
        case XK_dollar:
            return gameplay::Keyboard::KEY_DOLLAR;
        case XK_4:
            return gameplay::Keyboard::KEY_FOUR;
        case XK_percent:
        case XK_asciicircum :
            return gameplay::Keyboard::KEY_CIRCUMFLEX;
            return gameplay::Keyboard::KEY_PERCENT;
        case XK_5:
            return gameplay::Keyboard::KEY_FIVE;
        case XK_6:
            return gameplay::Keyboard::KEY_SIX;
        case XK_ampersand:
            return gameplay::Keyboard::KEY_AMPERSAND;
        case XK_7:
            return gameplay::Keyboard::KEY_SEVEN;
        case XK_asterisk:
            return gameplay::Keyboard::KEY_ASTERISK;
        case XK_8:
            return gameplay::Keyboard::KEY_EIGHT;
        case XK_parenleft:
            return gameplay::Keyboard::KEY_LEFT_PARENTHESIS;
        case XK_9:
            return gameplay::Keyboard::KEY_NINE;
        case XK_equal:
            return gameplay::Keyboard::KEY_EQUAL;
        case XK_plus:
            return gameplay::Keyboard::KEY_PLUS;
        case XK_less:
            return gameplay::Keyboard::KEY_LESS_THAN;
        case XK_comma:
            return gameplay::Keyboard::KEY_COMMA;
        case XK_underscore:
            return gameplay::Keyboard::KEY_UNDERSCORE;
        case XK_minus:
            return gameplay::Keyboard::KEY_MINUS;
        case XK_greater:
            return gameplay::Keyboard::KEY_GREATER_THAN;
        case XK_period:
            return gameplay::Keyboard::KEY_PERIOD;
        case XK_colon:
            return gameplay::Keyboard::KEY_COLON;
        case XK_semicolon:
            return gameplay::Keyboard::KEY_SEMICOLON;
        case XK_question:
            return gameplay::Keyboard::KEY_QUESTION;
        case XK_slash:
            return gameplay::Keyboard::KEY_SLASH;
        case XK_grave:
            return gameplay::Keyboard::KEY_GRAVE;
        case XK_asciitilde:
            return gameplay::Keyboard::KEY_TILDE;
        case XK_braceleft:
            return gameplay::Keyboard::KEY_LEFT_BRACE;
        case XK_bracketleft:
            return gameplay::Keyboard::KEY_LEFT_BRACKET;
        case XK_bar:
            return gameplay::Keyboard::KEY_BAR;
        case XK_backslash:
            return gameplay::Keyboard::KEY_BACK_SLASH;
        case XK_braceright:
            return gameplay::Keyboard::KEY_RIGHT_BRACE;
        case XK_bracketright:
            return gameplay::Keyboard::KEY_RIGHT_BRACKET;
        case XK_quotedbl:
            return gameplay::Keyboard::KEY_QUOTE;
        case XK_apostrophe:
            return gameplay::Keyboard::KEY_APOSTROPHE;
        case XK_EuroSign:
            return gameplay::Keyboard::KEY_EURO;
        case XK_sterling:
            return gameplay::Keyboard::KEY_POUND;
        case XK_yen:
            return gameplay::Keyboard::KEY_YEN;
        case XK_periodcentered:
            return gameplay::Keyboard::KEY_MIDDLE_DOT;
        case XK_A:
            return gameplay::Keyboard::KEY_CAPITAL_A;
        case XK_a:
            return gameplay::Keyboard::KEY_A;
        case XK_B:
            return gameplay::Keyboard::KEY_CAPITAL_B;
        case XK_b:
            return gameplay::Keyboard::KEY_B;
        case XK_C:
            return gameplay::Keyboard::KEY_CAPITAL_C;
        case XK_c:
            return gameplay::Keyboard::KEY_C;
        case XK_D:
            return gameplay::Keyboard::KEY_CAPITAL_D;
        case XK_d:
            return gameplay::Keyboard::KEY_D;
        case XK_E:
            return gameplay::Keyboard::KEY_CAPITAL_E;
        case XK_e:
            return gameplay::Keyboard::KEY_E;
        case XK_F:
            return gameplay::Keyboard::KEY_CAPITAL_F;
        case XK_f:
            return gameplay::Keyboard::KEY_F;
        case XK_G:
            return gameplay::Keyboard::KEY_CAPITAL_G;
        case XK_g:
            return gameplay::Keyboard::KEY_G;
        case XK_H:
            return gameplay::Keyboard::KEY_CAPITAL_H;
        case XK_h:
            return gameplay::Keyboard::KEY_H;
        case XK_I:
            return gameplay::Keyboard::KEY_CAPITAL_I;
        case XK_i:
            return gameplay::Keyboard::KEY_I;
        case XK_J:
            return gameplay::Keyboard::KEY_CAPITAL_J;
        case XK_j:
            return gameplay::Keyboard::KEY_J;
        case XK_K:
            return gameplay::Keyboard::KEY_CAPITAL_K;
        case XK_k:
            return gameplay::Keyboard::KEY_K;
        case XK_L:
            return gameplay::Keyboard::KEY_CAPITAL_L;
        case XK_l:
            return gameplay::Keyboard::KEY_L;
        case XK_M:
            return gameplay::Keyboard::KEY_CAPITAL_M;
        case XK_m:
            return gameplay::Keyboard::KEY_M;
        case XK_N:
            return gameplay::Keyboard::KEY_CAPITAL_N;
        case XK_n:
            return gameplay::Keyboard::KEY_N;
        case XK_O:
            return gameplay::Keyboard::KEY_CAPITAL_O;
        case XK_o:
            return gameplay::Keyboard::KEY_O;
        case XK_P:
            return gameplay::Keyboard::KEY_CAPITAL_P;
        case XK_p:
            return gameplay::Keyboard::KEY_P;
        case XK_Q:
            return gameplay::Keyboard::KEY_CAPITAL_Q;
        case XK_q:
            return gameplay::Keyboard::KEY_Q;
        case XK_R:
            return gameplay::Keyboard::KEY_CAPITAL_R;
        case XK_r:
            return gameplay::Keyboard::KEY_R;
        case XK_S:
            return gameplay::Keyboard::KEY_CAPITAL_S;
        case XK_s:
            return gameplay::Keyboard::KEY_S;
        case XK_T:
            return gameplay::Keyboard::KEY_CAPITAL_T;
        case XK_t:
            return gameplay::Keyboard::KEY_T;
        case XK_U:
            return gameplay::Keyboard::KEY_CAPITAL_U;
        case XK_u:
            return gameplay::Keyboard::KEY_U;
        case XK_V:
            return gameplay::Keyboard::KEY_CAPITAL_V;
        case XK_v:
            return gameplay::Keyboard::KEY_V;
        case XK_W:
            return gameplay::Keyboard::KEY_CAPITAL_W;
        case XK_w:
            return gameplay::Keyboard::KEY_W;
        case XK_X:
            return gameplay::Keyboard::KEY_CAPITAL_X;
        case XK_x:
            return gameplay::Keyboard::KEY_X;
        case XK_Y:
            return gameplay::Keyboard::KEY_CAPITAL_Y;
        case XK_y:
            return gameplay::Keyboard::KEY_Y;
        case XK_Z:
            return gameplay::Keyboard::KEY_CAPITAL_Z;
        case XK_z:
            return gameplay::Keyboard::KEY_Z;
        default:
            return gameplay::Keyboard::KEY_NONE;
    }
}

/**
 * Returns the unicode value for the given keycode or zero if the key is not a valid printable character.
 */
static int getUnicode(gameplay::Keyboard::Key key)
{
    switch (key)
    {
        case gameplay::Keyboard::KEY_BACKSPACE:
            return 0x0008;
        case gameplay::Keyboard::KEY_TAB:
            return 0x0009;
        case gameplay::Keyboard::KEY_RETURN:
        case gameplay::Keyboard::KEY_KP_ENTER:
            return 0x000A;
        case gameplay::Keyboard::KEY_ESCAPE:
            return 0x001B;
        case gameplay::Keyboard::KEY_SPACE:
        case gameplay::Keyboard::KEY_EXCLAM:
        case gameplay::Keyboard::KEY_QUOTE:
        case gameplay::Keyboard::KEY_NUMBER:
        case gameplay::Keyboard::KEY_DOLLAR:
        case gameplay::Keyboard::KEY_PERCENT:
        case gameplay::Keyboard::KEY_CIRCUMFLEX:
        case gameplay::Keyboard::KEY_AMPERSAND:
        case gameplay::Keyboard::KEY_APOSTROPHE:
        case gameplay::Keyboard::KEY_LEFT_PARENTHESIS:
        case gameplay::Keyboard::KEY_RIGHT_PARENTHESIS:
        case gameplay::Keyboard::KEY_ASTERISK:
        case gameplay::Keyboard::KEY_PLUS:
        case gameplay::Keyboard::KEY_COMMA:
        case gameplay::Keyboard::KEY_MINUS:
        case gameplay::Keyboard::KEY_PERIOD:
        case gameplay::Keyboard::KEY_SLASH:
        case gameplay::Keyboard::KEY_ZERO:
        case gameplay::Keyboard::KEY_ONE:
        case gameplay::Keyboard::KEY_TWO:
        case gameplay::Keyboard::KEY_THREE:
        case gameplay::Keyboard::KEY_FOUR:
        case gameplay::Keyboard::KEY_FIVE:
        case gameplay::Keyboard::KEY_SIX:
        case gameplay::Keyboard::KEY_SEVEN:
        case gameplay::Keyboard::KEY_EIGHT:
        case gameplay::Keyboard::KEY_NINE:
        case gameplay::Keyboard::KEY_COLON:
        case gameplay::Keyboard::KEY_SEMICOLON:
        case gameplay::Keyboard::KEY_LESS_THAN:
        case gameplay::Keyboard::KEY_EQUAL:
        case gameplay::Keyboard::KEY_GREATER_THAN:
        case gameplay::Keyboard::KEY_QUESTION:
        case gameplay::Keyboard::KEY_AT:
        case gameplay::Keyboard::KEY_CAPITAL_A:
        case gameplay::Keyboard::KEY_CAPITAL_B:
        case gameplay::Keyboard::KEY_CAPITAL_C:
        case gameplay::Keyboard::KEY_CAPITAL_D:
        case gameplay::Keyboard::KEY_CAPITAL_E:
        case gameplay::Keyboard::KEY_CAPITAL_F:
        case gameplay::Keyboard::KEY_CAPITAL_G:
        case gameplay::Keyboard::KEY_CAPITAL_H:
        case gameplay::Keyboard::KEY_CAPITAL_I:
        case gameplay::Keyboard::KEY_CAPITAL_J:
        case gameplay::Keyboard::KEY_CAPITAL_K:
        case gameplay::Keyboard::KEY_CAPITAL_L:
        case gameplay::Keyboard::KEY_CAPITAL_M:
        case gameplay::Keyboard::KEY_CAPITAL_N:
        case gameplay::Keyboard::KEY_CAPITAL_O:
        case gameplay::Keyboard::KEY_CAPITAL_P:
        case gameplay::Keyboard::KEY_CAPITAL_Q:
        case gameplay::Keyboard::KEY_CAPITAL_R:
        case gameplay::Keyboard::KEY_CAPITAL_S:
        case gameplay::Keyboard::KEY_CAPITAL_T:
        case gameplay::Keyboard::KEY_CAPITAL_U:
        case gameplay::Keyboard::KEY_CAPITAL_V:
        case gameplay::Keyboard::KEY_CAPITAL_W:
        case gameplay::Keyboard::KEY_CAPITAL_X:
        case gameplay::Keyboard::KEY_CAPITAL_Y:
        case gameplay::Keyboard::KEY_CAPITAL_Z:
        case gameplay::Keyboard::KEY_LEFT_BRACKET:
        case gameplay::Keyboard::KEY_BACK_SLASH:
        case gameplay::Keyboard::KEY_RIGHT_BRACKET:
        case gameplay::Keyboard::KEY_UNDERSCORE:
        case gameplay::Keyboard::KEY_GRAVE:
        case gameplay::Keyboard::KEY_A:
        case gameplay::Keyboard::KEY_B:
        case gameplay::Keyboard::KEY_C:
        case gameplay::Keyboard::KEY_D:
        case gameplay::Keyboard::KEY_E:
        case gameplay::Keyboard::KEY_F:
        case gameplay::Keyboard::KEY_G:
        case gameplay::Keyboard::KEY_H:
        case gameplay::Keyboard::KEY_I:
        case gameplay::Keyboard::KEY_J:
        case gameplay::Keyboard::KEY_K:
        case gameplay::Keyboard::KEY_L:
        case gameplay::Keyboard::KEY_M:
        case gameplay::Keyboard::KEY_N:
        case gameplay::Keyboard::KEY_O:
        case gameplay::Keyboard::KEY_P:
        case gameplay::Keyboard::KEY_Q:
        case gameplay::Keyboard::KEY_R:
        case gameplay::Keyboard::KEY_S:
        case gameplay::Keyboard::KEY_T:
        case gameplay::Keyboard::KEY_U:
        case gameplay::Keyboard::KEY_V:
        case gameplay::Keyboard::KEY_W:
        case gameplay::Keyboard::KEY_X:
        case gameplay::Keyboard::KEY_Y:
        case gameplay::Keyboard::KEY_Z:
        case gameplay::Keyboard::KEY_LEFT_BRACE:
        case gameplay::Keyboard::KEY_BAR:
        case gameplay::Keyboard::KEY_RIGHT_BRACE:
        case gameplay::Keyboard::KEY_TILDE:
            return key;
        default:
            return 0;
    }
}

namespace gameplay
{

extern void print(const char* format, ...)
{
    GP_ASSERT(format);
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stdout, format, argptr);
    va_end(argptr);
}

extern int strcmpnocase(const char* s1, const char* s2)
{
    return strcasecmp(s1, s2);
}

Platform::Platform(Game* game) : _game(game)
{
}

Platform::~Platform()
{
}

Platform* Platform::create(Game* game)
{

    GP_ASSERT(game);

    FileSystem::setResourcePath("./");
    Platform* platform = new Platform(game);

    // Get the window configuration values
    const char *title = NULL;
    int __width = 0, __height = 0, __samples = 0;
    bool fullscreen = false;
    
    // default window sizes come from canvas
    int tmpFullscreen = 0;
    emscripten_get_canvas_size(&__width, &__height, &tmpFullscreen);
    
    if (game->getConfig())
    {
        Properties* config = game->getConfig()->getNamespace("window", true);
        if (config)
        {
            // Read window title.
            title = config->getString("title");

            // Read window rect.
            int width = config->getInt("width");
            int height = config->getInt("height");
            int samples = config->getInt("samples");
            fullscreen = config->getBool("fullscreen");

            if (width != 0) __width = width;
            if (height != 0) __height = height;
            if (samples != 0) __samples = samples;
        }
    }
    
    __windowSize[0] = __width;
    __windowSize[1] = __height;
    emscripten_set_canvas_size(__width, __height);

    // Construct a fake argv array for GLUT. LLVM seems extra picky about what
    // it will accept here, so we allocate a "real" argv array on the heap, and
    // tear it down after init.
    char *arg1 = (char*)malloc(1);
    char **dummyArgv = (char**)malloc(sizeof(char*));
    dummyArgv[0] = arg1;
    free(dummyArgv[0]);
    free(dummyArgv);

    // Hard-coded to 32-bit/OpenGL ES 2.0.
    // NOTE: EGL_SAMPLE_BUFFERS, EGL_SAMPLES and EGL_DEPTH_SIZE MUST remain at the beginning of the attribute list
    // since they are expected to be at indices 0-5 in config fallback code later.
    // EGL_DEPTH_SIZE is also expected to
    EGLint eglConfigAttrs[] =
    {
        EGL_SAMPLE_BUFFERS, __samples > 0 ? 1 : 0,
        EGL_SAMPLES, __samples,
        EGL_DEPTH_SIZE, 24,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    __multiSampling = __samples > 0;

    EGLint eglConfigCount;
    const EGLint eglContextAttrs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    const EGLint eglSurfaceAttrs[] =
    {
        EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
        EGL_NONE
    };

    if (__eglDisplay == EGL_NO_DISPLAY && __eglContext == EGL_NO_CONTEXT)
    {
        // Get the EGL display and initialize.
        __eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (__eglDisplay == EGL_NO_DISPLAY)
        {
            GP_ERROR("eglGetDisplay");
            return NULL;
        }

        if (eglInitialize(__eglDisplay, NULL, NULL) != EGL_TRUE)
        {
            GP_ERROR("eglInitialize");
            return NULL;
        }

        // Try both 24 and 16-bit depth sizes since some hardware (i.e. Tegra) does not support 24-bit depth
        bool validConfig = false;
        EGLint depthSizes[] = { 24, 16 };
        for (unsigned int i = 0; i < 2; ++i)
        {
            eglConfigAttrs[1] = __samples > 0 ? 1 : 0;
            eglConfigAttrs[3] = __samples;
            eglConfigAttrs[5] = depthSizes[i];

            if (eglChooseConfig(__eglDisplay, eglConfigAttrs, &__eglConfig, 1, &eglConfigCount) == EGL_TRUE && eglConfigCount > 0)
            {
                validConfig = true;
                break;
            }

            if (__samples > 0)
            {
                // Try lowering the MSAA sample size until we find a  config
                int sampleCount = __samples;
                while (sampleCount)
                {
                    GP_WARN("No EGL config found for depth_size=%d and samples=%d. Trying samples=%d instead.", depthSizes[i], sampleCount, sampleCount / 2);
                    sampleCount /= 2;
                    eglConfigAttrs[1] = sampleCount > 0 ? 1 : 0;
                    eglConfigAttrs[3] = sampleCount;
                    if (eglChooseConfig(__eglDisplay, eglConfigAttrs, &__eglConfig, 1, &eglConfigCount) == EGL_TRUE && eglConfigCount > 0)
                    {
                        validConfig = true;
                        break;
                    }
                }

                __multiSampling = sampleCount > 0;

                if (validConfig)
                    break;
            }
            else
            {
                GP_WARN("No EGL config found for depth_size=%d.", depthSizes[i]);
            }
        }

        if (!validConfig)
        {
            GP_ERROR("eglChooseConfig");
            return NULL;
        }

        __eglContext = eglCreateContext(__eglDisplay, __eglConfig, EGL_NO_CONTEXT, eglContextAttrs);
        if (__eglContext == EGL_NO_CONTEXT)
        {
            GP_ERROR("eglCreateContext");
            return NULL;
        }
    }

    __eglSurface = eglCreateWindowSurface(__eglDisplay, __eglConfig, NULL, eglSurfaceAttrs);
    if (__eglSurface == EGL_NO_SURFACE)
    {
        GP_ERROR("eglCreateWindowSurface");
        return NULL;
    }

    if (eglMakeCurrent(__eglDisplay, __eglSurface, __eglSurface, __eglContext) != EGL_TRUE)
    {
        GP_ERROR("eglMakeCurrent");
        return NULL;
    }

    // Initialize OpenGL ES extensions.
    __glExtensions = (const char*)glGetString(GL_EXTENSIONS);

    if (strstr(__glExtensions, "GL_OES_vertex_array_object") || strstr(__glExtensions, "GL_ARB_vertex_array_object"))
    {
        glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
        glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
        glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
        //glIsVertexArray = (PFNGLISVERTEXARRAYOESPROC)eglGetProcAddress("glIsVertexArrayOES");
    }

    // Set vsync.
    eglSwapInterval(__eglDisplay, WINDOW_VSYNC ? 1 : 0);

    return platform;
}

void cleanupX11()
{
}

double timespec2millis(struct timespec *a)
{
    GP_ASSERT(a);
    return (1000.0 * a->tv_sec) + (0.000001 * a->tv_nsec);
}

void updateWindowSize()
{
    // GP_ASSERT(__display);
    // GP_ASSERT(__window);
    // XWindowAttributes windowAttrs;
    // XGetWindowAttributes(__display, __window, &windowAttrs);
    // __windowSize[0] = windowAttrs.width;
    // __windowSize[1] = windowAttrs.height;
}

EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
    //printf("%d screen: (%ld,%ld), client: (%ld,%ld),%s%s%s%s button: %hu, buttons: %hu, movement: (%ld,%ld), canvas: (%ld,%ld)\n",
    //         eventType, e->screenX, e->screenY, e->clientX, e->clientY,
    //         e->ctrlKey ? " CTRL" : "", e->shiftKey ? " SHIFT" : "", e->altKey ? " ALT" : "", e->metaKey ? " META" : "",
    //         e->button, e->buttons, e->movementX, e->movementY, e->canvasX, e->canvasY);
    int x = e->canvasX;
    int y = e->canvasY;
    gameplay::Mouse::MouseEvent mouseEvt;
    bool eventConsumed = false;
    
    if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
    {
        switch(e->button)
        {
        default:
            __leftMouseButtonPressed = true;
            mouseEvt = gameplay::Mouse::MOUSE_PRESS_LEFT_BUTTON;
            break;
        case 1:
            mouseEvt = gameplay::Mouse::MOUSE_PRESS_MIDDLE_BUTTON;
            break;
        case 2:
            mouseEvt = gameplay::Mouse::MOUSE_PRESS_RIGHT_BUTTON;
            break;
        }
        eventConsumed = gameplay::Platform::mouseEventInternal(mouseEvt, x, y, 0);
        if (!eventConsumed && e->button == 0)
        {
            gameplay::Platform::touchEventInternal(gameplay::Touch::TOUCH_PRESS, x, y, 0, true);
        }
    }
    if (eventType == EMSCRIPTEN_EVENT_MOUSEUP)
    {
        switch(e->button)
        {
        default:
            __leftMouseButtonPressed = false;
            mouseEvt = gameplay::Mouse::MOUSE_RELEASE_LEFT_BUTTON;
            break;
        case 1:
            mouseEvt = gameplay::Mouse::MOUSE_RELEASE_MIDDLE_BUTTON;
            break;
        case 2:
            mouseEvt = gameplay::Mouse::MOUSE_RELEASE_RIGHT_BUTTON;
            break;
        }
        eventConsumed = gameplay::Platform::mouseEventInternal(mouseEvt, x, y, 0);
        if (!eventConsumed && e->button == 0)
        {
            gameplay::Platform::touchEventInternal(gameplay::Touch::TOUCH_RELEASE, x, y, 0, true);
        }
    }
    if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE && (e->movementX != 0 || e->movementY != 0))
    {
        __mouseCapturePointX = x;
        __mouseCapturePointY = y;
        eventConsumed = gameplay::Platform::mouseEventInternal(gameplay::Mouse::MOUSE_MOVE, x, y, 0);
        if (!eventConsumed && e->button == 0)
        {
            if (__leftMouseButtonPressed)
            {
                gameplay::Platform::touchEventInternal(gameplay::Touch::TOUCH_MOVE, x, y, 0, true);
            }
        }
    }

    return 1;
}

EM_BOOL wheel_callback(int eventType, const EmscriptenWheelEvent *e, void *userData)
{
    //printf("%d wheel %.3f %.3f %.3f %ld\n",
    //       eventType, e->deltaX, e->deltaY, e->deltaZ, e->deltaMode);
    
    if (eventType == EMSCRIPTEN_EVENT_WHEEL)
    {
        gameplay::Platform::mouseEventInternal(gameplay::Mouse::MOUSE_WHEEL, __mouseCapturePointX, __mouseCapturePointY, -e->deltaY * 0.01);
    }
    
    return 1;
}
    
void main_loop_iter(void* _game)
{
    Game* game = (Game*)_game;
    // if(__lastKeyCode)
    // {
    //     struct timeval t;
    //     gettimeofday(&t, NULL);
    //     double curTime = timeval2millis(&t);

    //     if(curTime - __lastKeyPressTime > 200)
    //     {
    //         Keyboard::Key gpkey = getKey(__lastKeyCode);
    //         gameplay::Platform::keyEventInternal(gameplay::Keyboard::KEY_RELEASE, gpkey);
    //         __lastKeyCode = 0;
    //     }
    // }

    if (game)
    {
        // Game state will be uninitialized if game was closed through Game::exit()
        if (game->getState() == Game::UNINITIALIZED)
            return;

        game->frame();
    }

    eglSwapBuffers(__eglDisplay, __eglSurface);
}

int Platform::enterMessagePump()
{
    GP_ASSERT(_game);

    updateWindowSize();

    static bool shiftDown = false;
    static bool capsOn = false;
    static XEvent evt;

    // Get the initial time.
    clock_gettime(CLOCK_REALTIME, &__timespec);
    __timeStart = timespec2millis(&__timespec);
    __timeAbsolute = 0L;

    // Run the game.
    _game->run();

    emscripten_set_mousedown_callback("#canvas", 0, true, mouse_callback);
    emscripten_set_mouseup_callback(0, 0, true, mouse_callback);
    emscripten_set_mousemove_callback(0, 0, true, mouse_callback);
    emscripten_set_wheel_callback("#canvas", 0, true, wheel_callback);
    emscripten_set_main_loop_arg(&main_loop_iter, (void *)_game, 0, 1);

    return 0;
}

void Platform::signalShutdown()
{
}

bool Platform::canExit()
{
    return true;
}

unsigned int Platform::getDisplayWidth()
{
    return __windowSize[0];
}

unsigned int Platform::getDisplayHeight()
{
    return __windowSize[1];
}

double Platform::getAbsoluteTime()
{

    clock_gettime(CLOCK_REALTIME, &__timespec);
    double now = timespec2millis(&__timespec);
    __timeAbsolute = now - __timeStart;

    return __timeAbsolute;
}

void Platform::setAbsoluteTime(double time)
{
    __timeAbsolute = time;
}

bool Platform::isVsync()
{
    return __vsync;
}

void Platform::setVsync(bool enable)
{
    __vsync = enable;
    eglSwapInterval(__eglDisplay, __vsync ? 1 : 0);
}

void Platform::swapBuffers()
{
    eglSwapBuffers(__eglDisplay, __eglSurface);
}

void Platform::sleep(long ms)
{
    usleep(ms * 1000);
}

void Platform::setMultiSampling(bool enabled)
{
    if (enabled == __multiSampling)
    {
        return;
    }

        // TODO
        __multiSampling = enabled;
}

    bool Platform::isMultiSampling()
    {
        return __multiSampling;
    }

void Platform::setMultiTouch(bool enabled)
{
    // not supported
}

bool Platform::isMultiTouch()
{
    return false;
}

bool Platform::hasAccelerometer()
{
    return false;
}

void Platform::getAccelerometerValues(float* pitch, float* roll)
{
    GP_ASSERT(pitch);
    GP_ASSERT(roll);

    *pitch = 0;
    *roll = 0;
}

void Platform::getSensorValues(float* accelX, float* accelY, float* accelZ, float* gyroX, float* gyroY, float* gyroZ)
{
    if (accelX)
    {
        *accelX = 0;
    }

    if (accelY)
    {
        *accelY = 0;
    }

    if (accelZ)
    {
        *accelZ = 0;
    }

    if (gyroX)
    {
        *gyroX = 0;
    }

    if (gyroY)
    {
        *gyroY = 0;
    }

    if (gyroZ)
    {
        *gyroZ = 0;
    }
}

void Platform::getArguments(int* argc, char*** argv)
{
    if (argc)
        *argc = __argc;
    if (argv)
        *argv = __argv;
}

bool Platform::hasMouse()
{
    return true;
}

void Platform::setMouseCaptured(bool captured)
{
}

bool Platform::isMouseCaptured()
{
    return __mouseCaptured;
}

void Platform::setCursorVisible(bool visible)
{
}

bool Platform::isCursorVisible()
{
    return __cursorVisible;
}

void Platform::displayKeyboard(bool display)
{
    // not supported
}

void Platform::shutdownInternal()
{
    Game::getInstance()->shutdown();
}

bool Platform::isGestureSupported(Gesture::GestureEvent evt)
{
    return false;
}

void Platform::registerGesture(Gesture::GestureEvent evt)
{
}

void Platform::unregisterGesture(Gesture::GestureEvent evt)
{
}

bool Platform::isGestureRegistered(Gesture::GestureEvent evt)
{
    return false;
}

void Platform::pollGamepadState(Gamepad* gamepad)
{
}

bool Platform::launchURL(const char* url)
{
    EM_ASM_({window.open(Module.Pointer_stringify($0),'_blank');}, url);
    return true;
}

std::string Platform::displayFileDialog(size_t mode, const char* title, const char* filterDescription, const char* filterExtensions, const char* initialDirectory)
{
    return "";
}

const char * Platform::getTemporaryFolderPath( )
{
    return "";
}

const char * Platform::getDocumentsFolderPath( )
{
    return "";
}

const char * Platform::getAppPrivateFolderPath( )
{
    return "";
}

const char * Platform::getUserAgentString( )
{
    return "";
}

}

#endif
#endif
