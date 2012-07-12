/**
 *  Copyright (C) 2012  Stefan Radomski (stefan.radomski@cs.tu-darmstadt.de)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the FreeBSD license as published by the FreeBSD
 *  project.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  You should have received a copy of the FreeBSD license along with this
 *  program. If not, see <http://www.opensource.org/licenses/bsd-license>.
 */

#include "umundo/util/Regex.h"

namespace umundo {

Regex::Regex(const string& pattern) {
  setPattern(pattern);
}

void Regex::setPattern(const string& pattern) {
  _pattern = pattern;
  _error.clear();

  const char *error;  
  _re = pcre_compile(_pattern.c_str(),    // the pattern
                          0,              // default options
                          &error,         // error message
                          &_errorOffset,  // error offset
                          NULL);          // default character table
  
  if (_re == NULL) {
    _error = error;
  }
}

bool Regex::hasError() {
  return _error.size() > 0;
}

bool Regex::matches(const string& text) {
  _error.clear();
  _matchIndex = std::make_pair(0, 0);
  _subMatchIndex.clear();
  
  if (_re == NULL)
    return false;

  int rc = pcre_exec(_re,           // the compiled pattern
                     NULL,          // no extra data from study
                     text.c_str(),  // subject string
                     text.length(), // length of subject
                     0,             // offset to start at
                     0,             // default options
                     _ovector,      // output vector for substrings
                     OVECCOUNT);    // numbrer of elements in output vector
  
  if (rc < 0) { // error occured
    switch (rc) {
      case PCRE_ERROR_NOMATCH:
        break;
      case PCRE_ERROR_NULL:
        _error = "PCRE_ERROR_NULL";
        break;
      case PCRE_ERROR_BADOPTION:
        _error = "PCRE_ERROR_BADOPTION";
        break;
      case PCRE_ERROR_BADMAGIC:
        _error = "PCRE_ERROR_BADMAGIC";
        break;
      case PCRE_ERROR_UNKNOWN_NODE:
        _error = "PCRE_ERROR_UNKNOWN_NODE";
        break;
      case PCRE_ERROR_NOMEMORY:
        _error = "PCRE_ERROR_NOMEMORY";
        break;
      case PCRE_ERROR_NOSUBSTRING:
        _error = "PCRE_ERROR_NOSUBSTRING";
        break;
        
      default:
        break;
    }
    return false;    
  }
  
  if (rc == 0) { // output vector too small
    rc = OVECCOUNT/3;
    _error = "Output vector too small";
    return false;
  }
  
  // copy match index
  _matchIndex = std::make_pair(_ovector[0], _ovector[1] - _ovector[0]);
  
  // copy indices of submatches
  if (rc > 1) {
    for (int i = 1; i < rc; i++) {
      _subMatchIndex.push_back(std::make_pair(_ovector[2*i], _ovector[2*i+1] - _ovector[2*i]));
    }
  }
  
  return true;
}

}