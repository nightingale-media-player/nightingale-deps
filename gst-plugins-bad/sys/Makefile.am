# if USE_DXR3
# DXR3_DIR=dxr3
# else
# DXR3_DIR=
# endif

# if USE_QCAM
# QCAM_DIR=qcam
# else
# QCAM_DIR=
# endif

if USE_VCD
VCD_DIR=vcd
else
VCD_DIR=
endif

# if USE_CDROM
#  CDROM_DIR=cdrom
# else
# CDROM_DIR=
# endif

if USE_FBDEV
FBDEV_DIR=fbdev
else
FBDEV_DIR=
endif

if USE_DVB
DVB_DIR=dvb
else
DVB_DIR=
endif

if USE_OSS4
OSS4_DIR=oss4
else
OSS4_DIR=
endif

if USE_QUICKTIME
QT_DIR=qtwrapper
else
QT_DIR=
endif

if USE_DIRECTSHOW
DSHOW_DIRS=dshowdecwrapper dshowvideosink
else
DSHOW_DIRS=
endif

if USE_WININET
WININET_DIR=wininet
else
WININET_DIR=
endif

if USE_ACM
ACM_DIRS=acmenc acmmp3dec
else
ACM_DIRS=
endif

if USE_DMO
DMO_DIR=dmoenc
else
DMO_DIR=
endif

SUBDIRS = $(ACM_DIRS) $(DMO_DIR) $(DSHOW_DIRS) $(DVB_DIR) $(FBDEV_DIR) $(OSS4_DIR) $(QT_DIR) $(VCD_DIR) $(WININET_DIR)

DIST_SUBDIRS = acmenc acmmp3dec dvb fbdev dmoenc dshowdecwrapper \
               dshowsrcwrapper dshowvideosink \
               oss4 qtwrapper vcd wasapi wininet winks winscreencap

