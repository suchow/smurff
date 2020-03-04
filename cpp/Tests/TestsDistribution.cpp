#include "catch.hpp"

#include <SmurffCpp/Types.h>

#include <Utils/Error.h>

#include <SmurffCpp/Utils/MatrixUtils.h>
#include <SmurffCpp/Utils/Distribution.h>

namespace smurff {

namespace mu = smurff::matrix_utils;

TEST_CASE( "mvnormal/covar" ) {
  init_bmrng(1234);

  const int num_samples = 1000;
  Vector mean = mu::make_dense({1, 10} , { 1., 2., 3., 4., 5., 6., 7., 8., 9., 10.});
  Matrix covar = Matrix::Identity(10,10);

  auto randomMatrix = MvNormal(covar, mean, num_samples);

  // check mean
  REQUIRE(mu::equals_vector(randomMatrix.colwise().sum(), num_samples * mean, num_samples/10));

  // check variance
  Matrix centered = (randomMatrix.rowwise() - mean);
  Matrix squared = centered.array().square(); 
  Vector actual_var = squared.colwise().sum() / num_samples;
  REQUIRE(mu::equals_vector(actual_var, covar.diagonal(), 0.1));

}


TEST_CASE( "mvnormal/prec" ) {
  init_bmrng(1234);

  const int num_samples = 1000;
  const double var = 10.;

  Vector mean = mu::make_dense({1, 10} , { 1., 2., 3., 4., 5., 6., 7., 8., 9., 10.});
  Matrix covar = Vector::Constant(10, var).asDiagonal(); /* 0.1 precision == 10. covar */
  Matrix prec = Vector::Constant(10, 1./var).asDiagonal(); /* 0.1 precision == 10. covar */

  REQUIRE(mu::equals(covar.inverse(), prec, 0.001));

  auto randomMatrix = MvNormal_prec(prec, mean, num_samples);

  // check mean
  REQUIRE(mu::equals_vector(randomMatrix.colwise().sum(), num_samples * mean, num_samples));

  // check variance
  Matrix centered = (randomMatrix.rowwise() - mean);
  Matrix squared = centered.array().square(); 
  Vector actual_var = squared.colwise().sum() / num_samples;
  REQUIRE(mu::equals_vector(actual_var, covar.diagonal(), 0.1));

}


TEST_CASE( "CondNormalWishart" ) {
  init_bmrng(1234);
  Vector mean = matrix_utils::make_dense({1, 3} , { 1., 2., 3. });
  Matrix T = Matrix::Identity(3,3);
  int kappa = 2;
  int nu = 4;
  Matrix U(mu::make_dense({4, 3}, {1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.}));

  auto N = U.rows();
  auto NS = U.transpose() * U;
  auto NU = U.colwise().sum();

  REQUIRE(NS.rows() == NS.cols());
  REQUIRE(NS.cols() == 3);
  REQUIRE(NU.cols() == 3);
  REQUIRE(U.cols() == 3);

// should be the same
  auto p2 = CondNormalWishart(U, mean, kappa, T, nu);
  auto p1 = CondNormalWishart(N, NS, NU, mean, kappa, T, nu);
}

}