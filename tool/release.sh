#!/bin/bash

#grep 'SW_VERSION' application/bsp/bsp.c | sed -n '1p' | cut -d '"' -f 2
version=`find . -name "bsp.c" | xargs grep 'SW_VERSION' | sed -n '1p' | cut -d '"' -f 2`
#A=`date`
#echo ${A}

src_bin=${1}
#dir_path=${2}
dst_bin_perfix=${src_bin%%.*}
dst_bin_subfix=${src_bin##*.}
dst_bin=${dst_bin_perfix}_V${version}.${dst_bin_subfix}

#echo src_bin = $src_bin
#echo dir_path = $dir_path
#echo version = $version
#echo dst_bin_perfix = $dst_bin_perfix
#echo dst_bin_subfix = $dst_bin_subfix
#echo dst_bin_subfix = $dst_bin_subfix
#echo $(basename ${src_bin})
#echo $(dirname ${src_bin})
echo dst_bin = $dst_bin

cp ${src_bin} ${dst_bin}
#rm -r ${src_bin}
