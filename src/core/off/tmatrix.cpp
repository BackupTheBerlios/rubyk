/*
  ==============================================================================

   This file is part of the RUBYK project (http://rubyk.org)
   Copyright (c) 2007-2009 by Gaspard Bucher - Buma (http://teti.ch).

  ------------------------------------------------------------------------------

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.

  ==============================================================================
*/

#include "tmatrix.h"
#include "values.h"

#include <errno.h>     // Error number definitions
#include <cstdlib>
#include <cstdio>


#define BUF_INITIAL_SIZE 16
#define BUF_START_ERROR_MSG_SIZE 80

/** Make a partial copy of another matrix starting at a specific row index. The size of the matrix automatically
  * grows to receive the copied data.
  * to copy the second row into the last row, you would write:
  * copy_at(-1, matrix, 1, 1)
  *
  * to copy the third line into the first row:
  * copy_at(0, matrix, 2, 2)
  *
  * to copy the last (most recent) 3 lines of matrix at the beginning:
  * copy_at(0, matrix, -3)
  *
  * @param pRowIndex   index of the row to start copying data into.
  * @param pOther      other matrix to copy the data from.
  * @param pStartRow   where to start copying the data from (default is 0).
  * @param pEndRow     last row to copy (default is -1 = last row).
  *
  * @return bool       returns false if allocation of new space failed. */
template<typename T>
bool TMatrix<T>::copy_at(const int pRowIndex, const TMatrix& pOther, int pStartRow, int pEndRow, bool pResize)
{
  size_t row_index = pRowIndex < 0 ? mRowCount + pRowIndex : pRowIndex;
  size_t start_row, end_row;
  if (!check_sizes("copy", &start_row, &end_row, pOther, pStartRow, pEndRow, pRowIndex == 0)) return false;

  if (row_index > mRowCount || row_index < 0) {
    // copy can start at most at the end of the current matrix
    set_error("cannot start copying at %i in matrix %ix%i", pRowIndex, mRowCount, mColCount);
    return false;
  }
  mColCount = pOther.mColCount;
  return raw_copy(row_index, pOther.data + start_row * mColCount, (end_row - start_row + 1) * mColCount, pResize && pRowIndex == 0);
}

template<typename T>
bool TMatrix<T>::from_file(FILE * pFile)
{
  bool read_all = mRowCount == 0;
  size_t start_i = 0;
  char c;

  // if mColCount == 0, get row_size from first row
  if (!mColCount) {
    TMatrix tmp;
    T val;
    tmp.set_sizes(0,0);

    while(do_fscanf(pFile, &val) > 0) {
      tmp.append(val);
      if ( (c=getc(pFile)) == '\n') {
        break;
      } else {
        ungetc(c, pFile);
      }
    }
    if(!copy(tmp)) return false;
    mColCount = size();
    mRowCount = 1;
    start_i = 1;
  }

  if (read_all) {
    TMatrix tmp;
    if(!tmp.set_sizes(1, mColCount)) {
      set_error("tmp (%s)", tmp.error_msg());
      return false;
    }
    while (true) {
      for(size_t j=0; j < mColCount; j++) {

        if ( (c=getc(pFile)) == '\n') {
          // end of line
          if ( (c=getc(pFile)) == '\n') {
            // end of matrix
            if (j == 0) {
              // ok.
              return true;
            } else {
              set_error("end of matrix in middle of vector %i", mRowCount + 1);
              return false;
            }
          } else
            ungetc(c, pFile);
        } else
          ungetc(c, pFile);

        if(do_fscanf(pFile, tmp.data + j) == EOF) {
          if (j == 0) {
            // ok. end of file
            return true;
          } else {
            set_error("end of file in middle of vector %i", mRowCount + 1);
            return false;
          }
        }
      }
      if (!append(tmp)) {
        set_error("could not append vector %i", mRowCount + 1);
        return false;
      }
    }
  } else {
    for(size_t i=start_i; i < mRowCount; i++) {
      for(size_t j=0; j < mColCount; j++) {
        if(do_fscanf(pFile, data + i * mColCount + j) == EOF) {
          set_error("end of file while reading value %i,%i out of %ix%i", i+1, j+1, mRowCount, mColCount);
          return false;
        }
        fscanf(pFile, "\n"); // ignore newline
      }
    }
  }

  return true;
}


/** Read a matrix from a filepath. */
template<typename T>
bool TMatrix<T>::from_file(const std::string &pPath, const char * pMode)
{
  FILE * file = fopen(pPath.c_str(), pMode);
    if (!file) {
      set_error("could not read from '%s' (%s)", pPath.c_str(), strerror(errno));
      return false;
    }
    if (!from_file(file)) {
      fclose(file);
      return false;
    }
  fclose(file);
  return true;
}

/** Write a matrix to a filepath. */
template<typename T>
bool TMatrix<T>::to_file(const std::string &pPath, const char * pMode, bool isMatrix) const
{
  FILE * file = fopen(pPath.c_str(), pMode);
    if (!file) {
      // too bad, we need this to be 'const' so we cannot use set_error...
      // FIXME: remove const and use CutMatrix (maybe rename to ViewMatrix)
      // set_error("could not write to '%s' (%s)", pPath.c_str(), strerror(errno));
      return false;
    }
    if (!to_file(file, isMatrix)) {
      fclose(file);
      return false;
    }
  fclose(file);
  return true;
}

template<typename T>
bool TMatrix<T>::to_file(FILE * pFile, bool isMatrix) const
{
  for(size_t i=0; i < mRowCount; i++) {
    for(size_t j=0; j < mColCount; j++) {
      do_fprintf(pFile, data[i * mColCount + j]);
    }
    fprintf(pFile, "\n");
  }
  if (isMatrix)
    fprintf(pFile, "\n");  // two \n\n between matrix
  return true;
}

template<>
void TMatrix<real_t>::print(FILE * pFile) const
{
  size_t i,j;

  for(i=0;i<mRowCount;i++) {
    if (i==0)
      fprintf(pFile, "[");
    else
      fprintf(pFile, " ");
    for(j=0;j<mColCount;j++)
        fprintf(pFile, " % .2f",data[(i * mColCount) + j]);
    if (i == mRowCount -1)
      fprintf(pFile, " ]\n");
    else
      fprintf(pFile, "\n");
  }
}

template<>
void TMatrix<int>::print(FILE * pFile) const
{
  size_t i,j;

  for(i=0;i<mRowCount;i++) {
    if (i==0)
      fprintf(pFile, "[");
    else
      fprintf(pFile, " ");
    for(j=0;j<mColCount;j++)
        fprintf(pFile, " %i",data[(i * mColCount) + j]);
    if (i == mRowCount -1)
      fprintf(pFile, " ]\n");
    else
      fprintf(pFile, "\n");
  }
}

template<>
void TMatrix<char>::print(FILE * pFile) const
{
  size_t i,j;

  for(i=0;i<mRowCount;i++) {
    if (i==0)
      fprintf(pFile, "[");
    else
      fprintf(pFile, " ");
    for(j=0;j<mColCount;j++)
        fprintf(pFile, " %i",(int)data[(i * mColCount) + j]);
    if (i == mRowCount -1)
      fprintf(pFile, " ]\n");
    else
      fprintf(pFile, "\n");
  }
}

template<>
std::ostream& operator<< (std::ostream& pStream, const TMatrix<real_t>& mat)
{
  char buf[200];
  size_t row_count = mat.row_count();
  size_t col_count = mat.col_count();
  size_t i,j;

  for(i=0;i<row_count;i++) {
    if (i==0)
      pStream << "[";
    else
      pStream << " ";
    for(j=0;j<col_count;j++) {
      snprintf(buf, 200, " % .2f", mat.data[(i * col_count) + j]);
      pStream << buf;
    }
    if (i == row_count -1)
      pStream << " ]\n";
    else
      pStream << "\n";
  }
  return pStream;
}

template<>
std::ostream& operator<< (std::ostream& pStream, const TMatrix<int>& mat)
{
  char buf[200];
  size_t row_count = mat.row_count();
  size_t col_count = mat.col_count();
  size_t i,j;

  for(i=0;i<row_count;i++) {
    if (i==0)
      pStream << "[";
    else
      pStream << " ";
    for(j=0;j<col_count;j++) {
      snprintf(buf, 200, " %i", mat.data[(i * col_count) + j]);
      pStream << buf;
    }
    if (i == row_count -1)
      pStream << " ]\n";
    else
      pStream << "\n";
  }
  return pStream;
}

template<>
std::ostream& operator<< (std::ostream& pStream, const TMatrix<char>& mat)
{
  char buf[200];
  size_t row_count = mat.row_count();
  size_t col_count = mat.col_count();
  size_t i,j;

  for(i=0;i<row_count;i++) {
    if (i==0)
      pStream << "[";
    else
      pStream << " ";
    for(j=0;j<col_count;j++) {
      snprintf(buf, 200, " %i", (int)mat.data[(i * col_count) + j]);
      pStream << buf;
    }
    if (i == row_count -1)
      pStream << " ]\n";
    else
      pStream << "\n";
  }
  return pStream;
}

/** Append a vector to the end of the current data. Size increases automatically. */
template<typename T>
bool TMatrix<T>::append(const T * pVector, size_t pVectorSize)
{
  //if (pVectorSize % mColCount != 0) {
  //  set_error("could not append vector: column count not matching (%i is not a multiple of %i)", pVectorSize, mColCount);
  //  return false;
  //}
  return raw_copy(mRowCount, pVector, pVectorSize);
}

/** Append a value at the end of a vector. Size increases automatically. */
template<typename T>
bool TMatrix<T>::append(T pValue)
{
  if (mRowCount > 1) {
    set_error("could not append: matrix is not a vector (%ix%i)", mRowCount, mColCount);
    return false;
  }
  mRowCount = 1;
  if(!check_alloc(mColCount + 1)) return false;
  data[mColCount] = pValue;
  mColCount++;
  return true;
}

/** Append another matrix/vector to the end of the current data. Size increases automatically.
  * @return false if the column count of both matrices do not match. */
template<typename T>
bool TMatrix<T>::append(const TMatrix& pOther, int pStartRow, int pEndRow)
{
  return copy_at(mRowCount, pOther, pStartRow, pEndRow);
}

/** Add elements of one matrix to another.
  * If rows/columns match, elements are added one by one.
  * If the other matrix is a vector and columns sizes match, the vector is added to each row.
  * If the other matrix is a scalar, add the value to all elements.
  *
  * @param pOther other matrix whose elements will be added.
  * @param pStartRow if you want to use only part of the other matrix, start row. Default 0 (first row).
  * @param pEndRow   when using only part of the other matrix. Default -1 (last row).
  *
  * @return true (never fails). */
template<typename T>
bool TMatrix<T>::add(const TMatrix& pOther, int pStartRow, int pEndRow, Real pScale)
{
  size_t start_row, end_row;
  if (!check_sizes("add", &start_row, &end_row, pOther, pStartRow, pEndRow, true)) return false;
  size_t row_count = end_row - start_row + 1;
  T * other_data = pOther.data + start_row * pOther.mColCount;

  if (pOther.mColCount == mColCount) {
    if (row_count == mRowCount) {
      // one to one
      if (pScale == 1.0)
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++) // FIXME: if we change realloc/malloc to align, we could write j+=4/8, with SSE optimization
            data[i * mColCount + j] += other_data[i * mColCount + j];

      else if (pScale == -1.0)
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++)
            data[i * mColCount + j] -= other_data[i * mColCount + j];

      else
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++)
            data[i * mColCount + j] += pScale * other_data[i * mColCount + j];

    } else if (row_count == 1) {
      // vector
      if (pScale == 1.0)
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++) // FIXME: if we change realloc/malloc to align, we could write j+=4/8, with SSE optimization
            data[i * mColCount + j] += other_data[j];

      else if (pScale == -1.0)
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++)
            data[i * mColCount + j] -= other_data[j];

      else
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++)
            data[i * mColCount + j] += pScale * other_data[j];

    } else {
      // bad size
      set_error("size error (add): source matrix %ix%i, target matrix %ix%i (bad vector count)", row_count, pOther.mColCount, mRowCount, mColCount);
      return false;
    }
  } else if (pOther.mColCount == 1 && row_count == mRowCount) {
    // different value for each row
    if (pScale == 1.0)
      for(size_t i=0; i < mRowCount; i++)
        for(size_t j=0; j < mColCount; j++) // FIXME: if we change realloc/malloc to align, we could write j+=4/8, with SSE optimization
          data[i * mColCount + j] += other_data[i];

    else if (pScale == -1.0)
      for(size_t i=0; i < mRowCount; i++)
        for(size_t j=0; j < mColCount; j++)
          data[i * mColCount + j] -= other_data[i];

    else
      for(size_t i=0; i < mRowCount; i++)
        for(size_t j=0; j < mColCount; j++)
          data[i * mColCount + j] += pScale * other_data[i];

  } else if (pOther.mColCount == 1 && row_count == 1) {
    // scalar
    return *this += pScale * other_data[0];
  } else {
    // bad size
    set_error("size error (add): source matrix %ix%i, target matrix %ix%i (incomptable)", row_count, pOther.mColCount, mRowCount, mColCount);
    return false;
  }
  return true;
}


/** Add an array of reals to each elements in the matrix.
  * If the size is the same as the matrix : one to one.
  * If the size is col_size : add to each row.
  * If the size is row_size : add corresponding value to element in the row. */
template<typename T>
bool TMatrix<T>::add(const T * pVector, size_t pVectorSize)
{
  size_t sz = size();
  if (pVectorSize == sz) {
    // one to one
    for(size_t i=0; i < sz; i++)
        data[i] += pVector[i];
  } else if (pVectorSize == mColCount) {
    // vector for each row
    for(size_t i=0; i < mRowCount; i++)
      for(size_t j=0; j < mColCount; j++)
          data[i * mColCount + j] += pVector[j];
  } else if (pVectorSize == mRowCount) {
    // 1 vector value for each row
    for(size_t i=0; i < mRowCount; i++)
      for(size_t j=0; j < mColCount; j++)
          data[i * mColCount + j] += pVector[i];
  } else {
    // fail
    set_error("size error (+=): cannot add vector %i to %ix%i", pVectorSize, mRowCount, mColCount);
    return false;
  }
  return true;
}

template<typename T>
bool TMatrix<T>::add (const TMatrix& A, const TMatrix& B, Real pScaleA, Real pScaleB)
{
  if (A.mRowCount != B.mRowCount || A.mColCount != B.mColCount) {
    set_error("size error (add): matrix A %ix%i, matrix B %ix%i (incompatible)", A.mRowCount, A.mColCount, B.mRowCount, B.mColCount);
    return false;
  }
  if (!set_sizes(A.mRowCount, A.mColCount)) return false;

  for(size_t i=0; i < mRowCount; i++) {
    size_t offset = i * mColCount;
    for(size_t j=0; j < mColCount; j++)
      data[offset + j] = pScaleA * A.data[offset + j] + pScaleB * B.data[offset + j];
  }
  return true;
}


/** Divide all elements by the values in another matrix.
  * If rows/columns match, elements are divided one by one.
  * If the other matrix is a vector and columns sizes match, each row is divided by the vector.
  * If the other matrix is a column vector and row counts match, corresponding rows are divided by the scalar.
  * If the other matrix is a scalar, divide all element by this value.
  *
  * @param pOther other matrix by which the elements of this matrix will be divided.
  * @param pStartRow if you want to use only part of the other matrix, start row. Default 0 (first row).
  * @param pEndRow   when using only part of the other matrix. Default -1 (last row).
  * @return true (never fails). */
template<typename T>
bool TMatrix<T>::divide(const TMatrix& pOther, int pStartRow, int pEndRow, Real pScale)
{
  size_t start_row, end_row;
  if (!check_sizes("divide", &start_row, &end_row, pOther, pStartRow, pEndRow, true)) return false;
  size_t row_count = end_row - start_row + 1;
  T * other_data = pOther.data + start_row * pOther.mColCount;

  if (pOther.mColCount == mColCount) {
    if (row_count == mRowCount) {
      // one to one
      if (pScale == 1.0)
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++) // FIXME: if we change realloc/malloc to align, we could write j+=4/8, with SSE optimization
            data[i * mColCount + j] /= other_data[i * mColCount + j];

      else if (pScale == -1.0)
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++)
            data[i * mColCount + j] /= -other_data[i * mColCount + j];

      else
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++)
            data[i * mColCount + j] /= pScale * other_data[i * mColCount + j];

    } else if (row_count == 1) {
      // vector
      if (pScale == 1.0)
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++) // FIXME: if we change realloc/malloc to align, we could write j+=4/8, with SSE optimization
            data[i * mColCount + j] /= other_data[j];

      else if (pScale == -1.0)
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++)
            data[i * mColCount + j] /= other_data[j];

      else
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++)
            data[i * mColCount + j] /= pScale * other_data[j];

    } else {
      // bad size
      set_error("size error (divide): source matrix %ix%i, target matrix %ix%i (bad vector count)", row_count, pOther.mColCount, mRowCount, mColCount);
      return false;
    }
  } else if (pOther.mColCount == 1 && row_count == mRowCount) {
    // different value for each row
    if (pScale == 1.0)
      for(size_t i=0; i < mRowCount; i++)
        for(size_t j=0; j < mColCount; j++) // FIXME: if we change realloc/malloc to align, we could write j+=4/8, with SSE optimization
          data[i * mColCount + j] /= other_data[i];

    else if (pScale == -1.0)
      for(size_t i=0; i < mRowCount; i++)
        for(size_t j=0; j < mColCount; j++)
          data[i * mColCount + j] /= other_data[i];

    else
      for(size_t i=0; i < mRowCount; i++)
        for(size_t j=0; j < mColCount; j++)
          data[i * mColCount + j] /= pScale * other_data[i];

  } else if (pOther.mColCount == 1 && row_count == 1) {
    // scalar
    return *this /= pScale * other_data[0];
  } else {
    // bad size
    set_error("size error (divide): source matrix %ix%i, target matrix %ix%i (incomptable)", row_count, pOther.mColCount, mRowCount, mColCount);
    return false;
  }
  return true;
}

/** Multiply all elements by the values in another matrix. a.divide(b) (a/b) is NOT the matrix multiplication (ab). See 'mat_multiply'. */
template<typename T>
bool TMatrix<T>::multiply(const TMatrix& pOther, int pStartRow, int pEndRow, Real pScale)
{
  size_t start_row, end_row;
  if (!check_sizes("multiply", &start_row, &end_row, pOther, pStartRow, pEndRow, true)) return false;
  size_t row_count = end_row - start_row + 1;
  T * other_data = pOther.data + start_row * pOther.mColCount;

  if (pOther.mColCount == mColCount) {
    if (row_count == mRowCount) {
      // one to one
      if (pScale == 1.0)
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++) // FIXME: if we change realloc/malloc to align, we could write j+=4/8, with SSE optimization
            data[i * mColCount + j] *= other_data[i * mColCount + j];

      else if (pScale == -1.0)
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++)
            data[i * mColCount + j] *= -other_data[i * mColCount + j];

      else
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++)
            data[i * mColCount + j] *= pScale * other_data[i * mColCount + j];

    } else if (row_count == 1) {
      // vector
      if (pScale == 1.0)
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++) // FIXME: if we change realloc/malloc to align, we could write j+=4/8, with SSE optimization
            data[i * mColCount + j] *= other_data[j];

      else if (pScale == -1.0)
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++)
            data[i * mColCount + j] *= other_data[j];

      else
        for(size_t i=0; i < mRowCount; i++)
          for(size_t j=0; j < mColCount; j++)
            data[i * mColCount + j] *= pScale * other_data[j];

    } else {
      // bad size
      set_error("size error (multiply): source matrix %ix%i, target matrix %ix%i (bad vector count)", row_count, pOther.mColCount, mRowCount, mColCount);
      return false;
    }
  } else if (pOther.mColCount == 1 && row_count == mRowCount) {
    // different value for each row
    if (pScale == 1.0)
      for(size_t i=0; i < mRowCount; i++)
        for(size_t j=0; j < mColCount; j++) // FIXME: if we change realloc/malloc to align, we could write j+=4/8, with SSE optimization
          data[i * mColCount + j] *= other_data[i];

    else if (pScale == -1.0)
      for(size_t i=0; i < mRowCount; i++)
        for(size_t j=0; j < mColCount; j++)
          data[i * mColCount + j] *= other_data[i];

    else
      for(size_t i=0; i < mRowCount; i++)
        for(size_t j=0; j < mColCount; j++)
          data[i * mColCount + j] *= pScale * other_data[i];

  } else if (pOther.mColCount == 1 && row_count == 1) {
    // scalar
    return *this *= pScale * other_data[0];
  } else {
    // bad size
    set_error("size error (multiply): source matrix %ix%i, target matrix %ix%i (incomptable)", row_count, pOther.mColCount, mRowCount, mColCount);
    return false;
  }
  return true;
}
////////////////  MATRIX OPERATIONS ////////////////////////

/** Matrix multiplication.
  * Write C.mat_multiply(A, B) for C = AB
  *
  * @param A matrix A.
  * @param B matrix B.
  * @param pTransA transposition mode for matrix A (CblasNoTrans/CblasTrans).
  * @param pTransB transposition mode for matrix B.
  * @param pScale  scale factor. Default is 1.0 (no scaling). */
template<typename T>
bool TMatrix<T>::mat_multiply(const TMatrix& A, const TMatrix& B, const enum CBLAS_TRANSPOSE pTransA, const enum CBLAS_TRANSPOSE pTransB, Real pScale)
{
  /** MxK  *  KxN */
  size_t m, k, n;
  if (pTransA == CblasNoTrans) {
    m = A.mRowCount;
    k = A.mColCount;
  } else {
    m = A.mColCount;
    k = A.mRowCount;
  }
  if (pTransB == CblasNoTrans) {
    if (B.mRowCount != k) {
      set_error("size error (%s): cannot multiply matrix %ix%i with matrix %ix%i", m, k, B.mRowCount, B.mColCount);
      return false;
    } else
      n = B.mColCount;
  } else if (B.mColCount != k) {
    set_error("size error (%s): cannot multiply matrix %ix%i with matrix %ix%i", m, k, B.mColCount, B.mRowCount);
    return false;
  } else
    n = B.mRowCount;

  if (!set_sizes(m,n)) return false;
  do_gemm(CblasRowMajor, pTransA, pTransB, m, n, k, pScale, A.data, A.mColCount, B.data, B.mColCount, 0.0, data, mColCount);
  return true;
}

template<>
void TMatrix<real_t>::do_gemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K, const Real alpha, const Real *A, const int lda, const Real *B, const int ldb, const Real beta, Real *C, const int ldc)
{
  cblas_dgemm(Order, TransA, TransB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc);
}

template<>
void TMatrix<float>::do_gemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K, const Real alpha, const float *A, const int lda, const float *B, const int ldb, const Real beta, float *C, const int ldc)
{
  cblas_sgemm(Order, TransA, TransB, M, N, K, alpha, A, lda, B, ldb, beta, C, ldc);
}

/** Compute A'A for the given (row major) matrix. Return false on failure. */
template<>
bool TMatrix<real_t>::symmetric(const TMatrix<real_t>& A)
{
  if(!set_sizes(A.col_count(), A.col_count())) return false;
  // you want to understand this line ?
  // visit: http://dev.gaspardbuma.org/en/post191.html
  cblas_dgemm(CblasRowMajor, CblasTrans , CblasNoTrans , A.col_count(), A.col_count(), A.row_count(), 1, A.data, A.col_count(), A.data, A.col_count(), 0.0, data, mColCount);
  return true;
}

/** Compute A'A for the given (row major) matrix. Return false on failure. */
template<>
bool TMatrix<float>::symmetric(const TMatrix<float>& A)
{
  if(!set_sizes(A.col_count(), A.col_count())) return false;
  // you want to understand this line ?
  // visit: http://dev.gaspardbuma.org/en/post191.html
  cblas_sgemm(CblasRowMajor, CblasTrans , CblasNoTrans , A.col_count(), A.col_count(), A.row_count(), 1, A.data, A.col_count(), A.data, A.col_count(), 0.0, data, mColCount);
  return true;
}


/** Set the matrix to the eigenvectors of a symmetric (row major) matrix. Return false on failure.
  * @param pEigenValues will contain the eigenvalues (in ascending order).
  * @param pMatrix source symmetric matrix (will be altered during processing. Send a copy if you want it kept clean).
  */
template<>
bool TMatrix<real_t>::eigenvectors(TMatrix<real_t>& pEigenValues, TMatrix<real_t>& pMatrix)
//bool TMatrix<real_t>::compute_eigenvectors(Real ** pEigenVectors, Real ** pEigenValues, long * pEigenCount, Real * pMatrix, int pSize)
{
  if (pMatrix.col_count() != pMatrix.row_count()) {
    set_error("Cannot compute eigenvectors if source matrix is not symmetric. It is %ix%i.", pMatrix.row_count(), pMatrix.col_count());
    return false;
  }

  size_t col_count = pMatrix.col_count();

  if (!set_sizes(col_count, col_count)) return false;
  if (!pEigenValues.set_sizes(col_count, 1)) {
    set_error("eigenvalues matrix: %s", pEigenValues.error_msg());
    return false;
  }

  // we have to instanciate all parameters as they are passed by reference.
  char jobz  = 'V';       //  1. 'N': compute eigenvalues only, 'V': eigenvalues and eigenvectors.
  char range = 'A';       //  2. 'A': find all eigenvalues,
                          //     'V': find all eigenvalues in the interval ]VL,VU]
                          //     'I': find the eigenvalues with index in [IL,IU]
  char uplo  = 'U';       //  3. symmetric matrix is only stored in the upper triangle. 'L': for lower.
  long n   = col_count;  //  4. Order of the matrix A (size of A is NxN).
  // (A) source           //  5.  source array
  long lda = col_count;  //  6. Leading dimension of A (=N).
  Real vl = 0.0;         //  7. See RANGE. (L stands for Low)
  Real vu = 0.0;         //  8. See RANGE. (U stands for Up)
  long  il = 0.0;         //  9. See RANGE.
  long  iu = 0.0;         // 10. See RANGE.
  Real abstol = 0.00001;    // 11. Absolute error tolerance for the eigenvalues. If set to zero,
                          //     machine precision will be used during convergence test. Higher values = faster but less precise.
  // (M) eigencount       // 12. Output of the number of eigenvalues found.
  // (W) eigenvalues      // 13. Output eigenvalues in ascending order.
  // (Z) eigenvector      // 14. Output array. First M columns contain the orthonormal eigenvectors. i-th column contains the eigenvector associated with the i-th eigenvalue.
  long ldz = col_count;  // 15. Leading dimension of Z (=N).
   long isuppz[2 * col_count];// 16. Output array of integers telling which eigenvectors are nonzero. ??
   Real work[col_count * 40];        // 17. Workspace (real array)
  long lwork = col_count * 40;       // 18. Size of the workspace. Should be >= (NB+6)N where NB is the maximal blocksize for SSYTRD and SORMTR returned by ILAENV
   long iwork[15 * col_count];         // 19. Workspace (integer array)
  long liwork = 15 * col_count;       // 20. Size of IWORK array. Should be >= 10N
  long info = 0;          // 21. Result information. 0 = success, -i: i-th argument had an illegal value, > 0 internal error

  long eigen_count;

  // compute the eigenvectors of pMatrix:
  // you want to understand this line ?
  // visit: http://dev.gaspardbuma.org/en/post191.html
  dsyevr_( &jobz, &range, &uplo, &n, pMatrix.data, &lda, &vl, &vu, &il, &iu, &abstol, &eigen_count, pEigenValues.data, data, &ldz, isuppz, work, &lwork, iwork, &liwork, &info );

  if ((size_t)eigen_count != mRowCount) {
    /// ignore
  }
  return true;
}

/** Manage memory allocation (make sure there is enough space for matrix of size pSize).
  * @return false on memory allocation failure. */
template<typename T>
bool TMatrix<T>::check_alloc(size_t pSize)
{
  if (!mStorageSize) {
    size_t storage;
    if (pSize < BUF_INITIAL_SIZE)
      storage = BUF_INITIAL_SIZE;
    else
      storage = pSize;

    data = (T*)malloc(storage * sizeof(T));
    if (!data) {
      if (!set_error("could not allocate %ix%i", storage, sizeof(T)))
        mErrorMsg = "error during allocation (plus could not allocate buffer for error message)";
      return false;
    }
    mStorageSize = storage;
  } else if (pSize > mStorageSize) {
    return reallocate(pSize);
  }
  return true;
}

/** The size of the matrix changed. We need to increase/decrease memory usage.
  * @return false on memory allocation failure. */
template<typename T>
bool TMatrix<T>::reallocate(size_t pSize)
{
  T * tmp = (T*)realloc(data, pSize * sizeof(T));
  if (!tmp) {
    if(!set_error("could not reallocate %i to %i", mStorageSize, pSize))
      mErrorMsg = "error during reallocation (plus could not allocate buffer for error message)";
    return false;
  }
  data = tmp;
  mStorageSize = pSize;
  return true;
}

/** Copy data using memcpy. (Update size if needed).
  * @param pRowOffset where to start copying (set to mRowCount to append at end). */
template<typename T>
bool TMatrix<T>::raw_copy(size_t pRowOffset, const T * pData, size_t pDataSize, bool pResize)
{
  size_t current_size = pRowOffset * mColCount;
  if(!check_alloc(current_size + pDataSize)) return false;
  if(!mColCount) {
    mColCount = pDataSize;
    mRowCount = 1;
  }
  // use memcpy
  memcpy(data + current_size, pData, pDataSize * sizeof(T));
  if (mRowCount < pRowOffset + pDataSize / mColCount || pResize) {
    mRowCount = pRowOffset + pDataSize / mColCount;
  }
  return true;
}

template<typename T>
bool TMatrix<T>::set_error(const char * fmt, ...)
{
  int n;
  char * np;
  va_list ap;

  if (mErrorBuffer == NULL) {
    mErrorBuffer = (char*)malloc(BUF_START_ERROR_MSG_SIZE * sizeof(char));
    if (mErrorBuffer) mErrorBufferSize = BUF_START_ERROR_MSG_SIZE;
  }

  while (1) {
     /* try to print in the allocated space. */
     va_start(ap, fmt);
     n = vsnprintf (mErrorBuffer, mErrorBufferSize, fmt, ap);
     va_end(ap);
     if (n > -1 && n < (int)mErrorBufferSize) {
       // OK
       mErrorMsg = mErrorBuffer;
       return true;
     }
     // try with more space
     if (n > -1)    // glibc 2.1
        mErrorBufferSize = n+1; // precisely what is needed
     else           // glibc 2.0
        mErrorBufferSize *= 2;  // twice the old size
     if ((np = (char*)realloc (mErrorBuffer, mErrorBufferSize)) == NULL) {
         mErrorBuffer[mErrorBufferSize - 1] = '\0';
         mErrorMsg = "failed to allocate memory for error message !";
        return false; // no more memory... bad
     } else {
        mErrorBuffer = np;
     }
  }
}

template<typename T>
inline bool TMatrix<T>::check_sizes(const char * pMsg, size_t * start_row, size_t * end_row, const TMatrix<T>& pOther, int pStartRow, int pEndRow, bool pAllowColCountChange)
{
  *end_row   = pEndRow   < 0 ? pOther.mRowCount + pEndRow   : pEndRow;
  *start_row = pStartRow < 0 ? pOther.mRowCount + pStartRow : pStartRow;
  if (*start_row >= pOther.mRowCount || *start_row < 0) {
    set_error("size error (%s): bad start row %i (%i) of matrix %ix%i", pMsg, pStartRow, *start_row, pOther.mRowCount, pOther.mColCount);
    return false;
  } else if (*end_row >= pOther.mRowCount || *end_row < 0) {
    set_error("size error (%s): bad end row %i (%i) of matrix %ix%i", pMsg, pEndRow, *end_row, pOther.mRowCount, pOther.mColCount);
    return false;
  } else if (pOther.mColCount != mColCount && !pAllowColCountChange) {
    set_error("size error (%s): source matrix %ix%i, target matrix %ix%i (bad column count)", pOther.mRowCount, pOther.mColCount, mRowCount, mColCount);
    return false;
  } else if (!pOther.mColCount) {
    set_error("size error (%s): source matrix %ix%i is empty.", pMsg, pOther.mRowCount, pOther.mColCount);
    return false;
  }
  return true;
}

template<typename T>
template<typename V>
bool TMatrix<T>::cast_append (const V * pVector, size_t pVectorSize, Real pScale)
{
  if (pVectorSize % mColCount != 0) {
    set_error("could not append vector: column count not matching (%i is not a multiple of %i)", pVectorSize, mColCount);
    return false;
  }
  size_t current_size = mRowCount * mColCount;
  if(!check_alloc(current_size + pVectorSize)) return false;
  // copy
  for(size_t i=0; i< pVectorSize; i++)
    data[current_size + i] = (T)(pVector[i] * pScale);

  mRowCount += pVectorSize / mColCount;
  return true;
}

template<>
bool TMatrix<real_t>::inverse()
{
  TMatrix<long> pivot;
  long info;

  if (!pivot.set_sizes(1, mColCount)) {
    set_error("could not allocate pivot array (%s)", pivot.error_msg());
    return false;
  }
  // LU decomposition (using LAPACK)
  long M = mRowCount;
  long N = mColCount;
  long LDA = mColCount;
  dgetrf_(&M, &N, data, &LDA, pivot.data, &info);
  if (info < 0) {
    set_error("bad argument %i for dgetrf in 'inverse'", -info);
    return false;
  } else if (info > 0) {
    set_error("value %i,%i is zero. Cannot compute inverse.",info,info);
    return false;
  } else {
    // make sure determinant is greater then MATRIX_MIN_DET
    Real det = 1.0;
    for (size_t i = 0; i < (size_t)M; i++) {
      det *= data[i * N + i]; // diagonal entries of LU factors
    }
    if (det > -MATRIX_MIN_DET && det < MATRIX_MIN_DET) {
      set_error("determinant is %.2e. Smaller then MATRIX_MIN_DET (%.1e). Cannot compute inverse.", det, MATRIX_MIN_DET);
      return false;
    }
  }
  // inverse
  long sz = (__CLPK_integer)size();
  Matrix work;
  if (!work.set_sizes(1, 32 * sz)) {
    set_error("could not allocate workspace array (%s)", work.error_msg());
    return false;
  }
  // (using LAPACK)
  long lwork = 32 * mColCount;
  dgetri_(&LDA, data, &LDA, pivot.data, work.data, &lwork, &info);
  if (info < 0) {
    set_error("bad argument %i for dgetri in 'inverse'", -info);
    return false;
  } else if (info > 0) {
    set_error("value %i,%i is zero. Cannot compute inverse.",info,info);
    return false;
  }
  return true;
}





/// explicit instanciation for reals and integers //////

#define TMATRIX_EXPLICIT(T) \
  template bool TMatrix<T>::to_file(const std::string &pPath, const char * pMode, bool isMatrix) const; \
  template bool TMatrix<T>::to_file(FILE * pFile, bool isMatrix) const; \
  template bool TMatrix<T>::from_file(const std::string &pPath, const char * pMode); \
  template bool TMatrix<T>::from_file(FILE * pFile); \
  template bool TMatrix<T>::add(const TMatrix& pOther, int pStartRow, int pEndRow, Real pScale); \
  template bool TMatrix<T>::add(const T * pVector, size_t pVectorSize); \
  template bool TMatrix<T>::add(const TMatrix& A, const TMatrix& B, Real pScaleA, Real pScaleB); \
  template bool TMatrix<T>::append(const T * pVector, size_t pVectorSize); \
  template bool TMatrix<T>::append(T pValue); \
  template bool TMatrix<T>::append(const TMatrix& pOther, int pStartRow, int pEndRow); \
  template bool TMatrix<T>::multiply(const TMatrix& pOther, int pStartRow, int pEndRow, Real pScale); \
  template bool TMatrix<T>::divide(const TMatrix& pOther, int pStartRow, int pEndRow, Real pScale);

TMATRIX_EXPLICIT(char)
TMATRIX_EXPLICIT(int)
TMATRIX_EXPLICIT(real_t)

template bool TMatrix<real_t>::mat_multiply(const TMatrix& A, const TMatrix& B, const enum CBLAS_TRANSPOSE pTransA, const enum CBLAS_TRANSPOSE pTransB, Real pScale);

// cast append
template bool TMatrix< int  >::cast_append<real_t> (const Real * pVector, size_t pVectorSize, Real pScale);
template bool TMatrix<real_t>::cast_append< int  > (const int * pVector, size_t pVectorSize, Real pScale);

// we instanciate those as they can be used to 'scale & copy':
template bool TMatrix<real_t>::cast_append<real_t> (const Real * pVector, size_t pVectorSize, Real pScale);
template bool TMatrix< int  >::cast_append< int  > (const int * pVector, size_t pVectorSize, Real pScale);

// no mat_multiply for integers