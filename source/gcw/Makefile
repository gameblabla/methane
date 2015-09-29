CXX = mipsel-linux-g++
STRIP = mipsel-linux-strip

OPT = -O2 -fomit-frame-pointer -fdata-sections -ffunction-sections -mips32r2
LDOPT = -Wl,--as-needed -Wl,--gc-sections -flto 

CFLAGS = $(OPT) -Wall -DMETHANE_MIKMOD -I/usr/include/SDL -I./ -I./../
CXXFLAGS = $(OPT) -Wall -DMETHANE_MIKMOD -I/usr/include/SDL -I./ -I./../
GP2X_LIBS = $(LDOPT) -lSDL -lSDLmain ./libmikmod.a -ldl


# Remove the following two lines to disable sound
METHANE_SND_FLAGS = 
METHANE_SND_LIBS =
all: message methane

# Methane brothers with sound
message:
	@echo "Compiling Super Methane Brothers."
	@echo "================================="

# Basic methane brothers compilation
methane: MainSource/gfxoff.o MainSource/baddie.o MainSource/bitdraw.o MainSource/bitgroup.o MainSource/bititem.o \
	MainSource/boss.o MainSource/game.o MainSource/gasobj.o MainSource/global.o MainSource/goodie.o MainSource/maps.o \
	MainSource/misc.o MainSource/objlist.o MainSource/player.o MainSource/power.o MainSource/target.o \
	MainSource/suck.o MainSource/weapon.o MainSource/sound.o MainData/snddata.o MainData/gfxdata.o MainData/gfxdata2.o \
	MainData/gfxdata3.o MainData/mapdata.o MainGp2x/doc.o MainGp2x/SDL_framerate.o MainMikMod/audiodrv.o
	${CXX} ${CXXFLAGS} MainSource/gfxoff.o MainSource/baddie.o MainSource/bitdraw.o MainSource/bitgroup.o MainSource/bititem.o \
	MainSource/boss.o MainSource/game.o MainSource/gasobj.o MainSource/global.o MainSource/goodie.o MainSource/maps.o \
	MainSource/misc.o MainSource/objlist.o MainSource/player.o MainSource/power.o MainSource/target.o \
	MainSource/suck.o MainSource/weapon.o MainSource/sound.o MainData/snddata.o MainData/gfxdata.o MainData/gfxdata2.o \
	MainData/gfxdata3.o MainData/mapdata.o MainGp2x/doc.o MainGp2x/SDL_framerate.o  MainMikMod/audiodrv.o \
	-o methane ${METHANE_LIBS} ${METHANE_SND_LIBS} ${GP2X_LIBS} ${LDFLAGS}
	$(STRIP) methane

clean:
	@rm -Rf MainSource
	@rm -Rf MainData
	@rm -Rf MainGp2x
	@rm -Rf MainMikMod
	@rm -f methane

distclean: clean


# The main source code
MainSource/%.o : ../%.cpp
	@echo "  Compiling $<..."
	@if [ ! -d MainSource ]; then mkdir MainSource; fi
	${CXX} ${CXXFLAGS} ${METHANE_FLAGS} ${METHANE_SND_FLAGS} -I ../linux -c $< -o $@

# The main data
MainData/%.o : ../data/%.cpp
	@echo "  Compiling $<..."
	@if [ ! -d MainData ]; then mkdir MainData; fi
	${CXX} ${CXXFLAGS} ${METHANE_FLAGS} ${METHANE_SND_FLAGS} -I ../ -c $< -o $@

# The Gp2x source code
MainGp2x/%.o : ../gcw/%.cpp
	@echo "  Compiling $<..."
	@if [ ! -d MainGp2x ]; then mkdir MainGp2x; fi
	${CXX} ${CXXFLAGS} ${METHANE_FLAGS} ${METHANE_SND_FLAGS} -I ../ -c $< -o $@

# The sound driver source code
MainMikMod/%.o : ../mikmod/%.cpp
	@echo "  Compiling $<..."
	@if [ ! -d MainMikMod ]; then mkdir MainMikMod; fi
	${CXX} ${CXXFLAGS} ${METHANE_FLAGS} ${METHANE_SND_FLAGS} -I ../ -c $< -o $@

