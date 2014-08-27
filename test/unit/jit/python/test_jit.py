#!/usr/bin/env py.test

"""Unit tests for the JIT compiler"""

# Copyright (C) 2011 Anders Logg
#
# This file is part of DOLFIN.
#
# DOLFIN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DOLFIN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.

import pytest
from dolfin import *


def test_nasty_jit_caching_bug():

    # This may result in something like "matrices are not aligned"
    # from FIAT if the JIT caching does not recognize that the two
    # forms are different

    for representation in ["tensor", "quadrature"]:

        parameters["form_compiler"]["representation"] = representation

        M1 = assemble(Constant(1.0)*dx(UnitSquareMesh(4, 4)))
        M2 = assemble(Constant(1.0)*dx(UnitCubeMesh(4, 4, 4)))

        assert round(M1 - 1.0, 7) == 0
        assert round(M2 - 1.0, 7) == 0


def test_compile_extension_module():

    # This test should do basically the same as the docstring of
    # the compile_extension_module function in compilemodule.py.
    # Remember to update the docstring if the test is modified!

    if not has_linear_algebra_backend("PETSc"):
        return

    from numpy import arange, exp
    code = """
    namespace dolfin {

      void PETSc_exp(std::shared_ptr<dolfin::PETScVector> vec)
      {
        Vec x = vec->vec();
        assert(x);
        VecExp(x);
      }
    }
    """
    for module_name in ["mypetscmodule", ""]:
        ext_module = compile_extension_module(\
            code, module_name=module_name,\
            additional_system_headers=["petscvec.h"])
        vec = PETScVector(mpi_comm_world(), 10)
        np_vec = vec.array()
        np_vec[:] = arange(len(np_vec))
        vec.set_local(np_vec)
        ext_module.PETSc_exp(vec)
        np_vec[:] = exp(np_vec)
        assert (np_vec == vec.array()).all()


def test_compile_extension_module_kwargs():
    # This test check that instant_kwargs of compile_extension_module
    # are taken into account when computing signature
    m2 = compile_extension_module('', cppargs='-O2')
    m0 = compile_extension_module('', cppargs='')
    assert not m2.__file__ == m0.__file__
