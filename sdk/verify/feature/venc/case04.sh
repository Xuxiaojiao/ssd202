#!/bin/sh

. _test.sh
ret_test=$?
if [ "${ret_test}" == "0" ]; then

log "\n\n${script}: Static Channel Attribute: CABAC (H264 only)\n"

expected_ret=0

#needed predefined variable in this script: $of, $codec, $unset_script, $expected_ret, $exe
#input $1: prefix of output file, output:$md5
run_case() {
  rm ${of} 2> /dev/null; #remove output file from previous test
  ${exe} 1 ${codec};     #how this program is executed.
  ret=$?                 #keep the result as run_std_case_check() input

  run_std_case_check $1

  #how result,$md5 should be checked
  run_std_md5_check $1
}


#the rest tests the CONFIG system
if true ; then
export VENC_GLOB_APPLY_CFG=1 #make something like nRows to take effect

run_suite() {
  export Cabac=0
  run_case cabac0

  export Cabac=1
  run_case cabac1
}

codec=h264
p_cabac0="b9b7db9663e39e95cfb28de65a8ebc16"
p_cabac1="0856d1863ed11d9f4c012a8799ddbb82"
s_cabac0="00000000000000000000000000000000"
s_cabac1="00000000000000000000000000000000"
run_suite


fi

print_result
fi