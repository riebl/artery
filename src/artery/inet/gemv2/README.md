# GEMV^2

**G**eometry-based **E**fficient propagation **M**odel for **V2V** communication


## The original GEMV^2

GEMV^2 has been developed by [Mate Boban](http://mateboban.net) while working on his Ph.D. thesis.
There are three chief sources of information about this model:

- The [GEMV^2 website](http://vehicle2x.net):
  You can download the Matlab implementation as well as the user manual of GEMV^2 from there.
- Studying the code of this Matlab reference implementation
- An article explaining some concepts behind the model:
  > Mate Boban, Joao Barros, and Ozan K. Tonguz: "Geometry-Based Vehicle-to-Vehicle Channel Modeling for Large-Scale Simulation",
  > IEEE Transactions on Vehicular Technology, Volume 63, Number 9, November 2014 (doi: 10.1109/TVT.2014.2317803)

  A copy of this article is also linked at the website.


## GEMV^2 in Artery

This C++ implementation of GEMV^2 has been developed by Thiago C. Vieira (Universidade Federal do Parana - UFPR) and Raphael Riebl (Technische Hochschule Ingolstadt - THI).
Though some ideas such as the usage of R-Trees are used by both - Mate Boban's Matlab code and Artery - this implemenation is actually a complete rewrite.
While the Matlab implementation computes received power for each communication pair per time step at once, we compute the signal attenuation per transmission by implementing INET's *IPathLoss* interface.

