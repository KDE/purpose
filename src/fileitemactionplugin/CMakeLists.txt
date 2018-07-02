find_package(KF5 REQUIRED COMPONENTS KIO)

add_definitions(-DTRANSLATION_DOMAIN="purpose-fileitemaction")
include_directories(${CMAKE_SOURCE_DIR})

kcoreaddons_add_plugin(sharefileitemaction
    SOURCES sharefileitemaction.cpp
    JSON sharefileitemaction.json
    INSTALL_NAMESPACE "kf5/kfileitemaction")

target_link_libraries(sharefileitemaction
    KF5::KIOWidgets
    KF5::I18n
    KF5PurposeWidgets
)
