/*==========================================================
* D2Ex2
* https://github.com/lolet/D2Ex2
* ==========================================================
* Copyright (c) 2011-2015 Bartosz Jankowski
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

#ifndef __EXFONTTABLE_H__
#define __EXFONTTABLE_H__

#include <vector>

class ExFontManager
{
public:
	ExFontManager();
	~ExFontManager();
	D2FontTable* get(const unsigned int font) const { D2EXASSERT(font > 13, "Invalid font (%d)", font)  return table[font].get(); };

private:
	std::vector<blizz_unique_ptr<D2FontTable>> table;
};

#endif