
find_package(Qt6 COMPONENTS Concurrent Network Widgets LinguistTools REQUIRED)

set(CMAKE_AUTOUIC_SEARCH_PATHS forms)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB launcher_src
	"include/*.h"
	"src/*.cpp"
)

file(GLOB launcher_forms
	"forms/*.ui"
)

add_executable(launcher WIN32
	${launcher_src}
	${launcher_forms}
	resources.qrc
)

# lrelease the hand-maintained .ts files into .qm and embed them in the launcher binary
# (RESOURCE_PREFIX below) so there's no separate translations/ folder to lose on install -
# QTranslator::load() in main.cpp reads them back from ":/translations/...".
qt_add_translations(launcher
	TS_FILES translations/katyps5_es.ts
	RESOURCE_PREFIX "/translations"
)


target_link_libraries(launcher Qt6::Concurrent Qt6::Network Qt6::Widgets)
if(WIN32)
target_link_libraries(launcher setupapi)
endif()
if (CLANG AND NOT KATY_CLANG_CL)
target_compile_options(launcher PRIVATE -frtti)
target_link_libraries(launcher pthread)
endif()
target_include_directories(launcher PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src")
set_target_properties(launcher PROPERTIES AUTOUIC ON AUTOMOC ON AUTORCC ON)

if(WIN32 AND NOT KATY_CLANG_CL)
	set_target_properties(launcher PROPERTIES LINK_FLAGS "${KATY_LD_OPTIONS}")
elseif(LINUX)
	# Use the same linker as katy_emulator.
	set_target_properties(launcher PROPERTIES LINK_FLAGS "${KATY_LD_OPTIONS}")
endif()

add_dependencies(launcher katy_emulator)

if(KATY_CLANG_CL)
	add_custom_command(TARGET launcher POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:katy_emulator> $<TARGET_FILE_DIR:launcher>/$<TARGET_PDB_FILE_NAME:katy_emulator>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE_DIR:katy_emulator>/${KATY_EMULATOR_MAP_NAME} $<TARGET_FILE_DIR:launcher>/${KATY_EMULATOR_MAP_NAME}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${KATY_THIRD_PARTY_DIR}/winpthread/bin/libwinpthread-1.dll" $<TARGET_FILE_DIR:launcher>/libwinpthread-1.dll
	)
endif()

if(WIN32)
    find_program(QT_WINDEPLOYQT NAMES windeployqt PATHS "${Qt6_DIR}/../../../bin")
    if(NOT QT_WINDEPLOYQT)
    	message(FATAL_ERROR "Could not find windeployqt")
    endif()
	add_custom_command(TARGET launcher POST_BUILD
		COMMAND ${QT_WINDEPLOYQT} $<TARGET_FILE:launcher>
			--no-compiler-runtime
			--no-system-d3d-compiler
			--no-system-dxc-compiler
			--no-opengl-sw
			--no-translations
		COMMENT "Deploying Qt runtime and TLS plugins"
		VERBATIM
	)
endif()

set(launcher_name "launcher")

set_target_properties(launcher PROPERTIES OUTPUT_NAME ${launcher_name})

install(TARGETS launcher DESTINATION .)

if(WIN32)
    install(CODE "
    		execute_process(
    			COMMAND ${QT_WINDEPLOYQT} \"\${CMAKE_INSTALL_PREFIX}/${launcher_name}.exe\"
				--no-compiler-runtime
				--no-system-d3d-compiler
				--no-system-dxc-compiler
				--no-opengl-sw
				--no-translations
    		)
    ")
elseif(LINUX)
	# Bundle the Qt runtime and plugins.
	set_target_properties(launcher PROPERTIES
		INSTALL_RPATH "$ORIGIN/lib"
		BUILD_WITH_INSTALL_RPATH FALSE
		LINK_FLAGS "${KATY_LD_OPTIONS} -Wl,--disable-new-dtags")

	# Support distribution and official Qt layouts.
	get_target_property(katy_qt_core_lib Qt6::Core IMPORTED_LOCATION_RELEASE)
	if(NOT katy_qt_core_lib)
		get_target_property(katy_qt_core_lib Qt6::Core IMPORTED_LOCATION)
	endif()
	get_filename_component(katy_qt_libdir "${katy_qt_core_lib}" DIRECTORY)

	set(katy_qt_plugins_dir "")
	foreach(katy_candidate
			"${katy_qt_libdir}/qt6/plugins"
			"${katy_qt_libdir}/../plugins"
			"${Qt6_DIR}/../../../plugins")
		if(IS_DIRECTORY "${katy_candidate}/platforms")
			get_filename_component(katy_qt_plugins_dir "${katy_candidate}" ABSOLUTE)
			break()
		endif()
	endforeach()

	if(katy_qt_plugins_dir)
		message(STATUS "Qt plugins for launcher deployment: ${katy_qt_plugins_dir}")
		set(katy_qt_plugin_files "")
		foreach(katy_qt_plugin platforms tls imageformats xcbglintegrations wayland-shell-integration)
			if(IS_DIRECTORY "${katy_qt_plugins_dir}/${katy_qt_plugin}")
				file(GLOB katy_qt_plugin_group
					"${katy_qt_plugins_dir}/${katy_qt_plugin}/*.so"
					"${katy_qt_plugins_dir}/${katy_qt_plugin}/*.so.*")
				list(APPEND katy_qt_plugin_files ${katy_qt_plugin_group})
				install(DIRECTORY "${katy_qt_plugins_dir}/${katy_qt_plugin}" DESTINATION "plugins")
			endif()
		endforeach()
		install(CODE "
			set(katy_plugin_files \"${katy_qt_plugin_files}\")
			file(GET_RUNTIME_DEPENDENCIES
				EXECUTABLES \"${CMAKE_CURRENT_BINARY_DIR}/${launcher_name}\"
				LIBRARIES \${katy_plugin_files}
				RESOLVED_DEPENDENCIES_VAR katy_resolved
				POST_INCLUDE_REGEXES \"libQt6\" \"libicu\"
				POST_EXCLUDE_REGEXES \".*\")
			foreach(katy_dep \${katy_resolved})
				file(INSTALL DESTINATION \"\${CMAKE_INSTALL_PREFIX}/lib\"
					TYPE SHARED_LIBRARY FOLLOW_SYMLINK_CHAIN FILES \"\${katy_dep}\")
			endforeach()
		")
		install(CODE "
			file(WRITE \"\${CMAKE_INSTALL_PREFIX}/qt.conf\" \"[Paths]\\nPlugins = plugins\\n\")
		")
	else()
		message(WARNING "Qt plugin directory not found; the installed launcher will need a system Qt")
	endif()
endif()


set(tidy_dirs "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(iwyu_maps "${CMAKE_CURRENT_SOURCE_DIR}/utils/qt6_16.imp")

get_property(inc_headers TARGET launcher PROPERTY INCLUDE_DIRECTORIES)

list(APPEND inc_headers
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/launcher_autogen/include
	${Qt6Widgets_INCLUDE_DIRS}
	${Qt6Core_INCLUDE_DIRS}
	${Qt6Gui_INCLUDE_DIRS}
	#${Qt6_DIR}/../../../include
	#${Qt6_DIR}/../../../include/QtWidgets
	#${Qt6_DIR}/../../../include/QtCore
	#${Qt6_DIR}/../../../include/QtGui
)

include_what_you_use_with_mappings(launcher "${inc_headers}" "${iwyu_maps}")

clang_tidy_check(launcher "" "${tidy_dirs}" "${inc_headers}")
