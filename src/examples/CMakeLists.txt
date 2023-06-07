macro(LoadSparrow)
    target_link_libraries(${ARGN} PRIVATE SparrowCore)
    target_link_libraries(${ARGN} PRIVATE SparrowEditor)
    target_include_directories(${ARGN}
            PRIVATE "${SparrowExternalDir}"
            PRIVATE "${SparrowCoreDir}"
            PRIVATE "${SparrowEditorDir}"
            )

endmacro(LoadSparrow)

add_executable(SPWNewRSTest "SPWNewRSTest.cpp")
add_executable(SPWEditorTest "SPWEditorTest.cpp")

LoadSparrow(SPWNewRSTest)
LoadSparrow(SPWEditorTest)
