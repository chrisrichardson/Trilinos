// @HEADER
// ************************************************************************
//
//               Rapid Optimization Library (ROL) Package
//                 Copyright (2014) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact lead developers:
//              Drew Kouri   (dpkouri@sandia.gov) and
//              Denis Ridzal (dridzal@sandia.gov)
//
// ************************************************************************
// @HEADER

#ifdef ENABLE_NUMPY

#ifndef PYROL_NUMPYVECTOR_HPP
#define PYROL_NUMPYVECTOR_HPP

#include "PyROL.hpp"
#include "PyROL_BaseVector.hpp"

namespace PyROL {

/** \class PyROL::NumPyVector
    \brief Provides a ROL interface for 1-dimensional NumPy arrays of double type
*/
class NumPyVector : public BaseVector {

  template <typename T> using RCP = Teuchos::RCP<T>;

  using Vector          = ROL::Vector<double>;

  using UnaryFunction   = ROL::Elementwise::UnaryFunction<double>;
  using BinaryFunction  = ROL::Elementwise::BinaryFunction<double>;
  using ReductionOp     = ROL::Elementwise::ReductionOp<double>;


private:

  PyObject*      pyVector_;
  PyArrayObject* pyArray_;  // Pointer to NumPy array object

  bool    hasOwnership_;   // Vector is responsible for deallocating memory of any
                           // NumPy array which is created at the C API level
                           // Python is will deallocate arrays it creates

  double*  data_;          // Pointer to the C-Array data encapsulated by the NumPy array

  npy_intp size_;          // Number of elements in the array


public:

  NumPyVector( PyObject* pyVector, bool hasOwnership=false ) :
    pyVector_(pyVector), pyArray_((PyArrayObject*)pyVector),
    hasOwnership_(hasOwnership) {

    Py_Initialize();
    //    import_array();

    // Get number of dimensions of array and throw excpetion if not 1
    int ndim = PyArray_NDIM(pyArray_);
    TEUCHOS_TEST_FOR_EXCEPTION( ndim != 1, std::logic_error,
                                "Error: PyROL only supports 1-d NumPy arrays." );

    // Get number of elements in array
    npy_intp* shape = PyArray_SHAPE(pyArray_);

    size_ = shape[0];
    data_ = static_cast<double*>(PyArray_DATA(pyArray_));
  }

  virtual ~NumPyVector( ) {
    if(hasOwnership_) {
      Py_XDECREF(pyArray_);
    }
  }

  PyObject* getPyVector( ) {
    return pyVector_;
  }

  const PyObject* getPyVector( ) const {
    return pyVector_;
  }

  int dimension( ) const {
    return static_cast<int>(size_);
  }

  RCP<Vector> clone() const {
    int nd = 1;
    npy_intp dims[] = {size_};
    PyObject* pyVector = PyArray_SimpleNew(nd,dims,NPY_DOUBLE);
    return Teuchos::rcp( new NumPyVector( pyVector, true ) );
  }

  RCP<Vector> basis( int i ) const {
    int nd = 1;
    npy_intp dims[] = {size_};
    PyObject* pyVector = PyArray_SimpleNew(nd,dims,NPY_DOUBLE);
    double* data = static_cast<double*>(PyArray_DATA((PyArrayObject*)pyVector));
    for( npy_intp j=0; j<size_; ++j ) {
        data[j] = 1.0*(i==j);
    }
    return Teuchos::rcp( new NumPyVector( pyVector, true ) );
  }

  void applyUnary( const UnaryFunction &f ) {
    for( npy_intp i=0; i<size_; ++i ) {
      data_[i] = f.apply(data_[i]);
    }
  }

  void applyBinary( const BinaryFunction &f, const Vector &x ) {
    const NumPyVector &ex = Teuchos::dyn_cast<const NumPyVector>(x);
    for( npy_intp i=0; i<size_; ++i ) {
      data_[i] = f.apply(data_[i],ex[i]);
    }
  }

  double reduce( const ReductionOp &r ) const {
    double result = r.initialValue();
    for( npy_intp i=0; i<size_; ++i ) {
      r.reduce(data_[i],result);
    }
    return result;
  }

  // Element access methods
  const double& operator[] ( npy_intp i ) const {
    return data_[i];
  }

  double& operator[] ( npy_intp i ) {
    return data_[i];
  }

}; // class NumPyVector

} // namespace PyROL

#endif // PYROL_NUMPYVECTOR_HPP

#endif // ENABLE_NUMPY
