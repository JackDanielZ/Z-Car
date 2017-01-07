set(CMAKE_BUILD_TYPE Debug)

LIST(APPEND CAR_SOURCES z-car.c common.c)

LIST(APPEND APP_SOURCES z-car_app.c common.c)

link_directories (${PC_LIBELEMENTARY_LIBRARY_DIRS})

include_directories(
   ${ELEMENTARY_INCLUDE_DIRS}
   ${EINA_INCLUDE_DIRS}
   ${EET_INCLUDE_DIRS}
   ${EO_INCLUDE_DIRS}
   ${ECORE_INCLUDE_DIRS}
   ${EVAS_INCLUDE_DIRS}
   ${EMOTION_INCLUDE_DIRS}
   )

add_executable(z-car ${CAR_SOURCES})

target_link_libraries(z-car
   pthread
   ${EFL_LIBRARIES}
   ${ELEMENTARY_LIBRARIES}
   ${EINA_LIBRARIES}
   ${EET_LIBRARIES}
   ${EO_LIBRARIES}
   ${ECORE_LIBRARIES}
   ${ECORE_CON_LIBRARIES}
   ${EVAS_LIBRARIES}
   ${EMOTION_LIBRARIES}
   ecore_file
   )

add_executable(z-car_app ${APP_SOURCES})

target_link_libraries(z-car_app
   pthread
   ${EFL_LIBRARIES}
   ${ELEMENTARY_LIBRARIES}
   ${EINA_LIBRARIES}
   ${EET_LIBRARIES}
   ${EO_LIBRARIES}
   ${ECORE_LIBRARIES}
   ${ECORE_CON_LIBRARIES}
   ${ECORE_INPUT_LIBRARIES}
   ${EVAS_LIBRARIES}
   ${EMOTION_LIBRARIES}
   ecore_file
   )

