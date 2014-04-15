#
# The ADD_EXAMPLE macro (defined in examples/CMakeLists.txt) ensures that
# the example is correctly built for the requested connector. It is equivalent
# to the following two lines:
#   ADD_EXECUTABLE(hello.wt hello.C)
#   TARGET_LINK_LIBRARIES(hello.wt ${EXAMPLES_CONNECTOR})
# except when the ISAPI (for Microsoft IIS) connector is used, where it will
# build a DLL with the proper symbols exported.
#

WT_ADD_EXAMPLE(hello.wt hello.C)
WT_ADD_EXAMPLE(hello2.wt hello2.C)
#set(EXAMPLE_CONNECTOR ${EXAMPLES_CONNECTOR} "libkswidget.a")

#
# If you have Wt installed somehwere, you should use the
# installed Wt header files for your own Wt projects.
# e.g. INCLUDE_DIRECTORIES(/usr/local/include)
# instead of the following:
#
INCLUDE_DIRECTORIES(${WT_SOURCE_DIR}/src "/home/freddi/.project/kswidget/include")
#link_directories("/home/freddi/.project/kswidget/Debug") failed

#add_library(kswidget STATIC IMPORTED)
#set_target_properties(kswidget PROPERTIES IMPORTED_LOCATION "/home/freddi/build/kswidget/libkswidget.a")

find_library(KSLIBS
 NAMES kswidget
 PATHS
  "/home/freddi/build/kswidget"
  "/home/freddi/.project/kswidgt/Debug/"
)

target_link_libraries(hello.wt ${KSLIBS})
