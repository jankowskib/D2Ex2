/*==========================================================
* D2Ex2
* https://github.com/lolet/D2Ex2
* ==========================================================
* Copyright (c) 2011-2014 Bartosz Jankowski
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
* ==========================================================
*/

class ExCellFile;
class ExControl;
class ExLagometer;
class ExEditBox;
class ExFontManager;

#include <vector>
#include <deque>
#include <sstream>
#include <string>
#include <atomic>

using namespace std;

#ifdef __DEFINE_VARS

#define GLOBAL(type, name) type name;

#else

#define GLOBAL(type, name) extern type name;

#endif

GLOBAL(ExFontManager*, gFontManager)
GLOBAL(ExControlManager*, gExGUI)

GLOBAL(BYTE, gVerCode)

GLOBAL(BYTE, gTestColorRed)
GLOBAL(BYTE, gTestColorGreen)
GLOBAL(BYTE, gTestColorBlue)

GLOBAL(int, gLocaleId)
GLOBAL(HANDLE, hPointersReadyEvent)
GLOBAL(HANDLE, hAutoTeleEvent)

GLOBAL(DWORD, gAutoTeleAction)
GLOBAL(DWORD, gAutoTeleArgument)
GLOBAL(atomic<bool>, gStopTeleport)
GLOBAL(atomic<bool>, gFastTP)
GLOBAL(atomic<bool>, gSpecing)
GLOBAL(atomic<bool>, gControl)
GLOBAL(atomic<int>, gBNCSRequests)
GLOBAL(atomic<int>, gBNCSResponseTick)
GLOBAL(string, gszSpectator)
GLOBAL(DWORD, gSpectatorTarget)

GLOBAL(DWORD, AimTarget)
GLOBAL(deque<COORDS>, HistoryPos) // Last 2 Coords
GLOBAL(vector<COORDS>, TelePath)
GLOBAL(vector<BYTE>, BlockedPackets)

GLOBAL(int, bLagometer)
GLOBAL(exId,  GoldBox)
GLOBAL(exId, lagometer)
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
GLOBAL(CRITICAL_SECTION, EX_CRITSECT)
GLOBAL(CRITICAL_SECTION, MEM_CRITSECT)
GLOBAL(CRITICAL_SECTION, TELE_CRITSECT)
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

GLOBAL(short, VK_ATNext)
GLOBAL(short, VK_ATWP)
GLOBAL(short, VK_ATPrev)
GLOBAL(short, VK_FastTP)

GLOBAL(int, gRespawnTime)
GLOBAL(bool, gDisableSpectator)

#undef GLOBAL
#undef __DEFINE_VARS