#!/bin/bash

#echo "number of argument $#"
cmd=""
progfile="../bowman"


if [ "$#" -eq "0" ]
	then
		cmd="make FLAG='-DDEBUG' "
else

	j=''
	if [ "$#" -gt "1" ];then
		if [ "$2" == "-j" ];then
			j="-j"
		fi
	fi

	if [ "$1" == "-j" ];then
		cmd="make FLAG='-DDEBUG' -j"
	elif [ "$1" == "run" ];then
		cmd="make FLAG='-DDEBUG' $j && $progfile"
	elif [ "$1" == "clean" ];then
		cmd="make clean && make FLAG='-DDEBUG' $j"
	elif [ "$1" == "clean_run" ];then
		cmd="make clean && make FLAG='-DDEBUG' $j && $progfile"
	elif [ "$1" == "check_leak" ];then
		cmd="make clean && make FLAG='-DDEBUG' $j && valgrind --leak-check=full $progfile"
	elif [ "$1" == "check_mem" ];then
		cmd="make clean && make FLAG='-DDEBUG' $j && valgrind --track-origins=yes $progfile"
	elif [ "$1" == "check_leak_mem" ];then
		cmd="make clean && make FLAG='-DDEBUG' $j && valgrind --leak-check=full --track-origins=yes $progfile"
	elif [ "$1" == "-h" ];then
		echo "./build [run/clean/clean_run/check_leak/check_mem/check_leak_mem]"
		
	fi
fi

echo $cmd
eval $cmd
