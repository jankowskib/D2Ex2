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

#ifndef __BLIZZ_SMART_PTR_H__
#define __BLIZZ_SMART_PTR_H__
#include <memory>

template<class T>
struct __blizz_deleter__
{
	void operator()(T* ptr)
	{
		D2Funcs.FOG_FreeMemory(ptr, __FILE__, __LINE__, NULL);
	}
};

/*
	Do not use for global pointers, or take care to free them manually before exit!
*/
template<typename T>
using blizz_unique_ptr = std::unique_ptr <T, __blizz_deleter__<T> >;

#endif