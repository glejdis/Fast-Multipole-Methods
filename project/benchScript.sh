#!/bin/bash

sudo echo 0 | sudo tee /sys/devices/system/cpu/cpufreq/boost
for cpu in /sys/devices/system/cpu/cpu?
do
  for mm in max
  do
    sudo echo 2200000 | sudo tee $cpu/cpufreq/scaling_${mm}_freq
  done
done

make clean
make space
taskset --c 5-5 ./space

sudo echo 1 | sudo tee /sys/devices/system/cpu/cpufreq/boost
for cpu in /sys/devices/system/cpu/cpu?
do
  for mm in min max
  do
    sudo cat $cpu/cpufreq/cpuinfo_${mm}_freq | sudo tee $cpu/cpufreq/scaling_${mm}_freq
  done
done

sudo -R chown michi:michi benchmark
sudo -R chown michi:michi data
make clean
