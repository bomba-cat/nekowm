cmake_minimum_required(VERSION 3.10)
project(nekowm C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O3")

set(SRC
  src/bar.c
  src/log.c
  src/message.c
  src/keyboard.c
  src/window.c
  src/event.c
  src/util.c
  src/neko.c
)

add_executable(nekowm ${SRC})

target_link_libraries(nekowm
  xcb
  xcb-keysyms
  xcb-randr
)

install(TARGETS nekowm
  RUNTIME DESTINATION bin
)

add_custom_target(session
  COMMAND ${CMAKE_COMMAND} -E make_directory /usr/share/xsessions
  COMMAND ${CMAKE_COMMAND} -E echo "[Desktop Entry]" > /usr/share/xsessions/nekowm.desktop
  COMMAND ${CMAKE_COMMAND} -E echo "Name=NekoWM" >> /usr/share/xsessions/nekowm.desktop
  COMMAND ${CMAKE_COMMAND} -E echo "Comment=Minimal tiling window manager" >> /usr/share/xsessions/nekowm.desktop
  COMMAND ${CMAKE_COMMAND} -E echo "Exec=${CMAKE_INSTALL_PREFIX}/bin/nekowm" >> /usr/share/xsessions/nekowm.desktop
  COMMAND ${CMAKE_COMMAND} -E echo "TryExec=${CMAKE_INSTALL_PREFIX}/bin/nekowm" >> /usr/share/xsessions/nekowm.desktop
  COMMAND ${CMAKE_COMMAND} -E echo "Type=Application" >> /usr/share/xsessions/nekowm.desktop
  COMMAND ${CMAKE_COMMAND} -E echo "DesktopNames=nekowm" >> /usr/share/xsessions/nekowm.desktop
)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -E echo "Uninstalling ${CMAKE_INSTALL_PREFIX}/bin/nekowm"
  COMMAND rm -f ${CMAKE_INSTALL_PREFIX}/bin/nekowm
)
