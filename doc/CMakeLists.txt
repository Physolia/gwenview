########### install files ###############
#
#
if(KF${QT_MAJOR_VERSION}DocTools_FOUND)
    kdoctools_create_handbook(index.docbook INSTALL_DESTINATION ${KDE_INSTALL_DOCBUNDLEDIR}/en SUBDIR gwenview)
endif()
