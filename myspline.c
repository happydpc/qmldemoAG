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


namespace GMlib {


//*****************************************
// Constructors and destructor           **
//*****************************************

  template <typename T>
  inline
  MSpline<T>::MSpline(const DVector<Vector<T,3>> &c, int d) {
    _d = d;
    _makeKnotVector(c.getDim());
    _C = c;
    for (int i=0;i<_C.getDim();i++){
        Selector<T,3>* s = new Selector<T,3>(_C[i],i,this);
        this->insert(s);

    }
    auto sk = new SelectorGridVisualizer<T>;
    sk->setSelectors(_C,0,isClosed());
    this->insertVisualizer(sk);

  }

  template <typename T>
  inline               //p vector of points sampled from the original curve, n number of control points we want
  MSpline<T>::MSpline(const DVector<Vector<T,3>> &p, int d, int n) {
      _d = d;//dimension
      _makeKnotVector(n);
      _createControlPoints(p,n);//surfaces

      auto sk = new SelectorGridVisualizer<T>;
      sk->setSelectors(_C,0,isClosed());
      this->insertVisualizer(sk);
  }

  template <typename T>
  inline
  MSpline<T>::MSpline( const MSpline<T>& copy ) : PCurve<T,3>(copy) {}


  template <typename T>
  MSpline<T>::~MSpline() {}


  //**************************************
  //        Public local functons       **
  //**************************************


  //***************************************************
  // Overrided (public) virtual functons from PCurve **
  //***************************************************

  template <typename T>
  bool MSpline<T>::isClosed() const {
    return false;
  }


  //******************************************************
  // Overrided (protected) virtual functons from PCurve **
  //******************************************************

  template <typename T>
  void MSpline<T>::eval( T t, int d, bool /*l*/ ) const {

    this->_p.setDim( d + 1 );
    int i = _findIndex(t);//parameter
    const T b1 = (1-_W(i,1,t))*(1-_W(i-1,2,t));//basis functions
    const T b2 = ((1-_W(i,1,t))*_W(i-1,2,t))+(_W(i,1,t)*(1-_W(i,2,t)));
    const T b3 = (_W(i,1,t)*_W(i,2,t));


    this->_p[0] = _C[i-2]*b1 + _C[i-1]*b2 + _C[i]*b3;

  }


  template <typename T>
  T MSpline<T>::getStartP() const {
    return _t(_d);//domain for the curve
  }


  template <typename T>
  T MSpline<T>::getEndP()const {
      return _t(_C.getDim());
  }

  template<typename T>
  T MSpline<T>::_W(int i, int d, T t) const
  {
      return ((t - _t(i))/(_t(i+d)-_t(i)));
  }

  template<typename T>
  int MSpline<T>::_findIndex(T t) const
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
  void MSpline<T>::_makeKnotVector(int n)
  {

      _t.setDim(n+_d+1);//order = degree + 1

        for(int i = 0;i<=_d;i++){
            _t[i] = 0;
        }
        for(int i=_d+1;i<= n;i++){
            _t[i] = i-_d;
        }
        for(int i=n+1;i<=n+_d;i++){
            _t[i] = _t[i-1];
        }
  }

  template<typename T>
  void MSpline<T>::_createControlPoints(const DVector<Vector<T, 3> > &p, int n)
  {
      int m = p.getDim();//number of control points for basis
      _C.setDim(n);//number of control points we are looking for
      DMatrix<T> A(m,n);
      for (int i = 0; i<m; i++){
          for (int j = 0; j<n;j++){
              A[i][j] = T(0);//fill with zeros
          }
      }
      for (int i = 0;i<m;i++){
          T t = _t[0]+i*(getEndP()-getStartP())/(m-1);//getParDelta, define ti, for the control points pi use them to evaluate c[ti] = pi


          int j = _findIndex(t); //recalculate basis functions for every ti
          const T b1 = (1-_W(j,1,t))*(1-_W(j-1,2,t));
          const T b2 = ((1-_W(j,1,t))*_W(j-1,2,t))+(_W(j,1,t)*(1-_W(j,2,t)));
          const T b3 = (_W(j,1,t)*_W(j,2,t));

          A[i][j-2] = b1;
          A[i][j-1] = b2;
          A[i][j] = b3; //set values into the matrix

      }
      DMatrix<T> Atrans = A;
      Atrans.transpose();
      DMatrix<T> B = Atrans*A;//apply least square
      B.invert();
      DVector<Vector<T,3>> x = Atrans*p; //vector of new control points
      _C = B*x;//new control points vector for our curve
      for(int i=0;i<_C.getDim();i++){
          Selector<T,3>* s = new Selector<T,3>(_C[i],i,this);
          this->insert(s);
      }


  }


} // END namespace GMlib
