install(
    TARGETS hecate_exe
    RUNTIME COMPONENT hecate_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
