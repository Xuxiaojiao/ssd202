#!/bin/sh

module="vpu"
device="vpu"
mode="664"

# Group: since distributions do	it differently,	look for wheel or use staff
if grep	'^staff:' /etc/group > /dev/null; then
    group="staff"
else
    group="wheel"
fi

# invoke insmod	with all arguments we got
# and use a pathname, as newer modutils	do not look in .	by default
/sbin/insmod -f	./$module.ko $*	|| exit	-1

major=`cat /proc/devices | busybox awk "\\$2==\"$module\" {print \\$1}"`

# Remove stale nodes and replace them, then give gid and perms
# Usually the script is	shorter, it is simple that has several devices in it.

rm -f /dev/${device}
mknod /dev/${device} c $major 0
chgrp $group /dev/${device}
chmod $mode  /dev/${device}
chown $SUDO_USER.$SUDO_USER /dev/${device}
 
