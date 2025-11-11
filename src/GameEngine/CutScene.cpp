#include "r3dPCH.h"
#include "CutScene.h"
#if USE_BINK_MOVIE
CutScene::CutScene()
{
	Bink = 0;
	Bink_buffer = 0;
	binkPlayed = 0;
	window = 0;
}

CutScene::~CutScene()
{
	Bink = 0;
	Bink_buffer = 0;
	binkPlayed = 0;
	window = 0;
}

void CutScene::Initialize(HWND hWnd , const char* movieName)
{
	//
	// Tell Bink to use DirectSound (must be before BinkOpen)!
	//

	BinkSoundUseDirectSound( 0 );

	//
	// Try to open the Bink file.
	//

	Bink = BinkOpen( movieName, 0 );

	if ( !Bink )
	{
		MessageBox( hWnd,
			BinkGetError( ),
			"Bink Error",
			MB_OK | MB_ICONSTOP );

		r3dOutToLog("BinkGetError Filename: %s\n", movieName);
	}

	//
	// Try to open the Bink buffer.
	//

	Bink_buffer = BinkBufferOpen( hWnd, Bink->Width, Bink->Height, 0 );
	if ( !Bink_buffer )
	{
		MessageBox( hWnd,
			BinkBufferGetError( ),
			"Bink Error",
			MB_OK | MB_ICONSTOP );

		r3dOutToLog("BinkBufferGetError\n");
		BinkClose( Bink );
	}

	window = hWnd;

	return;
}

bool CutScene::playMovie()
{
	for ( ; ; )
	{
		if(binkPlayed == 0)
		{
			//
			// Is it time for a new Bink frame?
			//

			if ( !BinkWait( Bink ) )
			{
				//
				// Yup, draw the next frame.
				//
				//r3dOutToLog("Bink wait\n");
				Show_next_frame( Bink,
					Bink_buffer,
					window );
			}
			else
			{
				//
				// Nope, give the rest of the system a chance to run (1 ms).
				//
				//r3dOutToLog("Bink sleep\n");
				Sleep( 1 );
			}
		}
		else
			break;
	}

	//
	// Close the Bink file.
	//

	if ( Bink )
	{
		BinkClose( Bink );
		Bink = 0;
	}

	//
	// Close the Bink buffer.
	//

	if ( Bink_buffer )
	{
		BinkBufferClose( Bink_buffer );
		Bink_buffer = 0;
	}

	return true;
}

void CutScene::Show_next_frame(HBINK bink, HBINKBUFFER bink_buffer, HWND window)
{
	//
	// Decompress the Bink frame.
	//

	BinkDoFrame( bink );

	//
	// Lock the BinkBuffer so that we can copy the decompressed frame into it.
	//

	if ( BinkBufferLock( bink_buffer ) )
	{
		//
		// Copy the decompressed frame into the BinkBuffer (this might be on-screen).
		//

		BinkCopyToBuffer( bink,
			bink_buffer->Buffer,
			bink_buffer->BufferPitch,
			bink_buffer->Height,
			0,0,
			bink_buffer->SurfaceType );

		//
		// Unlock the BinkBuffer.
		//

		BinkBufferUnlock( bink_buffer );
	}

	//
	// Tell the BinkBuffer to blit the pixels onto the screen (if the
	//   BinkBuffer is using an off-screen blitting style).
	//

	BinkBufferBlit( bink_buffer,
		bink->FrameRects,
		BinkGetRects( bink, bink_buffer->SurfaceType ) );

	//
	// Are we at the end of the movie?
	//

	if ( bink->FrameNum == bink->Frames )
	{
		//
		// Yup, close the window.
		//

		binkPlayed = 1;
	}
	else
	{
		//
		// Nope, advance to the next frame.
		//

		BinkNextFrame( bink );
	}
}
#endif