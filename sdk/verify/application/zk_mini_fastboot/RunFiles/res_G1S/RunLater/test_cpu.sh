#!/bin/sh
while true;do
temp=$(top -n 1 |grep zkgui_ui | awk '{print $7}');
value=${temp:0:2};
defaultvalue=45;
echo $value;
if [ $value -gt $defaultvalue ];then
	ps -ef | grep zkgui_ui| grep -v grep |awk '{print $1}' | xargs kill -9
#kill -11 |ps -ef |grep zkgui_ui | head -n 1 |awk '{print $1}'
else
	echo "zkgui good"
fi
sleep 60;
done

