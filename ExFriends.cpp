#include "stdafx.h"
#include "ExFriends.h"
#include "ExParty.h"
#include "Vars.h"
#include "Misc.h"

bool ExFriends::OnFriendMsg(wstring aText)
{
return true;
}

bool ExFriends::OnWhoisRequest(wstring aText)
{
//Nie mozna bylo odnalezc uzytkownika
//Bzik*Bzik is using Diablo II Lord of Destruction and is currently in private game "Emce".
//Uzytkownik ostatni raz byl widziany : Sun Feb 07 19:47:19
//REAL BN:
//testosdas (*tt) is using Diablo II Lord of Destruction in a private game.
#ifdef D2EX_CLOSED_BNET
if(BNQuene.size() == 0) return true;

if(aText.find(L"That user is not logged on.")!=wstring::npos ||
   aText.find(L"That character is not logged on.")!=wstring::npos) { 
	   EnterCriticalSection(&EX_CRITSECT);
	   string aName = BNQuene.front().substr(7);
	   BNQuene.pop_front();
	   ExParty::AddAccount(aName,L"N/A");
	   ExParty::Update();
	   LeaveCriticalSection(&EX_CRITSECT);
	   return false;
}
wstring::size_type Tok = aText.find_first_of(L"*");
if(Tok == wstring::npos) return false; 
wstring::size_type Tok2 = aText.find_first_of(L")",Tok);
if(Tok == wstring::npos) return false; 
wstring Char = aText.substr(0,Tok-2);
wstring Acc = aText.substr(Tok+1,Tok2-Tok-1);
string szChar;
Misc::WideToChar(szChar,Char);
ExParty::AddAccount(szChar,Acc);
EnterCriticalSection(&EX_CRITSECT);
BNQuene.pop_front();
LeaveCriticalSection(&EX_CRITSECT);
ExParty::Update();
#else
if(Request<0) Request = 0;
if(!Request) return true;

if(aText.find(L"Nie mozna bylo odnalezc uzytkownika")!=wstring::npos || 
   aText.find(L"Unknown user.")!=wstring::npos || 
   aText.find(L"User was last seen on:")!=wstring::npos || 
   aText.find(L"Uzytkownik ostatni raz byl widziany")!=wstring::npos){ Request--; return false; }
wstring::size_type Tok = aText.find_first_of(L"*");
if(Tok == wstring::npos) return false; 
wstring::size_type Tok2 = aText.find_first_of(L" ",Tok);
if(Tok == wstring::npos) return false; 
wstring Char = aText.substr(0,Tok);
wstring Acc = aText.substr(Tok+1,Tok2-Tok-1);
string szChar, szAcc;
Misc::WideToChar(szAcc,Acc);
Misc::WideToChar(szChar,Char);
ExParty::AddAccount(szChar,szAcc);
Request--;
ExParty::Update();
#endif

return false;
}