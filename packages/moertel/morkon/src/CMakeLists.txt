# KokkosCore_config.h
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/packages/kokkos/core/src)
# TpetraKernels_config.h
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/packages/tpetra/kernels/src)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/packages/tpetra/core/src)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/packages/tpetra/classic/src)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/packages/tpetra/classic/NodeAPI)
# TeuchosCore_config.h
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/packages/teuchos/core/src)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/packages/teuchos/kokkoscompat/src)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/packages/teuchos/kokkoscomm/src)

# Other Kokkos includes needed
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/kokkos/core/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/kokkos/containers/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/kokkos/algorithms/src)
# Other Tpetra includes needed
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/tpetra/kernels/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/tpetra/kernels/src/impl)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/tpetra/core/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/tpetra/core/src/kokkos_refactor)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/tpetra/classic/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/tpetra/classic/NodeAPI)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/tpetra/classic/LinAlg)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/teuchos/core/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/teuchos/parameterlist/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/teuchos/kokkoscompat/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/teuchos/comm/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/teuchos/kokkoscomm/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/teuchos/numerics/src)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SET(INCLUDES
    mrk_api_classes.hpp
    mrk_compute_normals.hpp
    mrk_data_types.hpp
	mrk_interface_host_side_adapter.hpp
    mrk_interface_impl.hpp
	mrk_int_types.hpp
	mrk_manager_impl.hpp
    mrk_search_for_pallet_generating_faces.hpp
    mrk_compute_pallets_from_candidate_face_pairs.hpp
    mrk_planar_geom.hpp
   )

SET(CPPSOURCES
   )


#TRIBITS_ADD_LIBRARY(morkon HEADERS ${INCLUDES})

#SET_PROPERTY(TARGET morkon_exp_exp_lib APPEND PROPERTY LINKER_LANGUAGE CXX)

