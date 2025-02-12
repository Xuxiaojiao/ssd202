integrate;
vifinit 1920 1080 0;
vpeinit 0 1920 1080 0x18;
setbind2 6 0 0 0 30 7 0 0 0 30 0x4 0;
#[2] :PORT
vpecreateport 0 7 1920 1080 11;
q;

#i2
region;
init;
#PORT 是倒数第四个参数
injectpic testFile/I2/304X200.i2 0 304 200 1 2 0 0 7 0 0 0;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;
region;
injectpic testFile/I2/200X200.i2 1 200 200 1 2 0 0 7 112 300 1;
injectpic testFile/I2/200X200.i2 2 200 200 1 2 0 0 7 300 112 2;
injectpic testFile/I2/200X200.i2 3 200 200 1 2 0 0 7 112 456 3;
injectpic testFile/I2/200X200.i2 4 200 200 1 2 0 0 7 456 112 4;
injectpic testFile/I2/200X200.i2 5 200 200 1 2 0 0 7 376 376 5;
injectpic testFile/I2/200X200.i2 6 200 200 1 2 0 0 7 832 112 6;
injectpic testFile/I2/200X200.i2 7 200 200 1 2 0 0 7 1080 563 7;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;
region;
#PORT 是第五个参数
setosddisplayattr 1 0 0 0 7 1 176 90 1 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 2 0 0 0 7 1 352 181 2 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 3 0 0 0 7 1 524 270 3 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 4 0 0 0 7 1 700 324 4 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 5 0 0 0 7 1 828 413 5 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 6 0 0 0 7 1 960 502 6 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 7 0 0 0 7 1 1080 593 7 0 0 0 0 0 0 0 0xFF;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 3 0 3;
q;

#i4
region;
destroy 0;
destroy 1;
destroy 2;
destroy 3;
destroy 4;
destroy 5;
destroy 6;
destroy 7;
#PORT 是倒数第四个参数
injectpic testFile/I4/304X200.i4 0 304 200 1 3 0 0 7 0 0 0;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;
region;
injectpic testFile/I4/200X200.i4 1 200 200 1 3 0 0 7 112 300 1;
injectpic testFile/I4/200X200.i4 2 200 200 1 3 0 0 7 300 112 2;
injectpic testFile/I4/200X200.i4 3 200 200 1 3 0 0 7 112 456 3;
injectpic testFile/I4/200X200.i4 4 200 200 1 3 0 0 7 456 112 4;
injectpic testFile/I4/200X200.i4 5 200 200 1 3 0 0 7 376 376 5;
injectpic testFile/I4/200X200.i4 6 200 200 1 3 0 0 7 832 112 6;
injectpic testFile/I4/200X200.i4 7 200 200 1 3 0 0 7 1080 563 7;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;
region;
#PORT 是第五个参数
setosddisplayattr 1 0 0 0 7 1 176 90 1 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 2 0 0 0 7 1 352 181 2 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 3 0 0 0 7 1 524 270 3 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 4 0 0 0 7 1 700 324 4 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 5 0 0 0 7 1 828 413 5 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 6 0 0 0 7 1 960 502 6 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 7 0 0 0 7 1 1080 593 7 0 0 0 0 0 0 0 0xFF;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;

#1555
region;
destroy 0;
destroy 1;
destroy 2;
destroy 3;
destroy 4;
destroy 5;
destroy 6;
destroy 7;
#PORT 是倒数第四个参数
injectpic testFile/1555/300X224.argb1555 0 300 224 1 0 0 0 7 0 0 0;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;
region;
injectpic testFile/1555/200X356.argb1555 1 200 356 1 0 0 0 7 725 0 1;
injectpic testFile/1555/200X356.argb1555 2 200 356 1 0 0 0 7 1450 0 2;
injectpic testFile/1555/200X356.argb1555 3 200 356 1 0 0 0 7 0 545 3;
injectpic testFile/1555/200X356.argb1555 4 200 356 1 0 0 0 7 725 375 4;
injectpic testFile/1555/200X356.argb1555 5 200 356 1 0 0 0 7 1450 375 5;
injectpic testFile/1555/200X356.argb1555 6 200 356 1 0 0 0 7 0 750 6;
injectpic testFile/1555/200X356.argb1555 7 200 356 1 0 0 0 7 735 750 7;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;
region;
#PORT 是第五个参数
setosddisplayattr 1 0 0 0 7 1 349 180 1 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 2 0 0 0 7 1 698 360 2 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 3 0 0 0 7 1 1050 540 3 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 4 0 0 0 7 1 55 375 4 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 5 0 0 0 7 1 404 615 5 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 6 0 0 0 7 1 1450 580 6 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 7 0 0 0 7 1 1650 780 7 0 0 0 0 0 0 0 0xFF;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;

#4444
region;
destroy 0;
destroy 1;
destroy 2;
destroy 3;
destroy 4;
destroy 5;
destroy 6;
destroy 7;
#PORT 是倒数第四个参数
injectpic testFile/4444/300X224.argb4444 0 300 224 1 1 0 0 7 0 0 0;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p3.yuv 7 0 3 0 3;
q;
region;
injectpic testFile/4444/200X356.argb4444 1 200 356 1 1 0 0 7 725 0 1;
injectpic testFile/4444/200X356.argb4444 2 200 356 1 1 0 0 7 1450 0 2;
injectpic testFile/4444/200X356.argb4444 3 200 356 1 1 0 0 7 0 545 3;
injectpic testFile/4444/200X356.argb4444 4 200 356 1 1 0 0 7 725 375 4;
injectpic testFile/4444/200X356.argb4444 5 200 356 1 1 0 0 7 1450 375 5;
injectpic testFile/4444/200X356.argb4444 6 200 356 1 1 0 0 7 0 750 6;
injectpic testFile/4444/200X356.argb4444 7 200 356 1 1 0 0 7 735 750 7;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;
region;
#PORT 是第五个参数
setosddisplayattr 1 0 0 0 7 1 349 180 1 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 2 0 0 0 7 1 698 360 2 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 3 0 0 0 7 1 1050 540 3 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 4 0 0 0 7 1 55 375 4 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 5 0 0 0 7 1 404 615 5 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 6 0 0 0 7 1 1450 580 6 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 7 0 0 0 7 1 1650 780 7 0 0 0 0 0 0 0 0xFF;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;

#8888
#region;
#destroy 0;
#destroy 1;
#destroy 2;
#destroy 3;
#destroy 4;
#destroy 5;
#destroy 6;
#destroy 7;
##PORT 是倒数第四个参数
#injectpic testFile/8888/300X224.argb8888 0 300 224 1 6 0 0 7 0 0 0;
#q;
#integrate;
#yuvwritefile ./data_1920x1080_vpe_p3.yuv 7 0 7 0 3;
#q;
#region;
#injectpic testFile/8888/200X356.argb8888 1 200 356 1 6 0 0 7 725 0 1;
#injectpic testFile/8888/200X356.argb8888 2 200 356 1 6 0 0 7 1450 0 2;
#injectpic testFile/8888/200X356.argb8888 3 200 356 1 6 0 0 7 0 545 3;
#injectpic testFile/8888/200X356.argb8888 4 200 356 1 6 0 0 7 725 375 4;
#injectpic testFile/8888/200X356.argb8888 5 200 356 1 6 0 0 7 1450 375 5;
#injectpic testFile/8888/200X356.argb8888 6 200 356 1 6 0 0 7 0 750 6;
#injectpic testFile/8888/200X356.argb8888 7 200 356 1 6 0 0 7 735 750 7;
#q;
#integrate;
#yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
#q;
#region;
##PORT 是第五个参数
#setosddisplayattr 1 0 0 0 7 1 349 180 1 0 0 0 0 0 0 0 0xFF;
#setosddisplayattr 2 0 0 0 7 1 698 360 2 0 0 0 0 0 0 0 0xFF;
#setosddisplayattr 3 0 0 0 7 1 1050 540 3 0 0 0 0 0 0 0 0xFF;
#setosddisplayattr 4 0 0 0 7 1 55 375 4 0 0 0 0 0 0 0 0xFF;
#setosddisplayattr 5 0 0 0 7 1 404 615 5 0 0 0 0 0 0 0 0xFF;
#setosddisplayattr 6 0 0 0 7 1 1450 580 6 0 0 0 0 0 0 0 0xFF;
#setosddisplayattr 7 0 0 0 7 1 1650 780 7 0 0 0 0 0 0 0 0xFF;
#q;
#integrate;
#yuvwritefile ./ data_1920x1080_vpe_p7.yuv 1 0 0 3;
#q;

#565
#region;
#destroy 0;
#destroy 1;
#destroy 2;
#destroy 3;
#destroy 4;
#destroy 5;
#destroy 6;
#destroy 7;
##PORT 是倒数第四个参数
#injectpic testFile/565/300X224.rgb565 0 300 224 1 5 0 0 7 0 0 0;
#q;
#integrate;
#yuvwritefile ./data_1920x1080_vpe_p3.yuv 7 0 7 0 3;
#q;
#region;
#injectpic testFile/565/200X356.rgb565 1 200 356 1 5 0 0 7 725 0 1;
#injectpic testFile/565/200X356.rgb565 2 200 356 1 5 0 0 7 1450 0 2;
#injectpic testFile/565/200X356.rgb565 3 200 356 1 5 0 0 7 0 545 3;
#injectpic testFile/565/200X356.rgb565 4 200 356 1 5 0 0 7 725 375 4;
#injectpic testFile/565/200X356.rgb565 5 200 356 1 5 0 0 7 1450 375 5;
#injectpic testFile/565/200X356.rgb565 6 200 356 1 5 0 0 7 0 750 6;
#injectpic testFile/565/200X356.rgb565 7 200 356 1 5 0 0 7 735 750 7;
#q;
#integrate;
#yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
#q;
#region;
##PORT 是第五个参数
#setosddisplayattr 1 0 0 0 7 1 349 180 1 0 0 0 0 0 0 0 0xFF;
#setosddisplayattr 2 0 0 0 7 1 698 360 2 0 0 0 0 0 0 0 0xFF;
#setosddisplayattr 3 0 0 0 7 1 1050 540 3 0 0 0 0 0 0 0 0xFF;
#setosddisplayattr 2 0 0 0 7 1 55 375 4 0 0 0 0 0 0 0 0xFF;
#setosddisplayattr 1 0 0 0 7 1 404 615 5 0 0 0 0 0 0 0 0xFF;
#setosddisplayattr 2 0 0 0 7 1 1450 580 6 0 0 0 0 0 0 0 0xFF;
#setosddisplayattr 3 0 0 0 7 1 1650 780 7 0 0 0 0 0 0 0 0xFF;
#q;
#integrate;
#yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
#q;

#i8
region;
destroy 0;
destroy 1;
destroy 2;
destroy 3;
destroy 4;
destroy 5;
destroy 6;
destroy 7;
#PORT 是倒数第四个参数
injectpic testFile/I8/304X200.i8 0 304 200 1 4 0 0 7 0 0 0;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;
region;
injectpic testFile/I8/200X200.i8 1 200 200 1 4 0 0 7 112 300 1;
injectpic testFile/I8/200X200.i8 2 200 200 1 4 0 0 7 300 112 2;
injectpic testFile/I8/200X200.i8 3 200 200 1 4 0 0 7 112 456 3;
injectpic testFile/I8/200X200.i8 4 200 200 1 4 0 0 7 456 112 4;
injectpic testFile/I8/200X200.i8 5 200 200 1 4 0 0 7 376 376 5;
injectpic testFile/I8/200X200.i8 6 200 200 1 4 0 0 7 832 112 6;
injectpic testFile/I8/200X200.i8 7 200 200 1 4 0 0 7 1080 563 7;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;
region;
#PORT 是第五个参数
setosddisplayattr 1 0 0 0 7 1 176 90 1 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 2 0 0 0 7 1 352 181 2 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 3 0 0 0 7 1 524 270 3 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 4 0 0 0 7 1 700 324 4 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 5 0 0 0 7 1 828 413 5 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 6 0 0 0 7 1 960 502 6 0 0 0 0 0 0 0 0xFF;
setosddisplayattr 7 0 0 0 7 1 1080 593 7 0 0 0 0 0 0 0 0xFF;
q;
integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;

#COVER
region;
destroy 0;
destroy 1;
destroy 2;
destroy 3;
destroy 4;
destroy 5;
destroy 6;
destroy 7;
#PORT 是第六个参数
injectcover 0 2000 2000 0 0 7 1000 1000 0 0xFF;
q;

integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
q;

region;
injectcover 1 2500 2500 0 0 7 1500 1500 1 0xFF00;
injectcover 2 3000 1000 0 0 7 2000 2000 2 0xFF0000;
injectcover 3 1000 2000 0 0 7 2500 2500 3 0x0;
q;

integrate;
yuvwritefile ./data_1920x1080_vpe_p7.yuv 7 0 7 0 3;
#exportfile ./data.es /dev/mtd2;
q;

region;
destroy 0;
destroy 1;
destroy 2;
destroy 3;
deinit;
q;

integrate;
vpedestroyport 7 0;
setunbind 6 0 0 0 7 0 0 0;
vpedeinit 0;
vifdeinit;
q;