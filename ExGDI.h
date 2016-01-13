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

#ifndef __EXGDI_H__
#define __EXGDI_H__


namespace ExGDI {
	extern fnRendererCallbacks gfnRenderFuncs;

	BOOL __fastcall initWindow(HINSTANCE); // 0
	BOOL __fastcall initPerspective(GFXSettings* pgfxSettings, GFXHelpers* pfnHelpers); // 1
	BOOL __fastcall release(); //2
	BOOL __fastcall init(HANDLE HWND, int nResolution); // 3
	BOOL __fastcall shutdown(); // 4
	BOOL __fastcall resizeWindow(HANDLE HWND, int nMode);
}

#endif