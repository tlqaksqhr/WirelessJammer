#!/bin/bash

sudo ifconfig wlan0 down
sudo iwconfig wlan0 mode monitor
sudo ifconfig wlan0 up

sudo ./chanhop.sh -i wlan0 -d .10&

sudo core/beacon wlan0
sudo core/station wlan0
sudo core/deauth wlan0

python index.py
