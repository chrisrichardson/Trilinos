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

#ifndef PYROL_TYPECONVERTERS_HPP
#define PYROL_TYPECONVERTERS_HPP

#include "PyROL.hpp"
#include "PyROL_PythonVector.hpp"
#include "PyROL_NumPyVector.hpp"

namespace PyROL {
inline Teuchos::RCP<ROL::Vector<double>> 
PyObject_AsVector( PyObject *pyObj ) {

  Teuchos::RCP<ROL::Vector<double>> vec;
#ifdef ENABLE_NUMPY
  // Check to see if this is a NumPy array
  if( PyObject_HasAttrString(pyObj,"__array_interface__") ) {
    vec = Teuchos::rcp( new NumPyVector(pyObj) );
  }
  else {
    vec = Teuchos::rcp( new PythonVector(pyObj) ) ;
  }
#else 
    vec = Teuchos::rcp( new PythonVector(pyObj) ) ;
#endif 
  return vec;
}

// Returns borrowed reference. TODO type deduction
template<class V=BaseVector>
PyObject* PyObject_FromVector( ROL::Vector<double> &vec ) {
  return Teuchos::dyn_cast<V>(vec).getPyVector();  
}

// Returns borrowed reference. TODO type deduction
template<class V=BaseVector>
const PyObject* PyObject_FromVector( const ROL::Vector<double> &vec ) {
  return Teuchos::dyn_cast<const V>(vec).getPyVector();  
}

enum PyOBJECT_TYPE : short {
  PyDICT, PyLIST, PySTRING, PyLONG, PyFLOAT, PyDOUBLE, PyBOOL, PyUNDEFINED
};


inline PyOBJECT_TYPE getPyObjectType( PyObject *pyObj ) {

  PyOBJECT_TYPE type;

  if( PyDict_Check(pyObj) )            { type = PyDICT;      }
  else if ( PyString_Check(pyObj) )    { type = PySTRING;    }
  else if( PyBool_Check(pyObj) )       { type = PyBOOL;      }
  else if( PyList_Check(pyObj) )       { type = PyLIST;      }
  else if( PyLong_Check(pyObj) )       { type = PyLONG;      }
  else if( PyFloat_Check(pyObj) )      { type = PyFLOAT;     }
  else                                 { type = PyUNDEFINED; }
  return type;
}

inline std::string pyObjectTypeAsString( PyOBJECT_TYPE type ) {
  switch(type) {
    case PyDICT:
      return "dict";
    case PyLIST:
      return "list";
    case PySTRING:
      return "string";
    case PyLONG:
      return "long";
    case PyFLOAT:
      return "float";
    case PyBOOL:
      return "bool";
    default:
      return "undefined"; 
  }
}


inline void dictToParameterList( PyObject* pyDict, 
                          Teuchos::ParameterList &parlist ) {

  // Get list of dictionary keys
  PyObject* pyKeyList = PyDict_Keys(pyDict);
 
  // Get number of dictionary keys
  Py_ssize_t len = PyList_Size(pyKeyList);

  // Loop over keys
  for( Py_ssize_t i=0; i<len; ++i ) {

    // Get current key
    PyObject *pyKey = PyList_GetItem(pyKeyList,i);
    
    // Determine
    PyOBJECT_TYPE keyType = getPyObjectType(pyKey);
   
    TEUCHOS_TEST_FOR_EXCEPTION(keyType!=PySTRING, std::invalid_argument,
      ">>> ERROR in addPairToParameterList(). "
      "Encountered key of unsupported type: " << pyObjectTypeAsString(keyType) 
      << std::endl);

    // FIXME Python 3 compatibility
//    PyObject* pyKeyString = PyUnicode_AsEncodedString(pyKey,"ASCII","strict");
    char* keyString = PyBytes_AsString(pyKey);
//    Py_XDECREF(pyKeyString);  
 
    PyObject* pyValue = PyDict_GetItem(pyDict,pyKey);
    PyOBJECT_TYPE valueType = getPyObjectType(pyValue);
  
    // Determine value type and write the key:value pair to the ParameterList
    switch( valueType ) {
      case PySTRING: {
//        PyObject* pyValueString = PyUnicode_AsEncodedString(pyValue,"ASCII","strict");
        char* valueString = PyBytes_AsString(pyValue);
        parlist.set(keyString,valueString);
//        Py_XDECREF(pyValueString); 
      }     
      break;

      case PyBOOL: {
        if(PyObject_IsTrue(pyValue)) {
          parlist.set(keyString,true);
        } else {
          parlist.set(keyString,false);
        }
      }
      break;

      case PyLONG: {
        long valueLong = PyLong_AsLong(pyValue);
        parlist.set(keyString,valueLong);
      }
      break;

      case PyFLOAT: {
        double valueDouble = PyFloat_AsDouble(pyValue);
        parlist.set(keyString,valueDouble);
      }
      break;
	
      case PyDICT: { // Recursively call this function
        Teuchos::ParameterList &sublist = parlist.sublist(keyString);
        dictToParameterList(pyValue,sublist);
      }
      break;

      case PyLIST:
      case PyDOUBLE:
      case PyUNDEFINED: {
        TEUCHOS_TEST_FOR_EXCEPTION(true, std::invalid_argument,
          ">>> ERROR in addPairToParameterList(). "
          "Encountered value of unsupported type: " << pyObjectTypeAsString(valueType) 
          << std::endl);
      }  
      break;
    } // switch( valueType )
    Py_XDECREF(pyValue);
  } // for(i...)

  
  Py_XDECREF(pyKeyList);

}




        
} // namespace PyROL


#endif // PYROL_TYPECONVERTERS_HPP

