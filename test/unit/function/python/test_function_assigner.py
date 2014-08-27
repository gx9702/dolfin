#!/usr/bin/env py.test

"""Unit tests for the FunctionAssigner class"""

# Copyright (C) 2013 Johan Hake
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
#
# First added:  2013-11-07
# Last changed: 2013-11-07

import pytest
from dolfin import *
import numpy as np

mesh = UnitCubeMesh(8, 8, 8)
V = FunctionSpace(mesh, "CG", 1)
Q = FunctionSpace(mesh, "CG", 2)
W = VectorFunctionSpace(mesh, "CG", 1)
QQ = VectorFunctionSpace(mesh, "CG", 2)
R = FunctionSpace(mesh, "R", 0)
RR = VectorFunctionSpace(mesh, "R", 0)
QQV = QQ*V
WW = W*W
WR = W*R
WRR = W*RR

@pytest.fixture
def u0():
     u0_ = Function(V)
     u0_.vector()[:] = 1.
     return u0_

@pytest.fixture
def u1():
     u1_ = Function(V)
     u1_.vector()[:] = 2.
     return u1_

@pytest.fixture
def u2():
     u2_ = Function(V)
     u2_.vector()[:] = 3.
     return u2_

@pytest.fixture
def r():
    r_ = Function(R)
    r_.vector()[:] = 3.
    return r_

@pytest.fixture
def rr():
    rr_ = Function(RR)
    rr_.vector()[:] = 2.
    return rr_

@pytest.fixture
def w():
    w_ = Function(W)
    w_.vector()[:] = 4.
    return w_

@pytest.fixture
def ww():
    ww_ = Function(WW)
    ww_.vector()[:] = 5.
    return ww_

@pytest.fixture
def wr():
    wr_ = Function(WR)
    wr_.vector()[:] = 6.
    return wr_

@pytest.fixture
def wrr():
    wrr_ = Function(WRR)
    wrr_.vector()[:] = 7.
    return wrr_

@pytest.fixture
def q():
    q_ = Function(Q)
    q_.vector()[:] = 1.
    return q_

@pytest.fixture
def qq():
    qq_ = Function(QQ)
    qq_.vector()[:] = 2.
    return qq_

@pytest.fixture
def qqv():
    qqv_ = Function(QQV)
    qqv_.vector()[:] = 3.
    return qqv_


def test_1_1_assigner(w, ww, wr, wrr, q, r, qqv, u0, u1, u2):

    assigner = FunctionAssigner(W.sub(0), V)
    assigner.assign(w.sub(0), u0)

    assert np.all(w.sub(0, deepcopy=True).vector().array() == u0.vector().array())

    assign(w.sub(2), u2)
    assert np.all(w.sub(2, deepcopy=True).vector().array() == u2.vector().array())

    assigner = FunctionAssigner(V, W.sub(2))
    assigner.assign(u0, w.sub(2))

    assert np.all(u0.vector().array() == w.sub(2, deepcopy=True).vector().array())

    assign(u1, w.sub(1))
    assert np.all(u1.vector().array() == w.sub(1, deepcopy=True).vector().array())

    assigner = FunctionAssigner(WW.sub(0), W)
    assigner.assign(ww.sub(0), w)

    assign(wr.sub(0), w)
    assert np.all(wr.sub(0, deepcopy=True).vector().array() == w.vector().array())

    assign(wr.sub(1), r)
    assert np.all(wr.sub(1, deepcopy=True).vector().array() == r.vector().array())

    assign(qqv.sub(0).sub(0), q)
    assert np.all(qqv.sub(0).sub(0, deepcopy=True).vector().array() == q.vector().array())

    with pytest.raises(RuntimeError):
        assign(qqv.sub(0), q)
    with pytest.raises(RuntimeError):
        assign(qqv.sub(1), q)
    with pytest.raises(RuntimeError):
        assign(wrr.sub(1), w)


def test_N_1_assigner(u0, u1, u2, qq, qqv, rr, w, wrr, r):

    vv = Function(W)
    assigner = FunctionAssigner(W, [V,V,V])
    assigner.assign(vv, [u0, u1, u2])

    assert np.all(vv.sub(0, deepcopy=True).vector().array() == u0.vector().array())
    assert np.all(vv.sub(1, deepcopy=True).vector().array() == u1.vector().array())
    assert np.all(vv.sub(2, deepcopy=True).vector().array() == u2.vector().array())

    assign(qqv, [qq, u1])
    assert np.all(qqv.sub(0, deepcopy=True).vector().array() == qq.vector().array())
    assert np.all(qqv.sub(1, deepcopy=True).vector().array() == u1.vector().array())

    assign(wrr, [w, rr])
    assert np.all(wrr.sub(0, deepcopy=True).vector().array() == w.vector().array())
    assert np.all(wrr.sub(1, deepcopy=True).vector().array() == rr.vector().array())

    with pytest.raises(RuntimeError):
        assign(qqv, [qq, u1, u1])

    with pytest.raises(RuntimeError):
        assign(wrr, [w, r, r])

def test_1_N_assigner(u0, u1, u2, w, qq, qqv):

    assigner = FunctionAssigner([V,V,V], W)
    assigner.assign([u0, u1, u2], w)

    assert np.all(w.sub(0, deepcopy=True).vector().array() == u0.vector().array())
    assert np.all(w.sub(1, deepcopy=True).vector().array() == u1.vector().array())
    assert np.all(w.sub(2, deepcopy=True).vector().array() == u2.vector().array())

    assign([qq, u1], qqv)

    assert np.all(qqv.sub(0, deepcopy=True).vector().array() == qq.vector().array())
    assert np.all(qqv.sub(1, deepcopy=True).vector().array() == u1.vector().array())
