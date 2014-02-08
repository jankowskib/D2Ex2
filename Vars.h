class ExCellFile;
class ExControl;
class ExLagometer;
class ExEditBox;

#include <vector>
#include <deque>
#include <sstream>
#include <string>

using namespace std;

#ifdef __DEFINE_VARS

#define GLOBAL(type, name) type name;

#else

#define GLOBAL(type, name) extern type name;

#endif
GLOBAL(int, gLocaleId)
GLOBAL(HANDLE, hPointersReadyEvent)
GLOBAL(BOOL, g_FastTP)
GLOBAL(DWORD, AimTarget)
GLOBAL(deque<COORDS>, HistoryPos) // Last 2 Coords
GLOBAL(vector<COORDS>, TelePath)

GLOBAL(int, bLagometer)
GLOBAL(ExEditBox*,  GoldBox)
GLOBAL(ExLagometer*, lagometer)
GLOBAL(int, Port)
GLOBAL(ExCellFile*, CellBox)
GLOBAL(ExCellFile*, CellButton)
GLOBAL(DWORD, HideCrap)
GLOBAL(DWORD, HideGold)
GLOBAL(DWORD, Maphack)
GLOBAL(DWORD, FullVisibility)
GLOBAL(DWORD, AutoShowMap)
GLOBAL(DWORD, PermShowLife)
GLOBAL(DWORD, PermShowMana)
GLOBAL(DWORD, TickAtJoin)
GLOBAL(DWORD, PVMStuff)
GLOBAL(int, ExpAtJoin)
GLOBAL(BOOL, (__fastcall *cb01)(BYTE* aPacket))
GLOBAL(BOOL, (__fastcall *cb02)(BYTE* aPacket))
GLOBAL(vector<ExControl*>,Controls)
GLOBAL(vector<ItemConfig>,ItemArray)
GLOBAL(WNDPROC,OldWNDPROC)
GLOBAL(bool, DontEnterCS)
GLOBAL(bool, DontLeaveCS)
GLOBAL(CRITICAL_SECTION, CON_CRITSECT)
GLOBAL(CRITICAL_SECTION, EX_CRITSECT)
GLOBAL(CRITICAL_SECTION, MEM_CRITSECT)
GLOBAL(CRITICAL_SECTION, BUFF_CRITSECT)
GLOBAL(CRITICAL_SECTION, TELE_CRITSECT)
GLOBAL(int, Request)
GLOBAL(int, BOLvl)
GLOBAL(int, BCLvl)
GLOBAL(int, ShoutLvl)
GLOBAL(int, EnchLvl)
GLOBAL(int, AmpLvl)
GLOBAL(int, LRLvl)
GLOBAL(int, SMLvl)
GLOBAL(int, BuffsEnabled)
GLOBAL(string, ConfigIni)
GLOBAL(string, D2ExDir)
GLOBAL(string, ClansIni)
GLOBAL(void*, MPQHandle)
//GLOBAL(ExMsgs *, aChat)
GLOBAL(int, BlobType)
GLOBAL(int, EnteringFont)
GLOBAL(deque<string>, BNQuene)

GLOBAL(short, cResModeX)
GLOBAL(short, cResModeY)
GLOBAL(int, gRenderer)

#undef GLOBAL
#undef __DEFINE_VARS