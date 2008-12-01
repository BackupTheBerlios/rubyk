#include "values.h"

size_t Value::from_string(const std::string& p)
{
  unsigned int size = p.size();
  unsigned int pos = 0;
  unsigned int value_end;
  Hash h;
  
  switch (p[0]) {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '0':
      // Number
      pos = p.find(' ',pos);
      value_end = (pos == std::string::npos) ? size : pos;
      Number(p.substr(0, value_end)).set(*this);
      return value_end;
    case '"':
      // String
      pos++;
      value_end = p.find('"',pos); // FIXME: parse escaped \"
      if (pos == std::string::npos) {
        gNilValue.set(*this);
        return size; // bad format, abort
      }
      String(p.substr(pos, value_end - pos)).set(*this);
      return value_end + 1;
    case '{':
      // Hash
      pos++;
      pos += h.mutable_data()->build_hash(p.substr(pos, size - pos));
      h.set(*this);
      return pos;
      // FIXME: matrix, error, nil, bang, etc
    default:
      return size; // abort, error
  }
}

std::ostream& operator<< (std::ostream& pStream, const Value& val)
{
  if (val.mPtr && val.mPtr->mDataPtr) {
    val.mPtr->mDataPtr->to_stream(pStream);
  } else {
    pStream << "Nil";
  }
  return pStream;
}


/** Display number inside stream. */
template<>
void MatrixData::to_stream(std::ostream& pStream) const
{
  char buffer[20];
  if (size() == 0) {
#ifdef _TESTING_
    pStream << "<" << type_name() << "[" << mId << "] 0>";
#else
    pStream << "<" << type_name() << " 0>";
#endif
  } else {
    size_t sz = 16;
    size_t start;
    if (sz > size()) sz = size();
    start = size() - sz;
#ifdef _TESTING_
    snprintf(buffer, 20, "<%s[%lu] [ % .2f", type_name(), mId, data[start]);
#else
    snprintf(buffer, 20, "<%s [ % .2f", type_name(), data[start]);
#endif
    pStream << buffer;
    for (size_t i= start+1; i < start+sz; i++) {
      snprintf(buffer, 20, " % .2f", data[i]);
      pStream << buffer;
    }
    pStream << " ], " << mRowCount << "x" << mColCount << ">";
  }
}

/** Display number inside stream. */
template<>
void CharMatrixData::to_stream(std::ostream& pStream) const
{
  if (size() == 0) {
#ifdef _TESTING_
    pStream << "<" << type_name() << "[" << mId << "] 0>";
#else
    pStream << "<" << type_name() << " 0>";
#endif
  } else {
    size_t sz = 16;
    size_t start;
    if (sz > size()) sz = size();
    start = size() - sz;
#ifdef _TESTING_
    pStream << "<" << type_name() << "[" << mId << "]" << " [ " << data[start];
#else
    pStream << "<" << type_name() << "[" << mId << "]" << " [ " << data[start];
#endif
    for (size_t i= start+1; i < start+sz; i++) {
      pStream << " " << data[i];
    }
    pStream << " ], " << mRowCount << "x" << mColCount << ">";
  }
}