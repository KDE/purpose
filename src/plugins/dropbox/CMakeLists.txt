add_definitions(-DTRANSLATION_DOMAIN=\"purpose_dropbox\")

add_share_plugin(dropboxplugin dropboxplugin.cpp dropboxjob.cpp)
target_link_libraries(dropboxplugin KF5::KIOCore KF5::I18n KF5::Purpose KAccounts)
