#include "legoinputmanager.h"

#include "decomp.h"

//DECOMP_SIZE_ASSERT(LegoInputManager, 0x338); // 0x10059085

// OFFSET: LEGO1 0x1005b790 STUB
LegoInputManager::LegoInputManager()
{
  m_unknown5C = 0;
  m_unknown64 = NULL;
  m_unknown60 = NULL;
  m_unknown68 = 0;
  m_unknown80 = 0;
  m_timer = 0;
  m_unknown6C = 0;
  m_unknown70 = 0;
  m_unknown84 = NULL;
  m_unknown81 = 0;
  m_unknown88 = 0;
  m_directinputInterface = NULL;
  m_directinputDeviceInterface = NULL;
  m_unknown94 = 0;
  m_unknown195 = 0;
  m_joyid = (UINT)-1;
  m_unknown19C = (UINT)-1;
  m_joystickIndex = 0;
  m_useJoystick = 0;
  m_unknown74 = 0x19;
  m_timeout = 1000;
}

// OFFSET: LEGO1 0x1005b8f0 STUB
LegoInputManager::~LegoInputManager()
{
  // TODO
}

// OFFSET: LEGO1 0x1005c740 STUB
void LegoInputManager::QueueEvent(NotificationId id, unsigned char p2, MxLong p3, MxLong p4, unsigned char p5)
{
  // TODO
}

// OFFSET: LEGO1 0x1005c470 STUB
void LegoInputManager::Register(MxCore *)
{
  // TODO
}

// OFFSET: LEGO1 0x1005c5c0 STUB
void LegoInputManager::UnRegister(MxCore *)
{
  // TODO
}

// OFFSET: LEGO1 0x1005b8b0 STUB
//MxLong LegoInputManager::Tickle()
//{
//  // TODO
//
//  return 0;
//}

// this function currently does not match 100% due to some member variables
// being at the wrong offsets, but the functionality is the same

// OFFSET: LEGO1 0x1005c030
void LegoInputManager::CreateAndAcquireKeyboard(HWND hwnd)
{
  HINSTANCE hinstance = (HINSTANCE)GetWindowLongA(hwnd, GWL_HINSTANCE);
  HRESULT hresult = DirectInputCreateA(hinstance, 0x500, &m_directinputInterface, NULL); // 0x500 for DX5
  if (hresult == DI_OK)
  {
    HRESULT createdeviceresult = m_directinputInterface->CreateDevice(GUID_SysKeyboard, &m_directinputDeviceInterface, NULL);
    if (createdeviceresult == DI_OK)
    {
      m_directinputDeviceInterface->RunControlPanel(hwnd, 6);
      m_directinputDeviceInterface->SetDataFormat(&c_dfDIKeyboard);
      m_directinputDeviceInterface->Acquire();
    }
  }
  return;
}

int LegoInputManager::GetJoystickState(unsigned int* joystick_x, unsigned int* joystick_y, DWORD* buttons_state, unsigned int* pov_position)
{
  if (m_joystickIndex != JOYSTICKID1)
  {
    if (m_joyid < 0)
    {
      int result = GetJoystickId();
      if (result == -1)
      {
        m_joystickIndex = 0;
        return -1;
      }
    }

    JOYINFOEX joyinfoex;
    joyinfoex.dwSize = 0x34;
    joyinfoex.dwFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS;
    UINT capabilities = m_joyCapsA.wCaps;
    if ((capabilities & JOYCAPS_HASPOV) != 0) 
    {
      joyinfoex.dwFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNPOV | JOY_RETURNBUTTONS;
      if ((capabilities & JOYCAPS_POVCTS) != 0)
      {
          joyinfoex.dwFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNPOV | JOY_RETURNBUTTONS | JOY_RETURNPOVCTS;
      }
    }
    MMRESULT mmresult = joyGetPosEx(m_joyid, &joyinfoex);
    if (mmresult == MMSYSERR_NOERROR) {
      *buttons_state = joyinfoex.dwButtons;
      UINT xmin = m_joyCapsA.wXmin;
      UINT ymax = m_joyCapsA.wYmax;
      UINT ymin = m_joyCapsA.wYmin;
      *joystick_x = ((joyinfoex.dwXpos - xmin) * 100) / (m_joyCapsA.wXmax - xmin);
      *joystick_y = ((joyinfoex.dwYpos - m_joyCapsA.wYmin) * 100) / (ymax - ymin);
      if (m_joyCapsA.wCaps & (JOYCAPS_POV4DIR | JOYCAPS_POVCTS) == 0) {
        *pov_position = (UINT)-1;
        return 0;
      }
      if (joyinfoex.dwPOV == -1) {
        *pov_position =  (UINT)-1;
        return 0;
      }
      *pov_position = joyinfoex.dwPOV / 100;
      return 0;
    }
  }
  return -1;
}

// FIXME: there's 2 different variables for the joystick id??? also the lines that 
// set dwSize and dwFlags result in incorrect offsets in the JOYINFOEX struct

//  OFFSET: LEGO1 0x1005c240
int LegoInputManager::GetJoystickId()
{
  JOYINFOEX joyinfoex;
  if (m_joystickIndex != JOYSTICKID1)
  {
    UINT joyid = m_unknown19C;
    if (-1 < (int)joyid)
    {
      joyinfoex.dwSize = 0x34;
      joyinfoex.dwFlags = 0xFF;
      MMRESULT result = joyGetPosEx(joyid, &joyinfoex);
      if (result == JOYERR_NOERROR && (joyGetDevCapsA(joyid, &m_joyCapsA, 0x194) == JOYERR_NOERROR))
      {
        m_joyid = joyid;
        return 0;
      }
    }
    for (joyid = JOYSTICKID1; joyid < 0x10; joyid++)
    {
      joyinfoex.dwSize = 0x34;
      joyinfoex.dwFlags = 0xFF;
      MMRESULT result = joyGetPosEx(joyid, &joyinfoex);
      if (result == JOYERR_NOERROR && (joyGetDevCapsA(joyid, &m_joyCapsA, 0x194) == JOYERR_NOERROR))
      {
        m_joyid = joyid;
        return 0;
      }
    }
  }
  return -1;
}
