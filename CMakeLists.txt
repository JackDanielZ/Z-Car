cmake_minimum_required(VERSION 2.8.1)

project(Z-Car)

find_package(Eina REQUIRED)
find_package(Eet REQUIRED)
find_package(Ecore REQUIRED Con Input)
find_package(Eo REQUIRED)
find_package(Efl REQUIRED)
find_package(Evas REQUIRED)
find_package(Elementary REQUIRED)
find_package(Emotion REQUIRED)

add_subdirectory(src)
