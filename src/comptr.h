/* 
** COM÷«ƒ‹÷∏’Î
** author
**   taoabc@gmail.com
*/

#ifndef ULT_COMPTR_H_
#define ULT_COMPTR_H_

#include <Windows.h>

namespace ult {

template <typename T>
bool IsNull(const T& p) {
  return (p == NULL || p == nullptr);
}

template <typename T>
class ComPtr {

public:

  ComPtr(void) :
      p_(nullptr) {
  }

  ComPtr(T* p) {
    p_ = p;
    if (!IsNull(p_)) {
      p_->AddRef();
    }
  }

  ComPtr(const ComPtr<T>& p) {
    p_ = p.p_;
    if (!IsNull(p_)) {
      p_->AddRef();
    }
  }

  ~ComPtr(void) {
    if (!IsNull(p_)) {
      p_->Release();
    }
  }

  void Release(void) {
    if (!IsNull(p_)) {
      p_->Release();
      p_ = nullptr;
    }
  }

  operator T*(void) const {
    return static_cast<T*>(p_);
  }

  T* Get(void) const {
    return static_cast<T*>(p_);
  }

  T** operator&(void) {
    return &p_;
  }

  T* operator->(void) const {
    return p_;
  }

  T* operator=(T* p) {
    if (!IsNull(p)) {
      p->AddRef();
    }
    if (!IsNull(p_)) {
      p_->Release();
    }
    p_ = p;
    return p_;
  }

  T* operator=(const ComPtr<T>& p) {
    return (*this = p.p_);
  }

  bool operator!(void) const {
    return IsNull(p_);
  }

  operator bool(void) const {
    return !IsNull(p_);
  }

  void Attach(T* p) {
    Release();
    p_ = p;
  }

  T* Detach(void) {
    T* p = p_;
    p_ = NULL;
    return p;
  }

  template <typename Tp>
  HRESULT QueryInterface(REFIID riid, Tp** ppobj) const {
    return p_->QueryInterface(riid, (void**)ppobj);
  }

private:

  T* p_;
}; // class ComPtr

template <typename T1, typename T2>
bool operator==(const ComPtr<T1>& p1, const ComPtr<T2>& p2) {
  return p1.Get() == p2.Get();
}

} //namespace ult

#endif