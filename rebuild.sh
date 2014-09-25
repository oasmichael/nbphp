#!/bin/bash
sudo make clean
sudo make
sudo make install
sudo /etc/init.d/php-fpm restart