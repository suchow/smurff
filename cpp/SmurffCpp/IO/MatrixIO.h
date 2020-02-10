#pragma once

#include <chrono>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <memory>

#include <SmurffCpp/Configs/MatrixConfig.h>

#include <SmurffCpp/Types.h>
#include <SmurffCpp/Types.h>

//
// GitHub issue #34:
//    https://github.com/ExaScience/smurff/issues/34
//
// Python golden stardard:
//    https://github.com/ExaScience/smurff/blob/master-smurff-merge/python/io/matrix_io.py
//

namespace smurff { namespace matrix_io
{
   enum class MatrixType
   {
      //sparse types
      none,
      sdm,
      sbm,
      mtx,

      //dense types
      csv,
      ddm
   };

   MatrixType ExtensionToMatrixType(const std::string& fname);

   std::shared_ptr<MatrixConfig> read_matrix(const std::string& filename, bool isScarce);
   std::shared_ptr<MatrixConfig> read_dense_float64_bin(std::istream& in);
   std::shared_ptr<MatrixConfig> read_dense_float64_csv(std::istream& in);
   std::shared_ptr<MatrixConfig> read_sparse_float64_bin(std::istream& in, bool isScarce);
   std::shared_ptr<MatrixConfig> read_sparse_binary_bin(std::istream& in, bool isScarce);
   std::shared_ptr<MatrixConfig> read_matrix_market(std::istream& in, bool isScarce);

   // ===

   void write_matrix(const std::string& filename, std::shared_ptr<const MatrixConfig> matrixConfig);
   void write_dense_float64_bin(std::ostream& out, std::shared_ptr<const MatrixConfig> matrixConfig);
   void write_dense_float64_csv(std::ostream& out, std::shared_ptr<const MatrixConfig> matrixConfig);
   void write_sparse_float64_bin(std::ostream& out, std::shared_ptr<const MatrixConfig> matrixConfig);
   void write_sparse_binary_bin(std::ostream& out, std::shared_ptr<const MatrixConfig> matrixConfig);
   void write_matrix_market(std::ostream& out, std::shared_ptr<const MatrixConfig> matrixConfig);

   namespace eigen{
      void read_matrix(const std::string& filename, Vector& V);
      void read_matrix(const std::string& filename, Matrix& X);
      void read_matrix(const std::string& filename, SparseMatrix& X);
      void read_matrix_hdf5(const std::string& filename, SparseMatrix& X);

      // ===
      void write_matrix(const std::string& filename, const Matrix& X);
      void write_matrix_hdf5(const std::string& filename, const Matrix& X);
      void write_matrix(const std::string& filename, const SparseMatrix& X);
      void write_matrix_hdf5(const std::string& filename, const SparseMatrix& X);

      // ===
      void read_dense_float64_bin(std::istream &in, Matrix &X);
      void read_dense_float64_csv(std::istream &in, Matrix &X);
      void read_matrix_market(std::istream &in, Matrix &X);
   }
}}
