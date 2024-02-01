set(SRCS
    main.cpp
    MorphingWindow.cpp
    MorphingWindow.h
    
    Utils/Buffers.h
    Utils/Drawable.h

    Utils/FpsGuard.h
    Utils/FpsGuard.cpp

    Shaders/diffuse.fs
    Shaders/diffuse.vs

    resources.qrc
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(demo-app ${SRCS})

target_link_libraries(demo-app
    PRIVATE
        Qt5::Widgets
        FGL::Base
        thirdparty::tinygltf
)