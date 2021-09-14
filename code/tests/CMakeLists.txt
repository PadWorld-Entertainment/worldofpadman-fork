enable_testing()

project(woptest)

set(SRCS_BASE
	../null/null_client.c
	../null/null_input.c
	../null/null_snddma.c
	../null/null_server.c
	../null/null_sys.c

	${QCOMMON_COLLISION_SRCS}
	../qcommon/cmd.c
	../qcommon/common.c
	../qcommon/cvar.c
	../qcommon/files.c
	../qcommon/huffman.c
	../qcommon/ioapi.c
	../qcommon/md4.c
	../qcommon/msg.c
	../qcommon/net_chan.c
	../qcommon/net_ip.c
	../qcommon/q_math.c
	../qcommon/q_shared.c
	../qcommon/unzip.c
	${QCOMMON_VM_SRCS}
	../sys/con_log.c
)

if (WIN32)
	list(APPEND SRCS_BASE ../sys/sys_win32.c)
	list(APPEND SRCS_BASE ../sys/win_resource.rc)
else()
	if(APPLE)
		list(APPEND SRCS_BASE ../sys/sys_osx.m)
	endif()
	list(APPEND SRCS_BASE ../sys/sys_unix.c)
endif()

set(LIBS zlib ${CMAKE_DL_LIBS})
if (MSVC)
	list(APPEND LIBS ws2_32 winmm psapi)
elseif (APPLE)
	set(FRAMEWORKS Cocoa)
	foreach (_framework ${FRAMEWORKS})
		list(APPEND LIBS "-framework ${_framework}")
	endforeach()
	list(APPEND TEST_DEFINES _THREAD_SAFE=1)
else()
	set(CMAKE_REQUIRED_LIBRARIES m)
	check_symbol_exists("cosf" "math.h" HAVE_COSF)
	set(CMAKE_REQUIRED_LIBRARIES)
	list(APPEND LIBS m)
endif()

set(TEST_DEFINES TESTS)
if (USE_VOIP)
	list(APPEND TEST_DEFINES USE_VOIP)
endif()

set(SRCS_MAIN test_main.c ${SRCS_BASE})
wop_add_executable(TARGET ${PROJECT_NAME}-main SRCS ${SRCS_MAIN})
add_asm(${PROJECT_NAME}-main)
add_botlib(${PROJECT_NAME}-main)
target_link_libraries(${PROJECT_NAME}-main ${LIBS})
target_include_directories(${PROJECT_NAME}-main PRIVATE ../qcommon)
target_compile_definitions(${PROJECT_NAME}-main PRIVATE ${TEST_DEFINES})
add_test(NAME ${PROJECT_NAME}-main COMMAND $<TARGET_FILE:${PROJECT_NAME}-main>)

set(SRCS_UI
	test_ui.c

	../ui/ui_main.c
	../ui/ui_addbots.c
	../ui/ui_atoms.c
	../ui/ui_callvote.c
	../ui/ui_confirm.c
	../ui/ui_connect.c
	../ui/ui_controls2.c
	../ui/ui_credits.c
	../ui/ui_demo2.c
	../ui/ui_display.c
	../ui/ui_exit.c
	../ui/ui_gameinfo.c
	../ui/ui_help.c
	../ui/ui_ingame.c
	../ui/ui_menu.c
	../ui/ui_mfield.c
	../ui/ui_mods.c
	../ui/ui_music.c
	../ui/ui_network.c
	../ui/ui_password.c
	../ui/ui_players.c
	../ui/ui_playersettings.c
	../ui/ui_preferences.c
	../ui/ui_qmenu.c
	../ui/ui_removebots.c
	../ui/ui_serverinfo.c
	../ui/ui_servers2.c
	../ui/ui_setup.c
	../ui/ui_sound.c
	../ui/ui_specifyserver.c
	../ui/ui_startserver.c
	../ui/ui_team.c
	../ui/ui_teamorders.c
	../ui/ui_video.c
	../ui/ui_voicechat.c
	../ui/ui_syscalls.c

	../game/bg_misc.c
	../game/bg_lib.c
	../cgame/wopc_advanced2d.c

	${SRCS_BASE}
)
wop_add_executable(TARGET ${PROJECT_NAME}-ui SRCS ${SRCS_UI})
add_asm(${PROJECT_NAME}-ui)
target_link_libraries(${PROJECT_NAME}-ui ${LIBS})
target_include_directories(${PROJECT_NAME}-ui PRIVATE ../qcommon)
target_include_directories(${PROJECT_NAME}-ui PRIVATE ../ui)
target_compile_definitions(${PROJECT_NAME}-ui PRIVATE ${TEST_DEFINES})
target_compile_definitions(${PROJECT_NAME}-ui PRIVATE UI)
target_compile_definitions(${PROJECT_NAME}-ui PRIVATE UI_HARD_LINKED)
add_test(NAME ${PROJECT_NAME}-ui COMMAND $<TARGET_FILE:${PROJECT_NAME}-ui>)
