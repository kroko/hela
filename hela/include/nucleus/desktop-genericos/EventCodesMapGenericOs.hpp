#ifndef EventCodesMapGenericOs_hpp
#define EventCodesMapGenericOs_hpp

namespace hela { namespace nucleus {

  // GLFW defines https://www.glfw.org/docs/latest/group__buttons.html
  // https://raw.githubusercontent.com/torvalds/linux/master/include/uapi/linux/input-event-codes.h
  // https://www.kernel.org/doc/Documentation/input/event-codes.txt
  // used this and constructed switch table using regex, no checks done
  // https://github.com/glfw/glfw/blob/5f9cbd0ebcc9ca8bfddbf99e78a54fb02dd030d7/src/wl_init.c#L904
  unsigned short GLFW_KEY_TO_LINUX_MAP(int key) {
    int remapped = key;
    switch (key) {
      case GLFW_KEY_GRAVE_ACCENT : remapped = KEY_GRAVE; break;
      case GLFW_KEY_1 : remapped = KEY_1; break;
      case GLFW_KEY_2 : remapped = KEY_2; break;
      case GLFW_KEY_3 : remapped = KEY_3; break;
      case GLFW_KEY_4 : remapped = KEY_4; break;
      case GLFW_KEY_5 : remapped = KEY_5; break;
      case GLFW_KEY_6 : remapped = KEY_6; break;
      case GLFW_KEY_7 : remapped = KEY_7; break;
      case GLFW_KEY_8 : remapped = KEY_8; break;
      case GLFW_KEY_9 : remapped = KEY_9; break;
      case GLFW_KEY_0 : remapped = KEY_0; break;
      case GLFW_KEY_SPACE : remapped = KEY_SPACE; break;
      case GLFW_KEY_MINUS : remapped = KEY_MINUS; break;
      case GLFW_KEY_EQUAL : remapped = KEY_EQUAL; break;
      case GLFW_KEY_Q : remapped = KEY_Q; break;
      case GLFW_KEY_W : remapped = KEY_W; break;
      case GLFW_KEY_E : remapped = KEY_E; break;
      case GLFW_KEY_R : remapped = KEY_R; break;
      case GLFW_KEY_T : remapped = KEY_T; break;
      case GLFW_KEY_Y : remapped = KEY_Y; break;
      case GLFW_KEY_U : remapped = KEY_U; break;
      case GLFW_KEY_I : remapped = KEY_I; break;
      case GLFW_KEY_O : remapped = KEY_O; break;
      case GLFW_KEY_P : remapped = KEY_P; break;
      case GLFW_KEY_LEFT_BRACKET : remapped = KEY_LEFTBRACE; break;
      case GLFW_KEY_RIGHT_BRACKET : remapped = KEY_RIGHTBRACE; break;
      case GLFW_KEY_A : remapped = KEY_A; break;
      case GLFW_KEY_S : remapped = KEY_S; break;
      case GLFW_KEY_D : remapped = KEY_D; break;
      case GLFW_KEY_F : remapped = KEY_F; break;
      case GLFW_KEY_G : remapped = KEY_G; break;
      case GLFW_KEY_H : remapped = KEY_H; break;
      case GLFW_KEY_J : remapped = KEY_J; break;
      case GLFW_KEY_K : remapped = KEY_K; break;
      case GLFW_KEY_L : remapped = KEY_L; break;
      case GLFW_KEY_SEMICOLON : remapped = KEY_SEMICOLON; break;
      case GLFW_KEY_APOSTROPHE : remapped = KEY_APOSTROPHE; break;
      case GLFW_KEY_Z : remapped = KEY_Z; break;
      case GLFW_KEY_X : remapped = KEY_X; break;
      case GLFW_KEY_C : remapped = KEY_C; break;
      case GLFW_KEY_V : remapped = KEY_V; break;
      case GLFW_KEY_B : remapped = KEY_B; break;
      case GLFW_KEY_N : remapped = KEY_N; break;
      case GLFW_KEY_M : remapped = KEY_M; break;
      case GLFW_KEY_COMMA : remapped = KEY_COMMA; break;
      case GLFW_KEY_PERIOD : remapped = KEY_DOT; break;
      case GLFW_KEY_SLASH : remapped = KEY_SLASH; break;
      case GLFW_KEY_BACKSLASH : remapped = KEY_BACKSLASH; break;
      case GLFW_KEY_ESCAPE : remapped = KEY_ESC; break;
      case GLFW_KEY_TAB : remapped = KEY_TAB; break;
      case GLFW_KEY_LEFT_SHIFT : remapped = KEY_LEFTSHIFT; break;
      case GLFW_KEY_RIGHT_SHIFT : remapped = KEY_RIGHTSHIFT; break;
      case GLFW_KEY_LEFT_CONTROL : remapped = KEY_LEFTCTRL; break;
      case GLFW_KEY_RIGHT_CONTROL : remapped = KEY_RIGHTCTRL; break;
      case GLFW_KEY_LEFT_ALT : remapped = KEY_LEFTALT; break;
      case GLFW_KEY_RIGHT_ALT : remapped = KEY_RIGHTALT; break;
      case GLFW_KEY_LEFT_SUPER : remapped = KEY_LEFTMETA; break;
      case GLFW_KEY_RIGHT_SUPER : remapped = KEY_RIGHTMETA; break;
      case GLFW_KEY_MENU : remapped = KEY_MENU; break;
      case GLFW_KEY_NUM_LOCK : remapped = KEY_NUMLOCK; break;
      case GLFW_KEY_CAPS_LOCK : remapped = KEY_CAPSLOCK; break;
      case GLFW_KEY_PRINT_SCREEN : remapped = KEY_PRINT; break;
      case GLFW_KEY_SCROLL_LOCK : remapped = KEY_SCROLLLOCK; break;
      case GLFW_KEY_PAUSE : remapped = KEY_PAUSE; break;
      case GLFW_KEY_DELETE : remapped = KEY_DELETE; break;
      case GLFW_KEY_BACKSPACE : remapped = KEY_BACKSPACE; break;
      case GLFW_KEY_ENTER : remapped = KEY_ENTER; break;
      case GLFW_KEY_HOME : remapped = KEY_HOME; break;
      case GLFW_KEY_END : remapped = KEY_END; break;
      case GLFW_KEY_PAGE_UP : remapped = KEY_PAGEUP; break;
      case GLFW_KEY_PAGE_DOWN : remapped = KEY_PAGEDOWN; break;
      case GLFW_KEY_INSERT : remapped = KEY_INSERT; break;
      case GLFW_KEY_LEFT : remapped = KEY_LEFT; break;
      case GLFW_KEY_RIGHT : remapped = KEY_RIGHT; break;
      case GLFW_KEY_DOWN : remapped = KEY_DOWN; break;
      case GLFW_KEY_UP : remapped = KEY_UP; break;
      case GLFW_KEY_F1 : remapped = KEY_F1; break;
      case GLFW_KEY_F2 : remapped = KEY_F2; break;
      case GLFW_KEY_F3 : remapped = KEY_F3; break;
      case GLFW_KEY_F4 : remapped = KEY_F4; break;
      case GLFW_KEY_F5 : remapped = KEY_F5; break;
      case GLFW_KEY_F6 : remapped = KEY_F6; break;
      case GLFW_KEY_F7 : remapped = KEY_F7; break;
      case GLFW_KEY_F8 : remapped = KEY_F8; break;
      case GLFW_KEY_F9 : remapped = KEY_F9; break;
      case GLFW_KEY_F10 : remapped = KEY_F10; break;
      case GLFW_KEY_F11 : remapped = KEY_F11; break;
      case GLFW_KEY_F12 : remapped = KEY_F12; break;
      case GLFW_KEY_F13 : remapped = KEY_F13; break;
      case GLFW_KEY_F14 : remapped = KEY_F14; break;
      case GLFW_KEY_F15 : remapped = KEY_F15; break;
      case GLFW_KEY_F16 : remapped = KEY_F16; break;
      case GLFW_KEY_F17 : remapped = KEY_F17; break;
      case GLFW_KEY_F18 : remapped = KEY_F18; break;
      case GLFW_KEY_F19 : remapped = KEY_F19; break;
      case GLFW_KEY_F20 : remapped = KEY_F20; break;
      case GLFW_KEY_F21 : remapped = KEY_F21; break;
      case GLFW_KEY_F22 : remapped = KEY_F22; break;
      case GLFW_KEY_F23 : remapped = KEY_F23; break;
      case GLFW_KEY_F24 : remapped = KEY_F24; break;
      case GLFW_KEY_KP_DIVIDE : remapped = KEY_KPSLASH; break;
      case GLFW_KEY_KP_MULTIPLY : remapped = KEY_KPDOT; break;
      case GLFW_KEY_KP_SUBTRACT : remapped = KEY_KPMINUS; break;
      case GLFW_KEY_KP_ADD : remapped = KEY_KPPLUS; break;
      case GLFW_KEY_KP_0 : remapped = KEY_KP0; break;
      case GLFW_KEY_KP_1 : remapped = KEY_KP1; break;
      case GLFW_KEY_KP_2 : remapped = KEY_KP2; break;
      case GLFW_KEY_KP_3 : remapped = KEY_KP3; break;
      case GLFW_KEY_KP_4 : remapped = KEY_KP4; break;
      case GLFW_KEY_KP_5 : remapped = KEY_KP5; break;
      case GLFW_KEY_KP_6 : remapped = KEY_KP6; break;
      case GLFW_KEY_KP_7 : remapped = KEY_KP7; break;
      case GLFW_KEY_KP_8 : remapped = KEY_KP8; break;
      case GLFW_KEY_KP_9 : remapped = KEY_KP9; break;
      case GLFW_KEY_KP_DECIMAL : remapped = KEY_KPCOMMA; break;
      case GLFW_KEY_KP_EQUAL : remapped = KEY_KPEQUAL; break;
      case GLFW_KEY_KP_ENTER : remapped = KEY_KPENTER; break;
      default : remapped = key; break;
    }
    return static_cast<unsigned short>(remapped);
  }

  unsigned short GLFW_MOUSE_BUTTON_TO_LINUX_MAP(int button) {
    int remapped = button;
    switch (button) {
      case GLFW_MOUSE_BUTTON_1 : remapped = BTN_LEFT; break;
      case GLFW_MOUSE_BUTTON_2 : remapped = BTN_RIGHT; break;
      case GLFW_MOUSE_BUTTON_3 : remapped = BTN_MIDDLE; break;
      case GLFW_MOUSE_BUTTON_4 : remapped = BTN_SIDE; break;
      case GLFW_MOUSE_BUTTON_5 : remapped = BTN_EXTRA; break;
      case GLFW_MOUSE_BUTTON_6 : remapped = BTN_FORWARD; break;
      case GLFW_MOUSE_BUTTON_7 : remapped = BTN_BACK; break;
      case GLFW_MOUSE_BUTTON_8 : remapped = BTN_TASK; break;
      default : remapped = button; break;
    }
    return static_cast<unsigned short>(remapped);
  }

  int GLFW_ACTION_TO_LINUX_MAP(int action) {
    int remapped = action;
    switch (action) {
      case GLFW_RELEASE : remapped = HELA_RELEASE; break;
      case GLFW_PRESS : remapped = HELA_PRESS; break;
      case GLFW_REPEAT : remapped = HELA_REPEAT; break;
      default : remapped = action; break;
    }
    return remapped;
  }

} }

#endif // EventCodesMapGenericOs_hpp
