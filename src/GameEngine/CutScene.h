#define USE_BINK_MOVIE 0
#if USE_BINK_MOVIE
#include "r3dPCH.h"
#include "r3d.h"
#include "../External/BinkW32/bink.h"

class CutScene
{
public:
	CutScene();
	~CutScene();
	
	void Initialize(HWND hWnd , const char* movieName);

	bool playMovie();

private:
	void Show_next_frame(HBINK bink, HBINKBUFFER bink_buffer, HWND window);

private:
	HWND window;
	HBINK Bink;
	HBINKBUFFER Bink_buffer;
	int binkPlayed;
};
#endif