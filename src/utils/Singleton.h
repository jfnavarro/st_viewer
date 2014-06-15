/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef SINGLETON_H
#define SINGLETON_H

template <class T>
class Singleton
{

protected:

    Singleton() {}
    virtual ~Singleton() {}

private:

    static T* _instance;

    Singleton(Singleton const&); // prevent assignment
    Singleton& operator=(Singleton const&); // prevent copy

public:

    static T* getInstance(bool release = false)
    {
        if (release) {
            if (_instance) {
                delete _instance;
                _instance = 0;
            }
        } else if (_instance == 0) {
            _instance = new T();
        }
        return _instance;
    }

    template <class C> static T* getInstance(C val)
    {
        if (_instance == 0) {
            _instance = new T(val);
        }
        return _instance;
    }

    template <class C, class M> static T* getInstance(C val1, M val2)
    {
        if (_instance == 0) {
            _instance = new T(val1, val2);
        }
        return _instance;
    }

};

template <class T>  T* Singleton<T>::_instance = 0;

#endif // SINGLETON_H
