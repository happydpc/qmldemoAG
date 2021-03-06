/**********************************************************************************
**
** Copyright (C) 1994 Narvik University College
** Contact: GMlib Online Portal at http://episteme.hin.no
**
** This file is part of the Geometric Modeling Library, GMlib.
**
** GMlib is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** GMlib is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with GMlib.  If not, see <http://www.gnu.org/licenses/>.
**
**********************************************************************************/
#include "C:/STE6245/gmlib-50e676222977fbf777a43302e74828ed87558300/gmlib.git/modules/scene/src/visualizers/gmselectorgridvisualizer.h"
#include "C:/STE6245/gmlib-50e676222977fbf777a43302e74828ed87558300/gmlib.git/modules/parametrics/src/curves/gmpsubcurve.h"


namespace GMlib {


//*****************************************
// Constructors and destructor           **
//*****************************************
template<typename T>
GMlib::MyBCurve<T>::MyBCurve(PCurve<T,3> *c, int n)
{
    _d = 1;//degree is 1, means we have
    _s  = c->getParStart();//start
    _e = c->getParEnd();//end


    if (c->isClosed()){
        _isclosed = true;
        n++;//number of subcurves,knots, for closed we need at least n+1 knots
    }
    else {
        _isclosed = false;
    }

    _makeKnotVector(n);
    _createLocalCurves(c,n);
}

  template <typename T>
  inline
  MyBCurve<T>::MyBCurve( const MyBCurve<T>& copy ) : PCurve<T,3>(copy) {}


  template <typename T>
  MyBCurve<T>::~MyBCurve() {}


  //**************************************
  //        Public local functons       **
  //**************************************


  //***************************************************
  // Overrided (public) virtual functons from PCurve **
  //***************************************************

  template <typename T>
  bool MyBCurve<T>::isClosed() const {
    return _isclosed;
  }


  //******************************************************
  // Overrided (protected) virtual functons from PCurve **
  //******************************************************

  template <typename T>
  void MyBCurve<T>::eval( T t, int d, bool /*l*/ ) const {

    this->_p.setDim( d + 1 );
    int i = _findIndex(t);

    const T b1 = 1-_B(_W(i,1,t));
    const T b2 = _B(_W(i,1,t));//computed equally for both cases. For closed with the same indices as for the references to the local curves

//                 local curves
    this->_p = b1*_C[i-1]->evaluateParent(t,0) + b2*_C[i]->evaluateParent(t,0);
  }


  template <typename T>
  T MyBCurve<T>::getStartP() const {
    return _t(_d);
  }


  template <typename T>
  T MyBCurve<T>::getEndP()const {
      return _t(_C.getDim());
  }

  template<typename T>
  T MyBCurve<T>::_W(int i, int d, T t) const
  {
      return ((t - _t(i))/(_t(i+d)-_t(i)));

  }

  template<typename T>
  int MyBCurve<T>::_findIndex(T t) const //find index of ti in the knot vector, which term of the sum is different from 0
  {
    int i=_d;
    int n = _C.getDim();
    for(;i<=n;i++){
        if(t>=_t(i) && t<_t(i+1))
            break;
    }
    if (i >= n){
        i=n-1;
    }
    return i;
  }

  template<typename T>
  void MyBCurve<T>::_makeKnotVector(int n)
  {
      auto local_d = (_e-_s)/(n-1);

      _t.setDim(n+_d+1);
      _t[0] = _t[1] = _s;

      for(int i = 0;i<n;i++){
          _t[i+2] = _s+(i+1)*local_d;
      }

      _t[n] = _t[n+1] = _e;

      if (_isclosed){
            // two first and two last knot intervals must be equal, it must be kept if a knot value is changed
          _t[0] = _t[1] - (_t[n] - _t[n-1]);
          _t[n+1] = _t[n] + (_t[2] - _t[1]);
      }

  }

  template<typename T>
  void MyBCurve<T>::_createLocalCurves(PCurve<T,3> *c, int n)
  {
      _C.setDim(n);
      for(int i=0;i<n-1;i++){
          auto cu = new PSubCurve<T>(c,_t[i],_t[i+2],_t[i+1]);
          cu->toggleDefaultVisualizer();
          cu->replot(21,0);
          cu->setCollapsed(true); //collapse subcurves so not to see them on the scene
          _C[i] = cu;
          this->insert(cu);
      }
      if (_isclosed){
          _C[n-1] = _C[0];
      }
      else {
          auto cu = new PSubCurve<T>(c,_t[n-1],_t[n+1],_t[n]);
          cu->toggleDefaultVisualizer();
          cu->replot(21,0);
          cu->setCollapsed(true);
          _C[n-1] = cu;
          this->insert(cu);
      }

  }

  template<typename T>
  T MyBCurve<T>::_B(T t) const
  {
      return 3*(t*t) - 2*(t*t*t);
  }

  template<typename T>
  void GMlib::MyBCurve<T>::localSimulate(double dt)
  {

      for (int i = 0;i<_C.getDim();i++){
          _C[i]->rotate(dt,GMlib::Vector<float,3>(1,1,0));
      }

  }




} // END namespace GMlib
