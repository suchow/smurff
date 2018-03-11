# SMURFF - Scalable Matrix Factorization Framework
[![Build Status](https://travis-ci.org/ExaScience/smurff.svg?branch=master)](https://travis-ci.org/ExaScience/smurff)
[https://anaconda.org/vanderaa/smurff/badges/installer/conda.svg]


## Bayesian Factorization with Side Information

Highly optimized and parallelized methods for Bayesian Factorization, including 
[BPMF](https://www.cs.toronto.edu/~amnih/papers/bpmf.pdf), 
[Macau](https://arxiv.org/abs/1509.04610) and
[GFA](https://arxiv.org/pdf/1411.5799.pdf).
The package uses optimized OpenMP/C++ code with a Cython wrapper to factorize large scale matrices.

Macau method is able to perform **matrix** and **tensor** factorization while
incorporating high-dimensional side information to the factorization.

# Examples
For examples see [documentation](https://github.com/ExaScience/smurff/blob/master/python/jupyter-notebook/smurff.ipynb).

# Installation

Using [conda](http://anaconda.org):

```bash
conda install -c vanderaa smurff 
```

Copmpile from source code: see [INSTALL.md](docs/INSTALL.md)

# Contributors
- Jaak Simm (Macau C++ version, Cython wrapper, Macau MPI version, Tensor factorization)
- Tom Vander Aa (OpenMP optimized BPMF, Matrix Cofactorization and GFA, Code Reorg)
- Adam Arany (Probit noise model)
- Tom Haber (Original BPMF code)
- Andrei Gedich
- Ilya Pasechnikov

# Acknowledgements
This work was partly funded by the European projects ExCAPE (http://excape-h2020.eu) and
EXA2CT, and the Flemish Exaptation project.

