include clear-config.mk
CFLAGS:=-O0
SRCS:=vdec.c md5.c
LIBS:=mi_sys mi_vdec mi_divp mi_common mi_venc dl
include add-config.mk