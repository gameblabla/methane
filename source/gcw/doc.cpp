/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 * Program WebSite: http://methane.sourceforge.net/index.html              *
 * Project Email: rombust@postmaster.co.uk                                 *
 *                                                                         *
 ***************************************************************************/

//------------------------------------------------------------------------------
// The AROS main document file
//------------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <SDL/SDL.h>
#include "SDL_framerate.h"

#include "doc.h"
#include "global.h"

#ifdef METHANE_MIKMOD
#include "../mikmod/audiodrv.h"
#endif

//Original resolution is 320x256, but we'll use 320x240
int DC_SCREEN_W = 320;
int DC_SCREEN_H = 252;

//------------------------------------------------------------------------------
// Private Functions
//------------------------------------------------------------------------------
extern void main_code(void);

//------------------------------------------------------------------------------
// The Game Instance
//------------------------------------------------------------------------------
CMethDoc Game;
FPSmanager FpsManager;
SDL_Surface * SdlScreen = 0;

static char TheScreen[SCR_WIDTH * SCR_HEIGHT];

//------------------------------------------------------------------------------
// The HighScore table filename
//------------------------------------------------------------------------------
#define HighScoreLoadBufferSize (MAX_HISCORES * 64)

//------------------------------------------------------------------------------
// The Main Function
//------------------------------------------------------------------------------

int main (int argc, char **argv)
{
    printf("Starting...\n");
	
	// init
	if (SDL_Init(SDL_INIT_VIDEO) < 0) 
	{
		fprintf (stderr, "Can't init SDL : %s", SDL_GetError());
		return 1;
	}
	atexit (SDL_Quit);

#ifdef JOYSTICKS	
	SDL_Init(SDL_INIT_JOYSTICK);
	
	//DC: Player 1 and 2 :)
	SDL_JoystickOpen (0);
	SDL_JoystickOpen (1);
#endif

    SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	
	// run
	main_code();
	// The main routine

	return (0) ;
}

//------------------------------------------------------------------------------
//! \brief The program main code
//------------------------------------------------------------------------------
void main_code(void)
{
	JOYSTICK *jptr1;
	JOYSTICK *jptr2;/*
#ifndef _TINSPIRE
	const float real_FPS = 1000/50;
#endif*/
	//Use fullscreen on Dreamcast.
	SdlScreen = SDL_SetVideoMode ( DC_SCREEN_W, DC_SCREEN_H, 8, SDL_SWSURFACE);
			
	if (!SdlScreen)
	{
		fprintf (stderr, "Couldn't set video mode : %s\n", SDL_GetError());
		return;
	}
	SDL_ShowCursor (SDL_DISABLE);

	//DC: Mantain GNU License, and modify controls desc.
	printf("The GNU General Public License V2 applies to this game.\n\n");
	printf("See: http://methane.sourceforge.net\n\n");
	printf("Instructions:\n\n");
	printf("Player controls :\n");
	printf("Press A (CTRL) to fire gas from the gun.\n");
	printf("Hold A to suck a trapped baddie into the gun.\n");
	printf("Release A to throw the trapped baddie from the gun.\n");
	printf("Direction keys to move.\n\n");
	printf("Throw baddies at the wall to destroy them.\n\n");
	printf("L (ESC) = Quit (and save high scores)\n");
	printf("B (TAB) = Change player graphic during game\n\n");

#ifdef METHANE_MIKMOD
	Game.InitSoundDriver();
#endif
	Game.InitGame ();
	Game.LoadScores();
	Game.StartGame();
		
	jptr1 = &Game.m_GameTarget.m_Joy1;
	jptr2 = &Game.m_GameTarget.m_Joy2;

	int run = 1;
	bool toogle = false; //DC: Improve change PuffBlow with the button.
	bool updateKB = true; //Dc: Insert name using Controller Pad
	int value;
	
	SDL_initFramerate (&FpsManager);
	int game_paused = 0;
	
	while(run)
	{
		
		SDL_PumpEvents();
		Uint8 * key = SDL_GetKeyState (NULL);

		if (key[SDLK_ESCAPE]) break;
		jptr1->key = 13;

		// event loop
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				//DREAMCAST: Only look at Keyboard keys :)
				//First Common keys for both players
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
					case SDLK_F1:
						if (game_paused==0) game_paused=1;
						else game_paused=0;
						break;
					case SDLK_TAB:
						if (!toogle){
							toogle=true;
							Game.m_GameTarget.m_Game.TogglePuffBlow();
						}
						break;
					//HiScore name
					
					case SDLK_PLUS:
					case SDLK_UP:
						//Get the actual char!
						value = (int)(Game.m_GameTarget.m_Game.GetActualChar());
						value++;
						//Check limits
						if (value<65) value = 65; //A (initial case)
						else if (value>90) value = 65; //Z->A
						jptr1->key = (char)(value);
						updateKB = false;
						break;
					case SDLK_MINUS:
					case SDLK_DOWN:
						//Get the actual char!
						value = (int)(Game.m_GameTarget.m_Game.GetActualChar());
						value--;
						//Check limits
						if (value<65) value = 90; //A->Z
						jptr1->key = (char)(value);
						updateKB = false;
						
					break;
					case SDLK_RIGHT:
						jptr1->right = true;
						break;
					case SDLK_LEFT:
						jptr1->left = true;
						break;
						
					/*case SDLK_UP:
						jptr1->up = true;
						break;
					case SDLK_DOWN:
						jptr1->down = true;
						break;*/
						
					case SDLK_LCTRL:
						jptr1->up = true;
						break;
					case SDLK_LALT:
						jptr1->fire = true;
						updateKB = false;
						break;
					default:
						break;
					}
					//Keyboard
					if (updateKB) jptr1->key = event.key.keysym.sym;
					else updateKB = true;
				break;
				case SDL_KEYUP:
					switch(event.key.keysym.sym)
					{
					case SDLK_TAB:
						toogle = false;
						break;
					case SDLK_RIGHT:
						jptr1->right = false;
						break;
					case SDLK_LEFT:
						jptr1->left = false;
						break;
					/*case SDLK_UP:
						jptr1->up = false;
						break;
					case SDLK_DOWN:
						jptr1->down = false;
						break;
					*/
					case SDLK_LCTRL:
						jptr1->up = false;
						break;
					case SDLK_LALT:
						jptr1->fire = false;
						break;
					default:
						break;
					}
				break;
				default:
					break;
			}
		}

		// (CHEAT MODE DISABLED) --> jptr1->next_level = 0;
		Game.MainLoop (0,game_paused);
		SDL_framerateDelay (&FpsManager);
	}
	//Stop music!
	Game.StopModule();
#ifdef METHANE_MIKMOD
	Game.RemoveSoundDriver();
#endif	
	Game.SaveScores ();

	SDL_FreeSurface (SdlScreen);
}

//------------------------------------------------------------------------------
//! \brief Initialise Document
//------------------------------------------------------------------------------
CMethDoc::CMethDoc()
{
#ifdef METHANE_MIKMOD
	SMB_NEW(m_pMikModDrv,CMikModDrv);
#endif
	m_GameTarget.Init(this);
}

//------------------------------------------------------------------------------
//! \brief Destroy Document
//------------------------------------------------------------------------------
CMethDoc::~CMethDoc()
{
#ifdef METHANE_MIKMOD
	if (m_pMikModDrv)
	{
		delete(m_pMikModDrv);
		m_pMikModDrv = 0;
	}
#endif
}

//------------------------------------------------------------------------------
//! \brief Initialise the game
//------------------------------------------------------------------------------
void CMethDoc::InitGame(void)
{
	m_GameTarget.InitGame (TheScreen);
#ifdef METHANE_MIKMOD
	m_GameTarget.PrepareSoundDriver ();
#endif
}

//------------------------------------------------------------------------------
//! \brief Initialise the sound driver
//------------------------------------------------------------------------------
void CMethDoc::InitSoundDriver(void)
{
#ifdef METHANE_MIKMOD
	m_pMikModDrv->InitDriver();
#endif
}

//------------------------------------------------------------------------------
//! \brief Remove the sound driver
//------------------------------------------------------------------------------
void CMethDoc::RemoveSoundDriver(void)
{
#ifdef METHANE_MIKMOD
	m_pMikModDrv->RemoveDriver();
#endif
}

//------------------------------------------------------------------------------
//! \brief Start the game
//------------------------------------------------------------------------------
void CMethDoc::StartGame(void)
{
	m_GameTarget.StartGame();
}

//------------------------------------------------------------------------------
//! \brief Redraw the game main view
//!
//! 	\param pal_change_flag : 0 = Palette not changed
//------------------------------------------------------------------------------
void CMethDoc::RedrawMainView( int pal_change_flag )
{
	// Function not used
}

//------------------------------------------------------------------------------
//! \brief Draw the screen
//!
//! 	\param screen_ptr = UNUSED
//------------------------------------------------------------------------------
void CMethDoc::DrawScreen( void *screen_ptr )
{
	METHANE_RGB *pptr;
	int cnt;
	SDL_Color colors [PALETTE_SIZE];

	if (SDL_MUSTLOCK (SdlScreen))
	{
		SDL_LockSurface (SdlScreen);
	}

	// Set the game palette
	pptr = m_GameTarget.m_rgbPalette;
	for (cnt=0; cnt < PALETTE_SIZE; cnt++, pptr++)
	{
		colors[cnt].r = pptr->red;
		colors[cnt].g = pptr->green;
		colors[cnt].b = pptr->blue;
	}
	SDL_SetPalette (SdlScreen, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, PALETTE_SIZE);

	// Copy the pixels
	// No Scaling here (as GP2X version).
	char * dptr = (char *) SdlScreen->pixels;
	char * sptr = TheScreen + 4 * DC_SCREEN_W;
	for (int y = 0; y < DC_SCREEN_H; y++) {
		for (int x = 0; x < DC_SCREEN_W; x++) {
			*dptr++ = *sptr++;
		}
		dptr += (SdlScreen->pitch - DC_SCREEN_W);
	}

	if (SDL_MUSTLOCK (SdlScreen))
	{
		SDL_UnlockSurface (SdlScreen);
	}

	// Show the new screen
	SDL_Flip (SdlScreen);
}

//------------------------------------------------------------------------------
//! \brief The Game Main Loop
//!
//! 	\param screen_ptr = UNUSED
//------------------------------------------------------------------------------
void CMethDoc::MainLoop( void *screen_ptr, int paused_flag )
{
	if (!paused_flag)
	{
		m_GameTarget.MainLoop();
	}
	DrawScreen( screen_ptr );
#ifdef METHANE_MIKMOD
	m_pMikModDrv->Update();
#endif
}

//------------------------------------------------------------------------------
//! \brief Play a sample (called from the game)
//!
//! 	\param id = SND_xxx id
//!	\param pos = Sample Position to use 0 to 255
//!	\param rate = The rate
//------------------------------------------------------------------------------
void CMethDoc::PlaySample(int id, int pos, int rate)
{
#ifdef METHANE_MIKMOD
	m_pMikModDrv->PlaySample(id, pos, rate);
#endif
}

//------------------------------------------------------------------------------
//! \brief Stop the module (called from the game)
//------------------------------------------------------------------------------
void CMethDoc::StopModule(void)
{
#ifdef METHANE_MIKMOD
	m_pMikModDrv->StopModule();
#endif
}

//------------------------------------------------------------------------------
//! \brief Play a module (called from the game)
//!
//! 	\param id = SMOD_xxx id
//------------------------------------------------------------------------------
void CMethDoc::PlayModule(int id)
{
#ifdef METHANE_MIKMOD
	m_pMikModDrv->PlayModule(id);
#endif
}

//------------------------------------------------------------------------------
//! \brief Update the current module (ie restart the module if it has stopped) (called from the game)
//!
//! 	\param id = SMOD_xxx id (The module that should be playing)
//------------------------------------------------------------------------------
void CMethDoc::UpdateModule(int id)
{
#ifdef METHANE_MIKMOD
	m_pMikModDrv->UpdateModule(id);
#endif
}

//------------------------------------------------------------------------------
//! \brief Load the high scores
//------------------------------------------------------------------------------
void CMethDoc::LoadScores(void)
{
	FILE *fptr;
	char *mptr;
	char *tptr;
	char let;
	int cnt;
	
	char buf[128];
	
#ifdef _TINSPIRE
	snprintf(buf, sizeof(buf), "./%s/%s", MAIN_SAVE_DIRECTORY, "methane_highscore.sav");
#else
	snprintf(buf, sizeof(buf), "%s/.methane/%s", MAIN_SAVE_DIRECTORY, "methane_highscore.sav");
#endif
	
	fptr = fopen(buf, "r");
	
	if (!fptr) return;	// No scores available

	// Allocate file memory, which is cleared to zero
	mptr = (char *) calloc(1, HighScoreLoadBufferSize);
	if (!mptr)		// No memory
	{
		fclose(fptr);
		return;
	}
	fread( mptr, 1, HighScoreLoadBufferSize-2, fptr);	// Get the file

	// (Note: mptr is zero terminated)
	tptr = mptr;
	for (cnt=0; cnt<MAX_HISCORES; cnt++)	// For each highscore
	{

		if (!tptr[0]) break;

		m_GameTarget.m_Game.InsertHiScore( atoi(&tptr[4]), tptr );

		do	// Find next name
		{
			let = *(tptr++);
		}while (!( (let=='$') || (!let) ));
		if (!let) break;	// Unexpected EOF
	}

	free(mptr);

	fclose(fptr);

}

//------------------------------------------------------------------------------
//! \brief Save the high scores
//------------------------------------------------------------------------------
void CMethDoc::SaveScores(void)
{
	FILE *fptr;
	int cnt;
	HISCORES *hs;

	char buf[128];

#ifdef _TINSPIRE
	snprintf(buf, sizeof(buf), "./%s/%s", MAIN_SAVE_DIRECTORY, "methane_highscore.sav");
#else	
	char buf2[128];
	snprintf(buf2, sizeof(buf2), "%s/.methane", MAIN_SAVE_DIRECTORY);
	mkdir(buf2, 0755);
	snprintf(buf, sizeof(buf), "%s/.methane/%s", MAIN_SAVE_DIRECTORY, "methane_highscore.sav");
#endif
	
	fptr = fopen(buf, "w");

	if (!fptr) return;	// Cannot write scores
	for (cnt=0, hs=m_GameTarget.m_Game.m_HiScores; cnt<MAX_HISCORES; cnt++, hs++)
	{
		fprintf(fptr, "%c%c%c%c%d$", hs->name[0], hs->name[1], hs->name[2], hs->name[3], hs->score);
	}
	fclose(fptr);
}
