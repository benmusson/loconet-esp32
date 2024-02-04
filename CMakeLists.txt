cmake_minimum_required(VERSION 3.16.0)
project(loconet_esp32)

idf_component_register(SRCS "src/loconet_esp32.c" INCLUDE_DIRS "include")