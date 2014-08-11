cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

project(tools)

set(GETTEXT_SRCS
    gettext.c
)

set(NGETTEXT_SRCS
    ngettext.c
)

set(ENVSUBST_SRCS
    envsubst.c
)

add_executable(gettext ${GETTEXT_SRCS})

add_executable(ngettext ${NGETTEXT_SRCS})

add_executable(envsubst ${ENVSUBST_SRCS})
