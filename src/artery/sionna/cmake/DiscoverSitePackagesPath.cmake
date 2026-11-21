##########
# Uility #
##########

# Discover path where python packages get installed. Sets
# `PYTHON_SITE_PACKAGES_PATH` variable with found info, raises
# error if Python was not discovered or path could not be deduced.

if(NOT DEFINED Python_FOUND)
    message(FATAL_ERROR "Could not discover python site-packages path: you sure you discovered Python?")
endif()

if(Python_SITEARCH)
    set(PYTHON_SITE_PACKAGES_PATH "${Python_SITEARCH}")
elseif(Python_SITELIB)
    set(PYTHON_SITE_PACKAGES_PATH "${Python_SITELIB}")
else()
    message(FATAL_ERROR "Could not determine Python site-packages path")
endif()
