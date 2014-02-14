


namespace ExAutoTele
{

	void __stdcall b_TeleportNext(StormMsg * Msg);
	void __stdcall b_TeleportPrev(StormMsg * Msg);
	void __stdcall b_TeleportWP(StormMsg * Msg);

	bool TeleportTo(int nLevelId, short mX, short mY);
	bool TeleportTo(int nLevelId, int dwObjectClassId);
	bool TeleportTo(int nLevelId);
}