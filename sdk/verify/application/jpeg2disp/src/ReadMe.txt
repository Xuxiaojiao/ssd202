���뷽����
1.��makefile��SDK_LIBS,SDK_INCS,UAPI_INCS�ĳ�ʵ��project code���ڵ�·����
2.make clean;make ����

ʹ�÷�����
1.�����������logo��logo.jpg�Լ�lib�ļ��п���������ͬһ��Ŀ¼��
2.��libĿ¼export������������export LD_LIBRARY_PATH=/lib:$PWD/lib:$LD_LIBRARY_PATH
3.����logo: ./logo

ע�����
1.demoĬ��ʹ�õ���1024x600�����κ�jpegͼƬ�����Ҫ��panel��Ӧ�����κ�jpegͼƬ��Ҫ�滻��
2./config/fbdev.ini�����FB_WIDTH��FB_HEIGHT��Ҫ���ָ���Ļ�ֱ���һ��
	FB_WIDTH = 1024
	FB_HEIGHT = 600
