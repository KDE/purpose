add_definitions(-DTRANSLATION_DOMAIN=\"purpose_ktp-sendfile\")
add_share_plugin(ktpsendfileplugin ktpsendfileplugin.cpp)
target_link_libraries(ktpsendfileplugin KF5::I18n)

