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
ExParty::Update();
Request--;
return false;
}