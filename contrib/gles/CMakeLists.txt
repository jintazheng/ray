IF (WIN32 OR MINGW AND BUILD_MUTILTHREAD_DLL)
    IF(BUILD_OPENGL_ES_LIB EQUAL 1)
        IF(LIBRARY_OUT_NAME MATCHES "64")
            CONFIGURE_FILE("lib/AMD/x86-64/libEGL.lib" ${LIBRARY_OUTPUT_PATH} COPYONLY)
            CONFIGURE_FILE("lib/AMD/x86-64/libGLESv2.lib" ${LIBRARY_OUTPUT_PATH} COPYONLY)
            CONFIGURE_FILE("lib/AMD/x86-64/libEGL.dll" ${LIBRARY_OUTPUT_PATH} COPYONLY)
            CONFIGURE_FILE("lib/AMD/x86-64/libGLESv2.dll" ${LIBRARY_OUTPUT_PATH} COPYONLY)
        ELSE()
            CONFIGURE_FILE("lib/AMD/x86/libEGL.lib" ${LIBRARY_OUTPUT_PATH} COPYONLY)
            CONFIGURE_FILE("lib/AMD/x86/libGLESv2.lib" ${LIBRARY_OUTPUT_PATH} COPYONLY)
            CONFIGURE_FILE("lib/AMD/x86/libEGL.dll" ${LIBRARY_OUTPUT_PATH} COPYONLY)
            CONFIGURE_FILE("lib/AMD/x86/libGLESv2.dll" ${LIBRARY_OUTPUT_PATH} COPYONLY)
        ENDIF()
    ELSEIF(BUILD_OPENGL_ES_LIB EQUAL 2)
        IF(LIBRARY_OUT_NAME MATCHES "64")
            CONFIGURE_FILE("lib/Mali/x64/libEGL.lib" ${LIBRARY_OUTPUT_PATH_DEBUG} COPYONLY)
            CONFIGURE_FILE("lib/Mali/x64/libGLESv2.lib" ${LIBRARY_OUTPUT_PATH_DEBUG} COPYONLY)
            CONFIGURE_FILE("lib/Mali/x64/libMaliEmulator.lib" ${LIBRARY_OUTPUT_PATH_DEBUG} COPYONLY)
            CONFIGURE_FILE("lib/Mali/x64/libEGL.dll" ${LIBRARY_OUTPUT_PATH_DEBUG} COPYONLY)
            CONFIGURE_FILE("lib/Mali/x64/libGLESv2.dll" ${LIBRARY_OUTPUT_PATH_DEBUG} COPYONLY)
            CONFIGURE_FILE("lib/Mali/x64/libMaliEmulator.dll" ${LIBRARY_OUTPUT_PATH_DEBUG} COPYONLY)
            CONFIGURE_FILE("lib/Mali/x64/log4cplus.dll" ${LIBRARY_OUTPUT_PATH_DEBUG} COPYONLY)

            CONFIGURE_FILE("lib/Mali/x64/libEGL.lib" ${LIBRARY_OUTPUT_PATH_RELEASE} COPYONLY)
            CONFIGURE_FILE("lib/Mali/x64/libGLESv2.lib" ${LIBRARY_OUTPUT_PATH_RELEASE} COPYONLY)
            CONFIGURE_FILE("lib/Mali/x64/libMaliEmulator.lib" ${LIBRARY_OUTPUT_PATH_RELEASE} COPYONLY)
            CONFIGURE_FILE("lib/Mali/x64/libEGL.dll" ${LIBRARY_OUTPUT_PATH_RELEASE} COPYONLY)
            CONFIGURE_FILE("lib/Mali/x64/libGLESv2.dll" ${LIBRARY_OUTPUT_PATH_RELEASE} COPYONLY)
            CONFIGURE_FILE("lib/Mali/x64/libMaliEmulator.dll" ${LIBRARY_OUTPUT_PATH_RELEASE} COPYONLY)
            CONFIGURE_FILE("lib/Mali/x64/log4cplus.dll" ${LIBRARY_OUTPUT_PATH_RELEASE} COPYONLY)
        ENDIF()
    ELSEIF(BUILD_OPENGL_ES_LIB EQUAL 3)
        IF(LIBRARY_OUT_NAME MATCHES "64")
            CONFIGURE_FILE("lib/PowerVR/x86_64/libEGL.lib" ${LIBRARY_OUTPUT_PATH} COPYONLY)
            CONFIGURE_FILE("lib/PowerVR/x86_64/libEGL.dll" ${LIBRARY_OUTPUT_PATH} COPYONLY)
            CONFIGURE_FILE("lib/PowerVR/x86_64/libGLESv2.dll" ${LIBRARY_OUTPUT_PATH} COPYONLY)
            CONFIGURE_FILE("lib/PowerVR/x86_64/libGLESv2.lib" ${LIBRARY_OUTPUT_PATH} COPYONLY)
        ELSE()
            CONFIGURE_FILE("lib/PowerVR/x86_32/libEGL.lib" ${LIBRARY_OUTPUT_PATH} COPYONLY)
            CONFIGURE_FILE("lib/PowerVR/x86_32/libEGL.dll" ${LIBRARY_OUTPUT_PATH} COPYONLY)
            CONFIGURE_FILE("lib/PowerVR/x86_32/libGLESv2.lib" ${LIBRARY_OUTPUT_PATH} COPYONLY)
            CONFIGURE_FILE("lib/PowerVR/x86_32/libGLESv2.dll" ${LIBRARY_OUTPUT_PATH} COPYONLY)
        ENDIF()
    ENDIF()
ENDIF()