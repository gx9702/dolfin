// Copyright (C) 2008 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2008.
//
// First added:  2008-01-07
// Last changed: 2009-09-05

#ifdef HAS_MPI
#include <mpi.h>
#endif

#ifdef HAS_PETSC
#include <petsc.h>
#endif

#ifdef HAS_SLEPC
#include <slepc.h>
#endif

#include <libxml/parser.h>
#include <dolfin/common/constants.h>
#include <dolfin/log/dolfin_log.h>
#include "SubSystemsManager.h"

using namespace dolfin;

// Initialise static data
dolfin::SubSystemsManager dolfin::SubSystemsManager::sub_systems_manager;

//-----------------------------------------------------------------------------
SubSystemsManager::SubSystemsManager() : petsc_initialized(false),
                                         control_mpi(false)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
SubSystemsManager::SubSystemsManager(const SubSystemsManager& sub_sys_manager)
{
  error("Should not be using copy constructor of SubSystemsManager.");
}
//-----------------------------------------------------------------------------
SubSystemsManager::~SubSystemsManager()
{
  finalize();
}
//-----------------------------------------------------------------------------
void SubSystemsManager::init_mpi()
{
#ifdef HAS_MPI
  if( MPI::Is_initialized() )
    return;

  // Initialise MPI and take responsibility
  MPI::Init();
  sub_systems_manager.control_mpi = true;
#else
  // Do nothing
#endif
}
//-----------------------------------------------------------------------------
void SubSystemsManager::init_petsc()
{
#ifdef HAS_PETSC
  if ( sub_systems_manager.petsc_initialized )
    return;

  info(TRACE, "Initializing PETSc (ignoring command-line arguments).");

  // Dummy command-line arguments for PETSc. This is needed since
  // PetscInitializeNoArguments() does not seem to work.
  int argc = 0;
  char** argv = NULL;

  // Initialize PETSc
  init_petsc(argc, argv);
#else
  error("DOLFIN has not been configured for PETSc.");
#endif
}
//-----------------------------------------------------------------------------
void SubSystemsManager::init_petsc(int argc, char* argv[])
{
#ifdef HAS_PETSC
  if ( sub_systems_manager.petsc_initialized )
    return;

  // Get status of MPI before PETSc initialisation
  const bool mpi_init_status = mpi_initialized();

  // Print message if PETSc is intialised with command line arguments
  if (argc > 1)
    info(TRACE, "Initializing PETSc with given command-line arguments.");

  // Initialize PETSc
  PetscInitialize(&argc, &argv, PETSC_NULL, PETSC_NULL);

#ifdef HAS_SLEPC
  // Initialize SLEPc
  SlepcInitialize(&argc, &argv, PETSC_NULL, PETSC_NULL);
#endif

  sub_systems_manager.petsc_initialized = true;

  // Determine if PETSc initialised MPI (and is therefore responsible for MPI finalization)
  if (mpi_initialized() and !mpi_init_status)
    sub_systems_manager.control_mpi = false;
#else
  error("DOLFIN has not been configured for PETSc.");
#endif
}
//-----------------------------------------------------------------------------
void SubSystemsManager::finalize()
{
  // Finalize subsystems in the correct order
  finalize_petsc();
  finalize_mpi();

  // Clean up libxml2 parser
  xmlCleanupParser();
}
//-----------------------------------------------------------------------------
bool SubSystemsManager::responsible_mpi()
{
  return sub_systems_manager.control_mpi;
}
//-----------------------------------------------------------------------------
bool SubSystemsManager::responsible_petsc()
{
  return sub_systems_manager.petsc_initialized;
}
//-----------------------------------------------------------------------------
void SubSystemsManager::finalize_mpi()
{
#ifdef HAS_MPI
  // Finalise MPI if required
  if (MPI::Is_initialized() and sub_systems_manager.control_mpi)
  {
    // Check in MPI has already been finalised (possibly incorrectly by a
    // 3rd party libary). Is it hasn't, finalise as normal.
    if (!MPI::Is_finalized())
      MPI::Finalize();
    else
    {
      // Use std::cout since log system may fail because MPI has been shut down.
      std::cout << "DOLFIN is responsible for MPI, but it has been finalized elsewhere prematurely." << std::endl;
      std::cout << "This is usually due to a bug in a 3rd party library, and can lead to unpredictable behaviour." << std::endl;
      std::cout << "If using PyTrilinos, make sure that PyTrilinos modules are imported before the DOLFIN module." << std::endl;
    }

    sub_systems_manager.control_mpi = false;
  }
#else
  // Do nothing
#endif
}
//-----------------------------------------------------------------------------
void SubSystemsManager::finalize_petsc()
{
#ifdef HAS_PETSC
  if (sub_systems_manager.petsc_initialized)
  {
    PetscFinalize();
    sub_systems_manager.petsc_initialized = false;

#ifdef HAS_SLEPC
    SlepcFinalize();
#endif
  }
#else
  // Do nothing
#endif
}
//-----------------------------------------------------------------------------
bool SubSystemsManager::mpi_initialized()
{
  // This function not affected if MPI_Finalize has been called. It returns
  // true if MPI_Init has been called at any point, even if MPI_Finalize has
  // been called.

#ifdef HAS_MPI
  return MPI::Is_initialized();
#else
  // DOLFIN is not configured for MPI (it might be through PETSc)
  return false;
#endif
}
//-----------------------------------------------------------------------------
