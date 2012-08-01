SUBDIRS = audio/GSM0610 \
          audio/Speex \
          audio/iLBC \
	  audio/LPC_10 \
	  audio/G726 \
	  audio/IMA_ADPCM \
	  audio/gsm-amr \
	  video/H.261-vic

HAS_H263=1
ifeq ($(HAS_H263),1)
SUBDIRS += video/H.263-ffmpeg
endif

all debug opt both \
optshared debugshared bothshared \
optnoshared debugnoshared bothnoshared:
	@set -e; $(foreach dir,$(SUBDIRS),if test -d ${dir} ; then $(MAKE) -C $(dir); fi ; )

clean optclean debugclean:
	@set -e; $(foreach dir,$(SUBDIRS),if test -d ${dir} ; then $(MAKE) -C $(dir) clean; fi ; )

install uninstall:
	@set -e; $(foreach dir,$(SUBDIRS),if test -d ${dir} ; then $(MAKE) -C $(dir) $@; fi ; )

optdepend debugdepend bothdepend release tagbuild:
	@echo
